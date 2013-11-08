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

size_t acc_get_device_idx(acc_device_t dev, int num);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_INTERNAL_RUNTIME */

