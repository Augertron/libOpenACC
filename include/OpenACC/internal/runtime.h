/*!
 * \addtogroup grp_libopenacc_api_internal Internal
 * @{
 * 
 * \file   OpenACC/internal/runtime.h
 * \author Tristan Vanderbruggen
 * \date   10/31/2013
 *
 */

#ifndef OPENACC_INTERNAL_RUNTIME
#define OPENACC_INTERNAL_RUNTIME 20131031

#ifdef __cplusplus
extern "C" {
#endif

struct acc_context_t_;
struct acc_region_t_;
struct acc_kernel_t_;

/*! \func acc_create_context
 *
 *  Create the context associated to 'region' and 'kernel'
 *
 *  \param region pointer to a parallel region descriptor
 *  \param kernel pointer to a kernel descriptor
 *  \return a non-zero value if an error occured
 */
struct acc_context_t_ * acc_create_context(struct acc_region_t_ * region, struct acc_kernel_t_ * kernel);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_INTERNAL_RUNTIME */

/** @} */

