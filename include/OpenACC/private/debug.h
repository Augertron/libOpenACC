/*!
 * \addtogroup grp_libopenacc_api_private Private
 * @{
 * 
 * \file   OpenACC/private/debug.h
 * \author Tristan Vanderbruggen
 * \date   09/2013
 *
 */

#ifndef OPENACC_PRIVATE_DEBUG
#define OPENACC_PRIVATE_DEBUG 20131031

#include "OpenACC/public/arch.h"
#include "OpenACC/private/kernel.h"
#include "OpenACC/private/region.h"
#include "OpenACC/device/host-context.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Dump the content of context build to launch a given kernel on a given device
void acc_debug_dump_context(struct acc_region_t_ * region, struct acc_kernel_t_ * kernel, struct acc_context_t_ * context, size_t device_idx);

void acc_dbg_dump_data_env();

/*!
 * \brief Debug: dump OpenACC runtime information
 */
void acc_dbg_dump_runtime();

const char * acc_ocl_status_to_char(cl_int status);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PRIVATE_DEBUG */

/** @} */

