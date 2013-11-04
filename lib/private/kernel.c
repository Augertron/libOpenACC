
#include "OpenACC/internal/region.h"
#include "OpenACC/internal/kernel.h"
#include "OpenACC/internal/loop.h"
#include "OpenACC/device/host-context.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

typedef struct acc_kernel_t_ * acc_kernel_t;
typedef struct acc_region_t_ * acc_region_t;

acc_kernel_t acc_build_kernel(acc_kernel_desc_t kernel) {
  acc_init_once();

  acc_kernel_t result = (acc_kernel_t)malloc(sizeof(struct acc_kernel_t_));

  result->desc = kernel;

  result->scalar_ptrs = (  void **)malloc(kernel->num_scalars * sizeof(  void *));
  result->data_ptrs   = (d_void **)malloc(kernel->num_datas   * sizeof(d_void *));

  result->loops = (acc_loop_desc_t *)malloc(kernel->num_loops * sizeof(struct acc_loop_desc_t_ *));
  unsigned i;
  for (i = 0; i < kernel->num_loops; i++)
    result->loops[i] = (acc_loop_desc_t)malloc(sizeof(struct acc_loop_desc_t_));

  return result;
}

void acc_enqueue_kernel(acc_region_t region, acc_kernel_t kernel) {
  acc_init_once();

  assert(region->num_dims == 1);

  assert(region->num_gang[0] > 0 && region->num_worker[0] > 0 && region->vector_length > 0);

  assert(kernel->loops[0]->stride == 1); /// \todo currently only support loops with positive unit stride

  unsigned best_matching_version = 0; // version #0 is always the generic (default) version 
  unsigned best_matching_score = 0;
  unsigned i, j;
  for (i = 1; i < kernel->desc->num_versions; i++) {
    if (    ( kernel->desc->versions[i]->num_gang == 0      || kernel->desc->versions[i]->num_gang      == region->num_gang[0]   )
         && ( kernel->desc->versions[i]->num_worker == 0    || kernel->desc->versions[i]->num_worker    == region->num_worker[0] )
         && ( kernel->desc->versions[i]->vector_length == 0 || kernel->desc->versions[i]->vector_length == region->vector_length )
    ) {
      unsigned score = 0;
      if (kernel->desc->versions[i]->num_gang      == region->num_gang[0]  ) score++;
      if (kernel->desc->versions[i]->num_worker    == region->num_worker[0]) score++;
      if (kernel->desc->versions[i]->vector_length == region->vector_length) score++;

      if (kernel->desc->versions[i]->loops != NULL) {
        for (j = 0; j < kernel->desc->num_loops; j++) {
          unsigned long loop_length = kernel->loops[j]->upper - kernel->loops[j]->lower;
          struct acc_loop_t_ * loop = &(kernel->desc->versions[i]->loops[j]);

          unsigned num_dynamic = 0;
          if (loop->tiles[e_tile_0].tiling == e_tiling_dynamic) num_dynamic++;
          if (loop->tiles[e_tile_1].tiling == e_tiling_dynamic) num_dynamic++;
          if (loop->tiles[e_tile_2].tiling == e_tiling_dynamic) num_dynamic++;
          if (loop->tiles[e_tile_3].tiling == e_tiling_dynamic) num_dynamic++;


          if (num_dynamic == 0) {
/*! \todo
            if (    loop->tiles[e_tile_0].strides != region->num_gang      * loop->tiles[e_tile_1].strides
                 || loop->tiles[e_tile_1].strides != region->num_worker    * loop->tiles[e_tile_2].strides
                 || loop->tiles[e_tile_2].strides != region->vector_length * loop->tiles[e_tile_3].strides
            ) break;
*/
            score += 4; // for static tiles
/*
            // Bonus points for empty tiles
            if (loop->tiles[e_tile_0].tiling == e_tiling_static_iteration && loop->tiles[e_tile_0].size == 1) score++;
            if (loop->tiles[e_tile_1].size == 1) score++;
            if (loop->tiles[e_tile_2].size == 1) score++;
            if (loop->tiles[e_tile_3].size == 1) score++;
*/
          }
          else if (num_dynamic == 1) {
            /// \todo determine value for stride
          }
          else if (num_dynamic > 1) {
            /// \todo determine values for strides (multiple answer)
            break;
          }
          else break; // Static tiling does not fit
        }
        if (j < kernel->desc->num_loops) break; 
      }

      if (score >= best_matching_score) {
        best_matching_version = i;
        best_matching_score = score;
      }
    }
  }

  char * version_suffix = kernel->desc->versions[best_matching_version]->suffix;

  char * kernel_name = (char *)malloc((strlen(kernel->desc->name) + strlen(version_suffix) + 1) * sizeof(char));
  strcpy(kernel_name, kernel->desc->name);
  strcat(kernel_name, version_suffix);

  assert(!"NIY"); /// \todo
}

