
#include "OpenACC/libopenacc-device.h"

__kernel void vect_add_kernel(unsigned long n, __global float * a, __global float * b, __global float * res, __constant struct acc_context_t_ * ctx) {

  // Verify that we are in Gang Partitionned & Worker Partitionned mode 
  if (acc_GP_mode(ctx) && acc_WP_mode(ctx)) {

    // Assume there is no tile before Gang (outter most tile)
    unsigned long loop_1_it_0 = acc_loop_no_tile(ctx, 1, 0);

    // Assume there is no tile between Gang and Vector
    unsigned long loop_1_it_1 = acc_loop_no_tile(ctx, 1, 1);

    // Get the bounds for the tile between Worker and Vector
    unsigned long loop_1_it_2;
    acc_loop_bounds_t loop_1_bounds_2 = acc_loop_bounds(ctx, 1, 2);

    // Assume there is no tile after Vector(inner most tile)
    unsigned long loop_1_it_3 = acc_loop_no_tile(ctx, 1, 3);

    for (loop_1_it_2 = loop_1_bounds_2.lower; loop_1_it_2 < loop_1_bounds_2.upper; loop_1_it_2 += loop_1_bounds_2.stride) {

      // Compute the current iteration of loop_1 
      unsigned long loop_1_it = acc_loop_iteration(ctx, 1, loop_1_it_0, loop_1_it_1, loop_1_it_2, loop_1_it_3);

      res[loop_1_it] = a[loop_1_it] + b[loop_1_it];
    }
  }
}

