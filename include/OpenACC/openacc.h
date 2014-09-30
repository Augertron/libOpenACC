/*!
 * \addtogroup grp_libopenacc_api_public Public
 * @{
 * 
 * \file   OpenACC/openacc.h
 * \author Tristan Vanderbruggen
 * \date   11/01/2013
 *
 */

#ifndef OPENACC_PUBLIC
#define OPENACC_PUBLIC 20131031

#include "OpenACC/public/def.h"

#include "OpenACC/public/arch.h"

#include "OpenACC/public/device.h"

#include "OpenACC/public/async.h"

#include "OpenACC/public/runtime.h"

#include "OpenACC/public/memory.h"

#if !defined(LIBOPENACC)

size_t acc_worker_id(short);

#endif

#endif /* OPENACC_PUBLIC */

/** @} */

