/*!
 * \addtogroup grp_libopenacc_api_public Public
 * @{
 * 
 * \file   OpenACC/internal/runtime.h
 * \author Tristan Vanderbruggen
 * \date   10/31/2013
 *
 */

#ifndef OPENACC_INTERNAL_RUNTIME
#define OPENACC_INTERNAL_RUNTIME 20131105

#include "OpenACC/public/arch.h"
#include "OpenACC/private/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

acc_device_t acc_get_device_type_by_device_idx(size_t device_idx);

int acc_device_idx_check_type(size_t device_idx, acc_device_t dev);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_INTERNAL_RUNTIME */

