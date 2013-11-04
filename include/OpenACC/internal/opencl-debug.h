/*!
 * \addtogroup grp_libopenacc_api_private Private
 * @{
 * 
 * \file   OpenACC/internal/opencl-debug.h
 * \author Tristan Vanderbruggen
 * \date   09/2013
 *
 */

#ifndef OPENACC_PRIVATE_DEBUG
#define OPENACC_PRIVATE_DEBUG 20131031

#include "OpenACC/public/arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Debug: Print an OpenCL build log
 */
void acc_dbg_ocl_build_log(unsigned device_idx, cl_program program, acc_device_t dev, int num);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PRIVATE_DEBUG */

/** @} */

