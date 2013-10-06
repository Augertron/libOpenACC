/*!
 * \file lib-openacc.h
 *
 * \author Tristan Vanderbruggen
 * \date 09/2013
 * 
 * Standard OpenACC C API
 *
 */

#ifndef __OPENACC_H__
#define __OPENACC_H__

/// Represent an Accelerator Device for OpenACC
typedef struct acc_device_t_ * acc_device_t;

/// For Host data pointer type (h_void *)
typedef void h_void;

/// For Device data pointer type (d_void *)
typedef void d_void;

/*!
 *  \param[dev]  an OpenACC device
 *  \return number of present devices of the same type
 */
int  acc_get_num_devices(acc_device_t dev);

/*!
 *  Set the device to be used in the following parallel regions
 *
 *  \param[dev]  an OpenACC device
 */
void acc_set_device_type(acc_device_t dev);

/*!
 *  \return the device to be used in the following parallel regions
 */
acc_device_t acc_get_device_type();

/*!
 *  Associate an ID (integer) to a device
 *
 *  \param[id]   an positive integer to become the ID of 'dev'
 *  \param[dev]  an OpenACC device
 */
void acc_set_device_num(int id, acc_device_t dev)

/*!
 *  \param[dev] an OpenACC device
 *  \return the ID associated to 'dev' (or -1)
 */
int acc_get_device_num (acc_device_t);

/*!
 */
int acc_async_test(int);

/*!
 */
int acc_async_test_all();

/*!
 */
void acc_async_wait(int);

/*!
 */
void acc_async_wait_async(int, int);

/*!
 */
void acc_async_wait_all();

/*!
 */
void acc_async_wait_all_async(int);

/*!
 */
void acc_init(acc_device_t);

/*!
 */
void acc_shutdown(acc_device_t);

/*!
 */
int acc_on_device(acc_device_t);

/*!
 */
d_void * acc_malloc(size_t);

/*!
 */
void acc_free(d_void *)

/*!
 */
void * acc_copyin(h_void *, size_t);

/*!
 */
void * acc_create(h_void *, size_t);

/*!
 */
void * acc_present_or_create(h_void *, size_t);

/*!
 */
void * acc_pcreate(h_void *, size_t);

/*!
 */
void acc_copyout(h_void*, size_t);

/*!
 */
void acc_delete(h_void *, size_t);

/*!
 */
void acc_update_device(h_void *, size_t);

/*!
 */
void acc_update_self(h_void *, size_t);

/*!
 */
void acc_map_data(h_void *, d_void *, size_t);

/*!
 */
void acc_unmap_data(h_void *);

/*!
 */
d_void * acc_deviceptr(h_void *);

/*!
 */
h_void * acc_hostptr(d_void *);

/*!
 */
int acc_is_present(h_void *, size_t);

/*!
 */
void acc_memcpy_to_device(d_void * dest, h_void * src, size_t bytes);

/*!
 */
void acc_memcpy_from_device(h_void * dest, d_void * src, size_t bytes);

#endif /* __OPENACC_H__ */

