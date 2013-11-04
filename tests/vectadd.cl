
#include "OpenACC/device/opencl.h"

/*!
 *  Generic kernel generated for Vector Addition. Include the 3 levels from OpenACC (gang, worker, vector) and the 4 tiles.
 *  Suffix should be "_gang_worker_vector_tile_0_tile_1_tile_2_tile_3" but by conv this suffix is replaced by ""
 */
__kernel void vect_add_kernel(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_0;
  long it_loop_0_gang;
  long it_loop_0_tile_1;
  long it_loop_0_worker;
  long it_loop_0_tile_2;
  long it_loop_0_vect;
  long it_loop_0_tile_3;

  long it_loop_0;

  const unsigned long stride_loop_0_gang   = ctx->loops[0].strides[1] * ctx->num_gang;
  const unsigned long stride_loop_0_worker = ctx->loops[0].strides[2] * ctx->num_worker;
  const unsigned long stride_loop_0_vector = ctx->loops[0].strides[3] * ctx->vector_length;

  // Outer tile loop
  for (it_loop_0_tile_0 = ctx->loops[0].lower; it_loop_0_tile_0 < ctx->loops[0].upper; it_loop_0_tile_0 += ctx->loops[0].strides[0]) {

    // Gang "loop"
    it_loop_0_gang = it_loop_0_tile_0 + acc_gang_id(ctx) * stride_loop_0_gang;

    // Loop for tile between Gang and Worker
    for (it_loop_0_tile_1 = it_loop_0_gang; it_loop_0_tile_1 <= it_loop_0_gang + stride_loop_0_gang; it_loop_0_tile_1 += ctx->loops[0].strides[1]) {

      // Worker "loop"
      it_loop_0_worker = it_loop_0_tile_1 + acc_worker_id(ctx) * stride_loop_0_worker;

      // Loop for tile between Worker and Vector
      for (it_loop_0_tile_2 = it_loop_0_worker; it_loop_0_tile_2 <= it_loop_0_worker + stride_loop_0_worker; it_loop_0_tile_2 += ctx->loops[0].strides[2]) {

        // Inner tile loop
        for (it_loop_0_tile_3 = it_loop_0_tile_2; it_loop_0_tile_3 <= it_loop_0_tile_2 + stride_loop_0_vector; it_loop_0_tile_3 += ctx->loops[0].strides[3]) {

          // vector loop
          for (it_loop_0_vect = 0; it_loop_0_vect < ctx->vector_length; it_loop_0_vect++) {
            it_loop_0 = it_loop_0_tile_3 + it_loop_0_vect * ctx->loops[0].strides[3];
            if (it_loop_0 >= ctx->loops[0].lower && it_loop_0 <= ctx->loops[0].upper) {
              res[it_loop_0] = a[it_loop_0] + b[it_loop_0];
            }
          }
        }
      }
    }
  }
}

/*!
 *  Kernel generated for Vector Addition when only considering Gang and Worker (no Vector) and only tile #2 (ie. between Worker and Vector).
 */
__kernel void vect_add_kernel_gang_worker_tile_2(__global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {
  long it_loop_0_tile_0;
  long it_loop_0_gang;
  long it_loop_0_tile_1;
  long it_loop_0_worker;
  long it_loop_0_tile_2;
  long it_loop_0_vect;
  long it_loop_0_tile_3;

  long it_loop_0;

  const unsigned long stride_loop_0_gang   = ctx->num_gang;
  const unsigned long stride_loop_0_worker = ctx->loops[0].strides[2] * ctx->num_worker;

  // Outer tile loop
  it_loop_0_tile_0 = ctx->loops[0].lower;

  // Gang "loop"
  it_loop_0_gang = it_loop_0_tile_0 + acc_gang_id(ctx) * stride_loop_0_gang;

  // Loop for tile between Gang and Worker
  it_loop_0_tile_1 = it_loop_0_gang;

  // Worker "loop"
  it_loop_0_worker = it_loop_0_tile_1 + acc_worker_id(ctx) * stride_loop_0_worker;

  // Loop for tile between Worker and Vector
  for (it_loop_0_tile_2 = it_loop_0_worker; it_loop_0_tile_2 <= it_loop_0_worker + stride_loop_0_worker; it_loop_0_tile_2 += ctx->loops[0].strides[2]) {
    it_loop_0 = it_loop_0_tile_2;
    if (it_loop_0 >= ctx->loops[0].lower && it_loop_0 <= ctx->loops[0].upper) {
      res[it_loop_0] = a[it_loop_0] + b[it_loop_0];
    }
  }
}

