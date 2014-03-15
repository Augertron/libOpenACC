
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

struct loop_triplet_t {
  unsigned long length;
  unsigned long nbr_it;
  unsigned long stride;
};

acc_context_t acc_create_context(acc_region_t region, acc_kernel_t kernel, size_t device_idx) {
  acc_context_t result = (acc_context_t)malloc(sizeof(struct acc_context_t_) + kernel->desc->num_loops * sizeof(struct acc_kernel_loop_t_));

  unsigned i;

  for (i = 0; i < region->num_devices; i++)
    if (region->devices[i].device_idx == device_idx)
      break;
  assert(i < region->num_devices);
  result->num_gang      = region->devices[i].num_gang;
  result->num_worker    = region->devices[i].num_worker;
  result->vector_length = region->devices[i].vector_length;

  result->num_loop = kernel->desc->num_loops;

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

  if (loop->num_iterations[e_tile_0] == 0) num_dynamic++;
  if (loop->num_iterations[e_tile_1] == 0) num_dynamic++;
  if (loop->num_iterations[e_tile_2] == 0) num_dynamic++;
  if (loop->num_iterations[e_tile_3] == 0) num_dynamic++;

  return num_dynamic;
}

static void init_loop_triplet(
  struct loop_triplet_t * loop_triplet,
  struct acc_loop_t_ * vers_loop,
  acc_context_t context,
  acc_loop_desc_t orig_loop
) {
  size_t tile;

  // Initialize array of loop triplet
  for (tile = 0; tile < 7; tile++) {
    loop_triplet[tile].length = 0;
    loop_triplet[tile].nbr_it = vers_loop->num_iterations[tile];
    loop_triplet[tile].stride = 0;
  }

  // Set/Check gang tile's number of iteration
  if (loop_triplet[e_gang].nbr_it == 0)
    loop_triplet[e_gang].nbr_it = context->num_gang;

  // Set/Check worker tile's number of iteration
  if (loop_triplet[e_worker].nbr_it == 0)
    loop_triplet[e_worker].nbr_it = context->num_worker;

  // Set/Check vector tile's number of iteration
  if (loop_triplet[e_vector].nbr_it == 0)
    loop_triplet[e_vector].nbr_it = context->vector_length;

  // Set length of first tile to length of the original loop
  loop_triplet[0].length = orig_loop->upper - orig_loop->lower;

  // Set stride of last tile to stride of the original loop
  loop_triplet[6].stride = orig_loop->stride;
}

void solve_no_dynamic(struct loop_triplet_t * loop_triplet) {
  size_t tile;

  assert(loop_triplet[6].stride != 0);
  assert(loop_triplet[6].nbr_it != 0);

  loop_triplet[6].length = loop_triplet[6].nbr_it * loop_triplet[6].stride;

  for (tile = 5; tile > 0; tile++) {
    assert(loop_triplet[tile].nbr_it != 0);

    loop_triplet[tile].stride = loop_triplet[tile+1].length;
    loop_triplet[tile].length = loop_triplet[tile].nbr_it * loop_triplet[tile].stride;
  }

  assert(loop_triplet[0].nbr_it != 0);

  loop_triplet[0].stride = loop_triplet[1].length;

  assert(loop_triplet[0].length == loop_triplet[0].nbr_it * loop_triplet[0].stride);
}

void solve_one_dynamic(
  struct loop_triplet_t * loop_triplet
) {
  size_t dynamic_tile;
  if      (loop_triplet[e_tile_0].nbr_it == 0) dynamic_tile = e_tile_0;
  else if (loop_triplet[e_tile_1].nbr_it == 0) dynamic_tile = e_tile_1;
  else if (loop_triplet[e_tile_2].nbr_it == 0) dynamic_tile = e_tile_2;
  else if (loop_triplet[e_tile_3].nbr_it == 0) dynamic_tile = e_tile_3;
  else assert(!"Problem with dynamic tiling...");

  // Propagate value from outer tile to the dynamic tile
  size_t tile;
  for (tile = 0; tile < dynamic_tile; tile++) {
    assert(loop_triplet[tile].nbr_it != 0);

    if (tile > 0)
      loop_triplet[tile].length = loop_triplet[tile - 1].stride;

    loop_triplet[tile].stride = loop_triplet[tile].length / loop_triplet[tile].nbr_it;
  }

  // Propagate value from inner tile to the dynamic tile
  for (tile = 6; tile > dynamic_tile; tile--) {
    assert(loop_triplet[tile].nbr_it != 0);

    if (tile < 6)
      loop_triplet[tile].stride = loop_triplet[tile + 1].length;

    loop_triplet[tile].length = loop_triplet[tile].nbr_it * loop_triplet[tile].stride;
  }

  // Middle point
  if (dynamic_tile == e_tile_0) {
    assert(loop_triplet[e_tile_0].length != 0);
    loop_triplet[e_tile_0].stride = loop_triplet[e_gang  ].length;
  }
  else if (dynamic_tile == e_tile_1) {
    loop_triplet[e_tile_1].length = loop_triplet[e_gang  ].stride;
    loop_triplet[e_tile_1].stride = loop_triplet[e_worker].length;
  }
  else if (dynamic_tile == e_tile_2) {
    loop_triplet[e_tile_2].length = loop_triplet[e_worker].stride;
    loop_triplet[e_tile_2].stride = loop_triplet[e_vector].length;
  }
  else if (dynamic_tile == e_tile_3) {
    assert(loop_triplet[e_tile_3].stride != 0);
    loop_triplet[e_tile_2].length = loop_triplet[e_vector].stride;
  }
  else assert(!"Dynamic tile should have been one of the four filling tile.");

  loop_triplet[dynamic_tile].nbr_it = loop_triplet[dynamic_tile].length / loop_triplet[dynamic_tile].stride;
}

size_t set_ctx_loop(struct acc_kernel_loop_t_ * ctx_loop, struct loop_triplet_t * loop_triplet) {
  size_t tile;

  for (tile = 0; tile < 7; tile++) {
    if (loop_triplet[tile].length == 0 || loop_triplet[tile].nbr_it == 0 || loop_triplet[tile].stride == 0) return tile;

    ctx_loop->tiles[tile].length = loop_triplet[tile].length;
    ctx_loop->tiles[tile].stride = loop_triplet[tile].stride;
  }

  return tile;
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

    if (version_match(kernel->desc->versions[version_idx], context)) {

      assert(kernel->desc->versions[version_idx]->loops != NULL);

      struct acc_kernel_loop_t_ * ctx_loops = (struct acc_kernel_loop_t_ *)malloc(context->num_loop * sizeof(struct acc_kernel_loop_t_));
      memcpy(ctx_loops, context->loops, context->num_loop * sizeof(struct acc_kernel_loop_t_));

      unsigned loop_idx;
      for (loop_idx = 0; loop_idx < context->num_loop; loop_idx++) {
        struct loop_triplet_t loop_triplet[7];

        struct acc_loop_t_ * vers_loop = &(kernel->desc->versions[version_idx]->loops[loop_idx]);
        acc_loop_desc_t orig_loop = kernel->loops[loop_idx];

        init_loop_triplet(loop_triplet, vers_loop, context, orig_loop);

        unsigned num_dynamic = num_dynamic_tiles(vers_loop);
        if (num_dynamic == 0) {
          solve_no_dynamic(loop_triplet);
          if (set_ctx_loop(&(ctx_loops[loop_idx]), loop_triplet) < 7)
            break;
        }
        else if (num_dynamic == 1) {
          solve_one_dynamic(loop_triplet);
          if (set_ctx_loop(&(ctx_loops[loop_idx]), loop_triplet) < 7)
            break;
        }
        else if (num_dynamic == 2) {
          assert(0); /// \todo pick one then the other and try static values
        }
        else assert(!"NIY! More than ONE dynamic filling tile...");
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

        for (tile = 0; tile < 7; tile++)
          if (loop->num_iterations[tile] != 0)
            score++;
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

struct cl_kernel_ * acc_build_ocl_kernel(acc_region_t region, acc_kernel_t kernel, acc_context_t context, size_t device_idx) {
  unsigned best_matching_version = 0;
  struct acc_kernel_loop_t_ * best_matching_loops = NULL;

  acc_select_kernel_version(region, kernel, context, device_idx, &best_matching_version, &best_matching_loops);

  assert(best_matching_loops != NULL);

  memcpy(context->loops, best_matching_loops, context->num_loop * sizeof(struct acc_kernel_loop_t_));

  // Build the kernel name 
  char * version_suffix = kernel->desc->versions[best_matching_version]->suffix;
  char * kernel_name = (char *)malloc((strlen(kernel->desc->name) + strlen(version_suffix) + 1) * sizeof(char));
  strcpy(kernel_name, kernel->desc->name);
  strcat(kernel_name, version_suffix);

  cl_program program = acc_runtime.opencl_data->devices_data[device_idx]->programs[region->desc->id];

  assert(program != NULL);

  cl_int status;
  struct cl_kernel_ * ocl_kernel = clCreateKernel(program, kernel_name, &status);
  if (status != CL_SUCCESS) {
    printf("[fatal]   clCreateKernel return %u for %s.\n", status, kernel_name);
    exit(-1); /// \todo error code
  }

  return ocl_kernel;
}

