#ifndef _LIBOPENACC_MEM_MANAGER_H_
#define _LIBOPENACC_MEM_MANAGER_H_

/*
  Maps host-device memory given host pointer and device pointer
  
  \params
  h_void* host pointer to map
  d_void* device pointer to map
  
  \returns
  none
  
  \precond
  host pointer must not exist in memory manager
  device pointer must not exist in memory manager
  
  \postcond
  mapping of host-device memory is inserted into memory manager
  
 */
void
mem_map_host_device (h_void*, d_void*);

/*
  Unmaps host-device memory given host pointer
  
  \params
  h_void* host pointer
  
  \returns
  d_void* device pointer unmapped or NULL if mapping did not exist
  
  \postcond
  host-device memory map for passed pointer is removed. If the address
  is not in the memory manager lookup table, nothing changes
 */
d_void*
mem_unmap_host (h_void*);

/*
  Unmaps host-device memory given device pointer
  
  \params
  d_void* device pointer
  
  \returns
  h_void* host pointer unmapped or NULL if mapping did not exist
  
  \postcond
  host-device memory map for passed pointer is removed. If the address
  is not in the memory manager lookup table, nothing changes
 */
h_void*
mem_unmap_device (d_void*);
/* perhaps unneeded */


/*
  Accessor of host pointer given device pointer
  
  \params
  h_void* the host pointer
  
  \returns
  d_void* the device pointer currently mapped to the host pointer. NULL if there is no existing mapping

 */
d_void*
mem_device_ptr (h_void*);

/*
  Accessor of device pointer given host pointer
  
  \params
  d_void* the device pointer
  
  \returns
  h_void* the host pointer currently mapped to the device pointer. NULL if there is no existing mapping

 */
h_void*
mem_host_ptr (d_void*);

#endif /* _LIBOPENACC_MEM_MANAGER_H_ */
