/*!
 * \addtogroup grp_libopenacc_api_internal Internal
 * @{
 * 
 * \file   OpenACC/internal/opencl-init.h
 * \author Tristan Vanderbruggen
 * \date   10/31/2013
 *
 */

#ifndef OPENACC_INTERNAL_OPENCL_INIT
#define OPENACC_INTERNAL_OPENCL_INIT 20131031

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  \brief Get the list of platforms and devices from OpenCL
 */
void acc_collect_ocl_devices();

/*!
 *  \brief Load sources of OpenCL C kernels from compiler generated file names
 */
void acc_load_ocl_sources();

/*!
 *  \brief Sorts collected OpenCL Devices
 */
void acc_init_ocl_devices();

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_INTERNAL */

/** @} */

