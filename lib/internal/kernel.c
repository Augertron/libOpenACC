
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

acc_context_t acc_create_context(acc_region_t region, acc_kernel_t kernel) {
  acc_context_t result = (acc_context_t)malloc(sizeof(struct acc_context_t_) + kernel->desc->num_loops * sizeof(struct acc_kernel_loop_t_));

  result->num_gang      = region->num_gang[0];
  result->num_worker    = region->num_worker[0];
  result->vector_length = region->vector_length;

  result->num_loop = kernel->desc->num_loops;

  unsigned i;
  for (i = 0; i < kernel->desc->num_loops; i++) {
    memcpy(&(result->loops[i].original), kernel->loops[i], sizeof(struct acc_loop_desc_t_));
    if (result->loops[i].original.nbr_it == 0)
      result->loops[i].original.nbr_it = (result->loops[i].original.upper - result->loops[i].original.lower) / result->loops[i].original.stride;
  }

  return result; 
}

cl_kernel acc_build_ocl_kernel(acc_region_t region, acc_kernel_t kernel, acc_context_t context) {

  unsigned best_matching_version = 0; // version #0 is always the generic (default) version 
  unsigned best_matching_score = 0;
  struct acc_kernel_loop_t_ * best_matching_loops;

  printf("acc_match_kernel_version\n");

  unsigned i, j;
  for (i = 1; i < kernel->desc->num_versions; i++) {

    printf("  version %u\n", i);

    if (    ( kernel->desc->versions[i]->num_gang      == 0 || kernel->desc->versions[i]->num_gang      == context->num_gang      )
         && ( kernel->desc->versions[i]->num_worker    == 0 || kernel->desc->versions[i]->num_worker    == context->num_worker    )
         && ( kernel->desc->versions[i]->vector_length == 0 || kernel->desc->versions[i]->vector_length == context->vector_length )
    ) {

      assert(kernel->desc->versions[i]->loops != NULL);

      struct acc_kernel_loop_t_ * ctx_loops = (struct acc_kernel_loop_t_ *)malloc(context->num_loop * sizeof(struct acc_kernel_loop_t_));

      printf("  scan loops\n");

      for (j = 0; j < context->num_loop; j++) {
        struct acc_loop_t_ * loop = &(kernel->desc->versions[i]->loops[j]);

        printf("    loop %u\n", j);

        unsigned num_dynamic = 0;
        if (loop->tiles[e_tile_0].tiling == e_tiling_dynamic) num_dynamic++;
        if (loop->tiles[e_tile_1].tiling == e_tiling_dynamic) num_dynamic++;
        if (loop->tiles[e_tile_2].tiling == e_tiling_dynamic) num_dynamic++;
        if (loop->tiles[e_tile_3].tiling == e_tiling_dynamic) num_dynamic++;

        printf("    num_dynamic %u\n", num_dynamic);

        size_t tile;

        // Copy tiles parameter from version descriptor
        for (tile = 0; tile < 7; tile++) {
          ctx_loops[j].tiles[tile].stride = loop->tiles[tile].stride;
          ctx_loops[j].tiles[tile].length = loop->tiles[tile].nbr_it;
        }

        // Set/Check gang tile's stride
        if (ctx_loops[j].tiles[e_gang].length == 0)
          ctx_loops[j].tiles[e_gang].length = context->num_gang;
        assert(ctx_loops[j].tiles[e_gang].length == context->num_gang);

        // Set/Check worker tile's stride
        if (ctx_loops[j].tiles[e_worker].length == 0)
          ctx_loops[j].tiles[e_worker].length = context->num_worker;
        assert(ctx_loops[j].tiles[e_worker].length == context->num_worker);

        // Set/Check vector tile's stride
        if (ctx_loops[j].tiles[e_vector].length == 0)
          ctx_loops[j].tiles[e_vector].length = context->vector_length;
        assert(ctx_loops[j].tiles[e_vector].length == context->vector_length);

        unsigned long loop_length = kernel->loops[j]->upper - kernel->loops[j]->lower;

        if (num_dynamic == 0) {
          for (tile = 0; tile < 7; tile++) {
            unsigned long tile_length;
            if (tile == 0)
              tile_length = loop_length;
            else
              tile_length = ctx_loops[j].tiles[tile - 1].stride;

            if (ctx_loops[j].tiles[tile].stride != 0 && ctx_loops[j].tiles[tile].length == 0)
              ctx_loops[j].tiles[tile].length = tile_length / ctx_loops[j].tiles[tile].stride;
            else if (ctx_loops[j].tiles[tile].stride == 0 && ctx_loops[j].tiles[tile].length != 0)
              ctx_loops[j].tiles[tile].stride = tile_length / ctx_loops[j].tiles[tile].length;

            assert(ctx_loops[j].tiles[tile].stride != 0 && ctx_loops[j].tiles[tile].length != 0);

            if (ctx_loops[j].tiles[tile].length != tile_length / ctx_loops[j].tiles[tile].stride)
              break;
          }
          if (tile < 7) break;
        }
        else if (num_dynamic == 1) {

          size_t dynamic_tile;
          if      (loop->tiles[e_tile_0].tiling == e_tiling_dynamic) dynamic_tile = e_tile_0;
          else if (loop->tiles[e_tile_1].tiling == e_tiling_dynamic) dynamic_tile = e_tile_1;
          else if (loop->tiles[e_tile_2].tiling == e_tiling_dynamic) dynamic_tile = e_tile_2;
          else if (loop->tiles[e_tile_3].tiling == e_tiling_dynamic) dynamic_tile = e_tile_3;
          else assert(!"Problem with dynamic tiling...");

          printf("    dynamic_tile %u\n", dynamic_tile);

          // Propagate value from outer tile to the dynamic tile
          for (tile = 0; tile < dynamic_tile; tile++) {
            unsigned long tile_length;
            if (tile == 0)
              tile_length = loop_length;
            else
              tile_length = ctx_loops[j].tiles[tile - 1].stride;

            if (ctx_loops[j].tiles[tile].stride != 0 && ctx_loops[j].tiles[tile].length == 0)
              ctx_loops[j].tiles[tile].length = tile_length / ctx_loops[j].tiles[tile].stride;
            else if (ctx_loops[j].tiles[tile].stride == 0 && ctx_loops[j].tiles[tile].length != 0)
              ctx_loops[j].tiles[tile].stride = tile_length / ctx_loops[j].tiles[tile].length;

            assert(ctx_loops[j].tiles[tile].stride != 0 && ctx_loops[j].tiles[tile].length != 0);
          }

          // Propagate value from outer tile to the dynamic tile
          for (tile = 6; tile > dynamic_tile; tile--) {
            if (ctx_loops[j].tiles[tile].length == 0) break; // if dynamic(tile[i]) then j>i => tile[j].length != 0.

            unsigned long stride;
            if (tile == 6)
              stride = kernel->loops[j]->stride;
            else
              stride = ctx_loops[j].tiles[tile + 1].stride * ctx_loops[j].tiles[tile + 1].length;

            if (ctx_loops[j].tiles[tile].stride != 0) {
              assert(ctx_loops[j].tiles[tile].stride == stride);
            }
            else if (ctx_loops[j].tiles[tile].stride == 0)
              ctx_loops[j].tiles[tile].stride = stride;

            assert(ctx_loops[j].tiles[tile].stride != 0 && ctx_loops[j].tiles[tile].length != 0);
          }
          if (tile > dynamic_tile) break;

          if (dynamic_tile == e_tile_0) {
            ctx_loops[j].tiles[e_tile_0].stride = ctx_loops[j].tiles[e_gang  ].length * ctx_loops[j].tiles[e_gang      ].stride;
            ctx_loops[j].tiles[e_tile_0].length = loop_length / ctx_loops[j].tiles[e_tile_0].stride;
          }
          else if (dynamic_tile == e_tile_1) {
            ctx_loops[j].tiles[e_tile_1].stride = ctx_loops[j].tiles[e_worker].length * ctx_loops[j].tiles[e_worker].stride;
            ctx_loops[j].tiles[e_tile_1].length = ctx_loops[j].tiles[e_gang  ].stride / ctx_loops[j].tiles[e_tile_1].stride;
          }
          else if (dynamic_tile == e_tile_2) {
            ctx_loops[j].tiles[e_tile_2].stride = ctx_loops[j].tiles[e_vector].length * ctx_loops[j].tiles[e_vector].stride;
            ctx_loops[j].tiles[e_tile_2].length = ctx_loops[j].tiles[e_worker].stride / ctx_loops[j].tiles[e_tile_2].stride;
          }
          else if (dynamic_tile == e_tile_3) {
            ctx_loops[j].tiles[e_tile_3].stride = kernel->loops[j]->stride;
            ctx_loops[j].tiles[e_tile_3].length = ctx_loops[j].tiles[e_vector].stride / ctx_loops[j].tiles[e_tile_3].stride;
          }
          else assert(!"The dynamic tile should have been one of the four filling tile.");
        }
        else if (num_dynamic > 1) assert(!"NIY! More than ONE dynamic filling tile...");

        assert(ctx_loops[j].tiles[e_tile_3].stride == kernel->loops[j]->stride);
      }
      if (j < kernel->desc->num_loops) {
        free(ctx_loops);
        continue;
      }

      // Compute the score for version[i]

      unsigned score = 0;

      for (j = 0; j < context->num_loop; j++) {
        struct acc_loop_t_ * loop = &(kernel->desc->versions[i]->loops[j]);

        if (kernel->desc->versions[i]->num_gang      == context->num_gang     ) score++;
        if (kernel->desc->versions[i]->num_worker    == context->num_worker   ) score++;
        if (kernel->desc->versions[i]->vector_length == context->vector_length) score++;

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
        best_matching_version = i;
        best_matching_score = score;
        best_matching_loops = ctx_loops;
      }
    }
  }

  assert(best_matching_version != 0); // default version disable for now (for checking purpose)
  assert(best_matching_loops != NULL);

  for (i = 0; i < context->num_loop; i++)
    for (j = 0; j < 7; j++)
      best_matching_loops[i].tiles[j].length = best_matching_loops[i].tiles[j].length * best_matching_loops[i].tiles[j].stride;

  memcpy(context->loops, best_matching_loops, context->num_loop * sizeof(struct acc_kernel_loop_t_));

  // Build the kernel name 
  char * version_suffix = kernel->desc->versions[best_matching_version]->suffix;
  char * kernel_name = (char *)malloc((strlen(kernel->desc->name) + strlen(version_suffix) + 1) * sizeof(char));
  strcpy(kernel_name, kernel->desc->name);
  strcat(kernel_name, version_suffix);

  cl_program program = acc_runtime.opencl_data->devices_data[region->device_idx]->programs[region->desc->id];

  assert(program != NULL);

  cl_int status;
  cl_kernel ocl_kernel = clCreateKernel(program, kernel_name, &status);
  if (status != CL_SUCCESS) {
    printf("[fatal]   clCreateKernel return %u for %s.\n", status, kernel_name);
    exit(-1); /// \todo error code
  }

  return ocl_kernel;
}

