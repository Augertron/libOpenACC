/*!
 * \file OpenACC/utils/containers/def.h
 *
 * \author Tristan Vanderbruggen
 * \date 10/31/2013
 *
 */

#ifndef LIBOPENACC_UTILS_CONTAINER_DEF
#define LIBOPENACC_UTILS_CONTAINER_DEF 20131111

#ifdef __cplusplus
extern "C" {
#endif

typedef void key_type;
typedef void data_type;

typedef int (* key_cmp_f) (const key_type *, const key_type *);

#ifdef __cplusplus
}
#endif

#endif /* LIBOPENACC_UTILS_CONTAINER_DEF */

