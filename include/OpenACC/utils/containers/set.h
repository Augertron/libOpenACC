/*!
 * \file OpenACC/utils/set.h
 *
 * \author Tristan Vanderbruggen
 * \date 10/31/2013
 *
 */

#ifndef LIBOPENACC_UTILS_SET
#define LIBOPENACC_UTILS_SET 20131111

#include "OpenACC/utils/containers/associative.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct associative_t_ * set_t;

set_t set_alloc(size_t size, size_t key_size, key_cmp_f less);

/*!
 *  
 */
void set_insert(set_t set, const key_type * key);

/*!
 *  
 */
void set_remove(set_t set, const key_type * key);

/*!
 *  
 */
const key_type * set_lookup(set_t set, const key_type * key);

void set_free(set_t set);

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_UTILS_SET */

