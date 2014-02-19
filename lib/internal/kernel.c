
#include "OpenACC/internal/region.h"
#include "OpenACC/internal/kernel.h"

#include "OpenACC/device/host-context.h"

#include "OpenACC/private/kernel.h"
#include "OpenACC/internal/kernel.h"

#include "OpenACC/internal/loop.h"

#include "OpenACC/private/runtime.h"
#include "OpenACC/private/debug.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <assert.h>

typedef struct acc_region_t_ * acc_region_t;
typedef struct acc_kernel_t_ * acc_kernel_t;
typedef struct acc_context_t_ * acc_context_t;

acc_context_t acc_create_context(acc_region_t region, acc_kernel_t kernel, size_t device_idx) {
  acc_context_t result = (acc_context_t)malloc(sizeof(struct acc_context_t_) + kernel->desc->num_loops * sizeof(struct acc_kernel_loop_t_));

  result->num_gang      = region->devices[device_idx].num_gang;
  result->num_worker    = region->devices[device_idx].num_worker;
  result->vector_length = region->devices[device_idx].vector_length;

  result->num_loop = kernel->desc->num_loops;

  unsigned i;
  for (i = 0; i < kernel->desc->num_loops; i++) {
    memcpy(&(result->loops[i].original), kernel->loops[i], sizeof(struct acc_loop_desc_t_));
    if (result->loops[i].original.nbr_it == 0)
      result->loops[i].original.nbr_it = (result->loops[i].original.upper - result->loops[i].original.lower) / result->loops[i].original.stride;
  }

  return result; 
}

static int version_match(acc_kernel_version_t version, acc_context_t context) {
  return ( version->num_gang      == 0 || version->num_gang      == context->num_gang      )
      && ( version->num_worker    == 0 || version->num_worker    == context->num_worker    )
      && ( version->vector_length == 0 || version->vector_length == context->vector_length );
}

static unsigned num_dynamic_tiles(struct acc_loop_t_ * loop) {
  unsigned num_dynamic = 0;

  if (loop->tiles[e_tile_0].tiling == e_tiling_dynamic) num_dynamic++;
  if (loop->tiles[e_tile_1].tiling == e_tiling_dynamic) num_dynamic++;
  if (loop->tiles[e_tile_2].tiling == e_tiling_dynamic) num_dynamic++;
  if (loop->tiles[e_tile_3].tiling == e_tiling_dynamic) num_dynamic++;

  return num_dynamic;
}

static void init_ctx_loop(struct acc_loop_t_ * vers_loop, acc_context_t context, struct acc_kernel_loop_t_ * ctx_loop) {
  size_t tile;

  // Copy tiles parameter from version descriptor
  for (tile = 0; tile < 7; tile++) {
    ctx_loop->tiles[tile].stride = vers_loop->tiles[tile].stride;
    ctx_loop->tiles[tile].length = vers_loop->tiles[tile].nbr_it;
  }

  // Set/Check gang tile's stride
  if (ctx_loop->tiles[e_gang].length == 0)
    ctx_loop->tiles[e_gang].length = context->num_gang;
  assert(ctx_loop->tiles[e_gang].length == context->num_gang);

  // Set/Check worker tile's stride
  if (ctx_loop->tiles[e_worker].length == 0)
    ctx_loop->tiles[e_worker].length = context->num_worker;
  assert(ctx_loop->tiles[e_worker].length == context->num_worker);

  // Set/Check vector tile's stride
  if (ctx_loop->tiles[e_vector].length == 0)
    ctx_loop->tiles[e_vector].length = context->vector_length;
  assert(ctx_loop->tiles[e_vector].length == context->vector_length);
}

static size_t fill_ctx_loop_no_dynamic(struct acc_kernel_loop_t_ * ctx_loop, unsigned long loop_length) {
  size_t tile;

  for (tile = 0; tile < 7; tile++) {
    unsigned long tile_length;
    if (tile == 0)
      tile_length = loop_length;
    else
      tile_length = ctx_loop->tiles[tile - 1].stride;

    if (ctx_loop->tiles[tile].stride != 0 && ctx_loop->tiles[tile].length == 0)
      ctx_loop->tiles[tile].length = tile_length / ctx_loop->tiles[tile].stride;
    else if (ctx_loop->tiles[tile].stride == 0 && ctx_loop->tiles[tile].length != 0)
      ctx_loop->tiles[tile].stride = tile_length / ctx_loop->tiles[tile].length;

    assert(ctx_loop->tiles[tile].stride != 0 && ctx_loop->tiles[tile].length != 0);

    if (ctx_loop->tiles[tile].length != tile_length / ctx_loop->tiles[tile].stride)
      break;
  }

  return tile;
}

static size_t fill_ctx_loop_one_dynamic(
  struct acc_loop_t_ * vers_loop,
  acc_loop_desc_t orig_loop,
  struct acc_kernel_loop_t_ * ctx_loop,
  unsigned long loop_length
) {
  size_t dynamic_tile;
  if      (vers_loop->tiles[e_tile_0].tiling == e_tiling_dynamic) dynamic_tile = e_tile_0;
  else if (vers_loop->tiles[e_tile_1].tiling == e_tiling_dynamic) dynamic_tile = e_tile_1;
  else if (vers_loop->tiles[e_tile_2].tiling == e_tiling_dynamic) dynamic_tile = e_tile_2;
  else if (vers_loop->tiles[e_tile_3].tiling == e_tiling_dynamic) dynamic_tile = e_tile_3;
  else assert(!"Problem with dynamic tiling...");

  // Propagate value from outer tile to the dynamic tile
  size_t tile;
  for (tile = 0; tile < dynamic_tile; tile++) {
    unsigned long tile_length;
    if (tile == 0)
      tile_length = loop_length;
    else
      tile_length = ctx_loop->tiles[tile - 1].stride;

    if (ctx_loop->tiles[tile].stride != 0 && ctx_loop->tiles[tile].length == 0)
      ctx_loop->tiles[tile].length = tile_length / ctx_loop->tiles[tile].stride;
    else if (ctx_loop->tiles[tile].stride == 0 && ctx_loop->tiles[tile].length != 0)
      ctx_loop->tiles[tile].stride = tile_length / ctx_loop->tiles[tile].length;

    assert(ctx_loop->tiles[tile].stride != 0 && ctx_loop->tiles[tile].length != 0);
  }

  // Propagate value from inner tile to the dynamic tile
  for (tile = 6; tile > dynamic_tile; tile--) {
    if (ctx_loop->tiles[tile].length == 0) break; // if dynamic(tile[i]) then j>i => tile[j].length != 0.

    unsigned long stride;
    if (tile == 6)
      stride = orig_loop->stride;
    else
      stride = ctx_loop->tiles[tile + 1].stride * ctx_loop->tiles[tile + 1].length;

    if (ctx_loop->tiles[tile].stride != 0) {
      assert(ctx_loop->tiles[tile].stride == stride);
    }
    else if (ctx_loop->tiles[tile].stride == 0)
      ctx_loop->tiles[tile].stride = stride;

    assert(ctx_loop->tiles[tile].stride != 0 && ctx_loop->tiles[tile].length != 0);
  }

  if (tile > dynamic_tile) return 0;

  // Middle point
  if (dynamic_tile == e_tile_0) {
    ctx_loop->tiles[e_tile_0].stride = ctx_loop->tiles[e_gang].length * ctx_loop->tiles[e_gang].stride;
    ctx_loop->tiles[e_tile_0].length = loop_length / ctx_loop->tiles[e_tile_0].stride;
  }
  else if (dynamic_tile == e_tile_1) {
    ctx_loop->tiles[e_tile_1].stride = ctx_loop->tiles[e_worker].length * ctx_loop->tiles[e_worker].stride;
    ctx_loop->tiles[e_tile_1].length = ctx_loop->tiles[e_gang  ].stride / ctx_loop->tiles[e_tile_1].stride;
  }
  else if (dynamic_tile == e_tile_2) {
    ctx_loop->tiles[e_tile_2].stride = ctx_loop->tiles[e_vector].length * ctx_loop->tiles[e_vector].stride;
    ctx_loop->tiles[e_tile_2].length = ctx_loop->tiles[e_worker].stride / ctx_loop->tiles[e_tile_2].stride;
  }
  else if (dynamic_tile == e_tile_3) {
    ctx_loop->tiles[e_tile_3].stride = orig_loop->stride;
    ctx_loop->tiles[e_tile_3].length = ctx_loop->tiles[e_vector].stride / ctx_loop->tiles[e_tile_3].stride;
  }
  else assert(!"Dynamic tile should have been one of the four filling tile.");

  return 1;
}

void acc_select_kernel_version(
  acc_region_t region,
  acc_kernel_t kernel,
  acc_context_t context,
  size_t device_idx,
  unsigned * best_matching_version,
  struct acc_kernel_loop_t_ ** best_matching_loops
) {
  unsigned best_matching_score = 0;

  unsigned version_idx;
  for (version_idx = 0; version_idx < kernel->desc->num_versions; version_idx++) {

//  printf("  version %u\n", version_idx);

    if (version_match(kernel->desc->versions[version_idx], context)) {

      assert(kernel->desc->versions[version_idx]->loops != NULL);

      struct acc_kernel_loop_t_ * ctx_loops = (struct acc_kernel_loop_t_ *)malloc(context->num_loop * sizeof(struct acc_kernel_loop_t_));

      unsigned loop_idx;
      for (loop_idx = 0; loop_idx < context->num_loop; loop_idx++) {

//      printf("    loop %u\n", loop_idx);

        struct acc_loop_t_ *        vers_loop = &(kernel->desc->versions[version_idx]->loops[loop_idx]);
        struct acc_kernel_loop_t_ * ctx_loop  = &(ctx_loops[loop_idx]);
        acc_loop_desc_t orig_loop = kernel->loops[loop_idx];

        init_ctx_loop(vers_loop, context, ctx_loop);

        unsigned long loop_length = orig_loop->upper - orig_loop->lower;
        unsigned num_dynamic = num_dynamic_tiles(vers_loop);
        if (num_dynamic == 0 && fill_ctx_loop_no_dynamic(ctx_loop, loop_length) < 7) break;
        else if (num_dynamic == 1 && !fill_ctx_loop_one_dynamic(vers_loop, orig_loop, ctx_loop, loop_length)) break;
        else if (num_dynamic > 1) assert(!"NIY! More than ONE dynamic filling tile...");

        assert(ctx_loop->tiles[e_tile_3].stride == orig_loop->stride);
      }
      if (loop_idx < kernel->desc->num_loops) {
        free(ctx_loops);
        continue;
      }

      // Compute the score for version[version_idx]

      unsigned score = 0;

      for (loop_idx = 0; loop_idx < context->num_loop; loop_idx++) {
        struct acc_loop_t_ * loop = &(kernel->desc->versions[version_idx]->loops[loop_idx]);

        if (kernel->desc->versions[version_idx]->num_gang      == context->num_gang     ) score++;
        if (kernel->desc->versions[version_idx]->num_worker    == context->num_worker   ) score++;
        if (kernel->desc->versions[version_idx]->vector_length == context->vector_length) score++;

        size_t tile;

        for (tile = 0; tile < 7; tile++) {
          switch (loop->tiles[tile].tiling) {
            case e_tiling_dynamic:
              break;
            case e_tiling_static_iteration:
              score += 1;
              break;
            case e_tiling_static_stride:
              score += 1;
              break;
            case e_tiling_static:
              score += 2;
              break;
            case e_no_tiling:
              assert(!"Should not be used!");
          }
        }
      }

      if (score >= best_matching_score) {
        best_matching_score = score;

        *best_matching_version = version_idx;

        if (*best_matching_loops != NULL)
          free(*best_matching_loops);
        *best_matching_loops = ctx_loops;
      }
      else {
        free(ctx_loops);
      }
    }
  }
}

cl_kernel acc_build_ocl_kernel(acc_region_t region, acc_kernel_t kernel, acc_context_t context, size_t device_idx) {
  unsigned best_matching_version = 0;
  struct acc_kernel_loop_t_ * best_matching_loops = NULL;

  acc_select_kernel_version(region, kernel, context, device_idx, &best_matching_version, &best_matching_loops);

  assert(best_matching_loops != NULL);

  unsigned i, j;
  for (i = 0; i < context->num_loop; i++)
    for (j = 0; j < 7; j++)
      best_matching_loops[i].tiles[j].length = best_matching_loops[i].tiles[j].length * best_matching_loops[i].tiles[j].stride;

  memcpy(context->loops, best_matching_loops, context->num_loop * sizeof(struct acc_kernel_loop_t_));

  // Build the kernel name 
  char * version_suffix = kernel->desc->versions[best_matching_version]->suffix;
  char * kernel_name = (char *)malloc((strlen(kernel->desc->name) + strlen(version_suffix) + 1) * sizeof(char));
  strcpy(kernel_name, kernel->desc->name);
  strcat(kernel_name, version_suffix);

  cl_program program = acc_runtime.opencl_data->devices_data[device_idx]->programs[region->desc->id];

  assert(program != NULL);

  cl_int status;
  cl_kernel ocl_kernel = clCreateKernel(program, kernel_name, &status);
  if (status != CL_SUCCESS) {
    printf("[fatal]   clCreateKernel return %u for %s.\n", status, kernel_name);
    exit(-1); /// \todo error code
  }

  return ocl_kernel;
}

