
#include "OpenACC/internal/region.h"
#include "OpenACC/internal/kernel.h"

#include "OpenACC/device/host-context.h"

#include "OpenACC/private/kernel.h"
#include "OpenACC/internal/kernel.h"

#include "OpenACC/internal/loop.h"
#include "OpenACC/internal/compiler.h"

#include "OpenACC/internal/runtime.h"
#include "OpenACC/private/debug.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <assert.h>

#ifndef DBG_HOST_CTX
# define DBG_HOST_CTX 0
#endif

typedef struct acc_region_t_ * acc_region_t;
typedef struct acc_kernel_t_ * acc_kernel_t;
typedef struct acc_context_t_ * acc_context_t;

static int version_match(struct acc_region_per_device_t_ * region_per_device, acc_kernel_version_t version, size_t device_idx) {
  return acc_device_idx_check_type(device_idx, version->device_affinity)
      && ( version->num_gang[0]      == 0 || version->num_gang[0]      == region_per_device->num_gang[0]      )
      && ( version->num_gang[1]      == 0 || version->num_gang[1]      == region_per_device->num_gang[1]      )
      && ( version->num_gang[2]      == 0 || version->num_gang[2]      == region_per_device->num_gang[2]      )
      && ( version->num_worker[0]    == 0 || version->num_worker[0]    == region_per_device->num_worker[0]    )
      && ( version->num_worker[1]    == 0 || version->num_worker[1]    == region_per_device->num_worker[1]    )
      && ( version->num_worker[2]    == 0 || version->num_worker[2]    == region_per_device->num_worker[2]    )
      && ( version->vector_length    == 0 || version->vector_length == region_per_device->vector_length       );
}

void acc_eval_kernel_version(
  struct acc_region_per_device_t_ * region_per_device,
  acc_kernel_t kernel,
  size_t device_idx,
  size_t version_idx,
  size_t * best_matching_version,
  float  * best_matching_score,
  struct acc_tile_t_ ** best_matching_tiles
) {
  struct acc_kernel_version_t_ * version = &(kernel->desc->versions[version_idx]);

  if (!version_match(region_per_device, version, device_idx)) return;

  struct acc_tile_t_ * tiles = malloc(version->num_tiles * sizeof(struct acc_tile_t_));

  size_t loop_idx, tile_pos;
  for (loop_idx = 0; loop_idx < kernel->desc->num_loops; loop_idx++) {
    struct acc_loop_desc_t_ * loop_desc = &(version->loops[loop_idx]);
    if (loop_desc->num_tiles == 0) continue;

    struct acc_loop_t_ * loop = &(kernel->loops[loop_idx]);
    size_t loop_length = loop->upper - loop->lower + 1;

    // In this loop tiles[tile_idx].length stores the number of iterations of each tile
    size_t dynamic_tile_pos = loop_desc->num_tiles;
    size_t static_tiles_length = 1;
    for (tile_pos = 0; tile_pos < loop_desc->num_tiles; tile_pos++) {
      struct acc_tile_desc_t_ * tile_desc = &(loop_desc->tiles[tile_pos]);

      switch (tile_desc->kind) {
        case e_gang_tile:
          tiles[tile_desc->id].length = region_per_device->num_gang[tile_desc->param.lvl];
          static_tiles_length *= tiles[tile_desc->id].length;
          break;
        case e_worker_tile:
          tiles[tile_desc->id].length = region_per_device->num_worker[tile_desc->param.lvl];
          static_tiles_length *= tiles[tile_desc->id].length;
          break;
        case e_vector_tile:
          tiles[tile_desc->id].length = region_per_device->vector_length;
          static_tiles_length *= tiles[tile_desc->id].length;
          break;
        case e_dynamic_tile:
          assert(dynamic_tile_pos == loop_desc->num_tiles); // Only one dynamic tile
          dynamic_tile_pos = tile_pos;
          break;
        case e_static_tile:
        case e_unrolled_tile:
          tiles[tile_desc->id].length = tile_desc->param.nbr_it;
          static_tiles_length *= tiles[tile_desc->id].length;
          break;
        default:
          assert(0);
      }
    }

    if (dynamic_tile_pos != loop_desc->num_tiles) {
      size_t dynamic_tile_idx = loop_desc->tiles[dynamic_tile_pos].id;
      tiles[dynamic_tile_idx].length = loop_length / static_tiles_length;
      if (loop_length % static_tiles_length != 0) {
        printf("[info]    Cannot set num iteration of dynamic tile for version %zd of kernel %zd because the length of the loop is not divisible by the product of the num iteration of the other tiles.\n", version_idx, kernel->desc->id);
        break;
      }
    }
    else assert(static_tiles_length == loop_length);

    size_t stride = loop->stride;
    size_t tile_idx;
    for (tile_pos = loop_desc->num_tiles; tile_pos > 0; tile_pos--) {
      tile_idx = loop_desc->tiles[tile_pos - 1].id;
      tiles[tile_idx].stride = stride;
      tiles[tile_idx].length *= stride;
      stride = tiles[tile_idx].length;
    }
    assert(tiles[loop_desc->tiles[0].id].length == loop_length);
  }

  if (loop_idx == kernel->desc->num_loops) {
    float score = 1e-6; // best_matching_score is initialized to 0
    for (loop_idx = 0; loop_idx < kernel->desc->num_loops; loop_idx++) {
      /// \todo Compute the score for this version
    }
    if (score > *best_matching_score) {
      *best_matching_score = score;

      *best_matching_version = version_idx;

      if (*best_matching_tiles != NULL)
        free(*best_matching_tiles);
      *best_matching_tiles = tiles;

      return; 
    }
  }

  free(tiles);
}

void acc_select_kernel_version(
  struct acc_region_per_device_t_ * region_per_device,
  size_t region_dev_idx,
  acc_kernel_t kernel,
  size_t device_idx,
  size_t * best_matching_version,
  struct acc_tile_t_ ** best_matching_tiles
) {
  float best_matching_score = 0;

  if (kernel->desc->version_by_devices != NULL) {
    size_t version_id = kernel->desc->version_by_devices[region_dev_idx];
    assert(version_id != -1);

    size_t version_idx = -1;
    size_t i;
    for (i = 0; i < kernel->desc->num_versions; i++)
      if (kernel->desc->versions[i].id == version_id) {
        version_idx = i;
        break;
      }
    assert(version_idx != -1);

    acc_eval_kernel_version(
      region_per_device, kernel,
      device_idx, version_idx,
      best_matching_version,
      &best_matching_score,
      best_matching_tiles
    );
    assert(*best_matching_tiles != NULL);
  }
  else {
    size_t version_idx;
    for (version_idx = 0; version_idx < kernel->desc->num_versions; version_idx++)
      acc_eval_kernel_version(
        region_per_device, kernel, 
        device_idx, version_idx,
        best_matching_version,
        &best_matching_score,
        best_matching_tiles
      );
  }
}

struct cl_kernel_ * acc_build_ocl_kernel(acc_region_t region, acc_kernel_t kernel, acc_context_t * context, size_t device_idx) {
  size_t best_matching_version = kernel->desc->num_versions;
  struct acc_tile_t_ * best_matching_tiles = NULL;

  size_t region_dev_idx;
  for (region_dev_idx = 0; region_dev_idx < region->num_devices; region_dev_idx++)
    if (region->devices[region_dev_idx].device_idx == device_idx)
      break;
  assert(region_dev_idx < region->num_devices);

  struct acc_region_per_device_t_ * region_per_device = &(region->devices[region_dev_idx]);

  acc_select_kernel_version(region_per_device, region_dev_idx, kernel, device_idx, &best_matching_version, &best_matching_tiles);
  assert(best_matching_version != kernel->desc->num_versions);
  assert(best_matching_tiles != NULL);

  struct acc_kernel_version_t_ * version = &(kernel->desc->versions[best_matching_version]);

  *context = malloc(sizeof(struct acc_context_t_) + 2 * (kernel->desc->num_loops + version->num_tiles) * sizeof(long));
  (*context)->num_loops = kernel->desc->num_loops;
  (*context)->num_tiles = version->num_tiles;
  size_t loop_id;
  for (loop_id = 0; loop_id < kernel->desc->num_loops; loop_id++) {
    (*context)->data[2 * loop_id]     = kernel->loops[loop_id].lower;
    (*context)->data[2 * loop_id + 1] = kernel->loops[loop_id].upper;
  }
  memcpy((*context)->data + 2 * kernel->desc->num_loops, best_matching_tiles, version->num_tiles * sizeof(struct acc_tile_t_));

#if DBG_HOST_CTX || 1
  acc_debug_dump_context(region, kernel, *context, device_idx);
#endif

  // Build the kernel name 
  char * version_suffix = version->suffix;
  char * kernel_name = (char *)malloc((strlen(kernel->desc->name) + strlen(version_suffix) + 1) * sizeof(char));
  strcpy(kernel_name, kernel->desc->name);
  strcat(kernel_name, version_suffix);

  size_t region_idx;
  for (region_idx = 0; region_idx < compiler_data.num_regions; region_idx++)
    if (compiler_data.regions[region_idx]->id == region->desc->id)
      break;
  assert(region_idx < compiler_data.num_regions);

  cl_program program = acc_runtime.opencl_data->devices_data[device_idx]->programs[region_idx];
  assert(program != NULL);

  cl_int status;
  struct cl_kernel_ * ocl_kernel = clCreateKernel(program, kernel_name, &status);
  if (status != CL_SUCCESS) {
    const char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal]   clCreateKernel return %s for %s.\n", status_str, kernel_name);
    exit(-1); /// \todo error code
  }

  return ocl_kernel;
}

