/*!
 * \addtogroup grp_libopenacc_api_public Public
 * @{
 * 
 * \file   OpenACC/public/memory.h
 * \author Tristan Vanderbruggen
 * \date   10/31/2013
 *
 */

#ifndef OPENACC_PUBLIC_MEMORY
#define OPENACC_PUBLIC_MEMORY 20131031

#ifdef __cplusplus
extern "C" {
#endif

d_void * acc_malloc(size_t);

void acc_free(d_void *);



d_void * acc_copyin(h_void *, size_t);

d_void * acc_present_or_copyin(h_void *, size_t);

d_void * acc_pcopyin(h_void *, size_t);

d_void * acc_create(h_void *, size_t);

d_void * acc_present_or_create(h_void *, size_t);

d_void * acc_pcreate(h_void *, size_t);

void acc_copyout(h_void*, size_t);

d_void * acc_present_or_copyout(h_void *, size_t);

d_void * acc_pcopyout(h_void *, size_t);

void acc_delete(h_void *, size_t);



void acc_update_device(h_void *, size_t);

void acc_update_self(h_void *, size_t);



void acc_map_data(h_void *, d_void *, size_t);

void acc_unmap_data(h_void *);



d_void * acc_deviceptr(h_void *);

h_void * acc_hostptr(d_void *);



int acc_is_present(h_void *, size_t);



void acc_memcpy_to_device(d_void * dest, h_void * src, size_t bytes);

void acc_memcpy_from_device(h_void * dest, d_void * src, size_t bytes);

#ifdef __cplusplus
}
#endif

#endif /* OPENACC_PUBLIC_MEMORY */

/*! @} */

