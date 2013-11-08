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

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Debug: dump OpenACC runtime information
 */
void acc_dbg_dump_runtime();

char * acc_ocl_status_to_char(cl_int status);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PRIVATE_DEBUG */

/** @} */

