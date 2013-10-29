/*!
 * \addtogroup grp_libopenacc_impl_host Host Side
 * @{
 * 
 * \file lib-openacc.c
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "OpenACC/libopenacc-internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#ifndef PRINT_BUILD_LOG
# define PRINT_BUILD_LOG 1
#endif

acc_runtime_t acc_runtime = {0, NULL, acc_device_any, 0}; // by default, we use the first of any device

const char * acc_device_env_name [acc_device_last] = {
  "ANY",
  "NVIDIA",
  "AMD",
  "RADEON",
  "INTEL",
  "CORE",
  "I7-3610QM",
  "I7-950",
  "XEONPHI"
};

const char * acc_device_name [acc_device_last] = {
  "All",
  "Nvidia",
  "AMD",
  "AMD radeon",
  "Intel(R)",
  "Intel(R) Core(TM)",
  "Intel(R) Core(TM) i7-3610QM CPU",
  "Intel(R) Core(TM) i7 CPU 950",
  "Intel(R) XeonPhi(TM)"
};

acc_device_defaults_t acc_device_defaults [acc_device_last] = {
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 }
};

device_desc_t devices_desc [2] = {
  { "Intel(R) Core(TM) i7-3610QM CPU"      , acc_device_i7_3610QM },
  { "Intel(R) Core(TM) i7 CPU         950" , acc_device_i7_950    }
};

device_type_desc_t devices_type_desc [12] = {
  { CL_DEVICE_TYPE_CPU         , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_GPU         , acc_device_last    , 0, NULL               }, ///< \todo NVidia GPU
  { CL_DEVICE_TYPE_ACCELERATOR , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_CPU         , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_GPU         , acc_device_radeon  , 0, NULL               }, ///< Radeon
  { CL_DEVICE_TYPE_ACCELERATOR , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_CPU         , acc_device_core    , 2, &(devices_desc[0]) }, ///< Core
  { CL_DEVICE_TYPE_GPU         , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_ACCELERATOR , acc_device_xeonphi , 0, NULL               }, ///< XeonPhi
  { CL_DEVICE_TYPE_CPU         , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_GPU         , acc_device_last    , 0, NULL               },
  { CL_DEVICE_TYPE_ACCELERATOR , acc_device_last    , 0, NULL               }
};

platform_desc_t platforms_desc[NUM_OPENCL_PLATFORMS] = {
  { "Nvidia",          acc_device_nvidia, 3, &(devices_type_desc[0]) }, /// \todo find actual platform name for Nvidia
  { "AMD",             acc_device_amd,    3, &(devices_type_desc[3]) }, /// \todo find actual platform name for AMD
  { "Intel(R) OpenCL", acc_device_intel,  3, &(devices_type_desc[6]) },
  { "Altera",          acc_device_altera, 3, &(devices_type_desc[9]) }  /// \todo find actual platform name for Altera
};

// OpenACC public API

int acc_get_num_devices(acc_device_t dev) {
  acc_init_once();

  return acc_runtime.devices[dev].num;
}

void acc_set_device_type(acc_device_t dev) {
  acc_init_once();

  acc_runtime.curr_device_type = dev;
}

acc_device_t acc_get_device_type() {
  acc_init_once();

  return acc_runtime.curr_device_type;
}

void acc_set_device_num (int num, acc_device_t dev) {
  acc_init_once();

  acc_runtime.curr_device_type = dev;
  acc_runtime.curr_device_num  = num;
}

int acc_get_device_num(acc_device_t dev) {
  acc_init_once();

  assert(acc_runtime.curr_device_type == dev);

  return acc_runtime.curr_device_num;
}

int acc_async_test(int id) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return -1;
}

int acc_async_test_all() {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return -1;
}

void acc_async_wait(int id) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_async_wait_async(int id_wait, int id_async) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_async_wait_all() {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_async_wait_all_async(int id) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_init(acc_device_t dev) {
  acc_init_once();

  size_t num_devices = acc_get_num_devices(dev);

  assert(num_devices > 0);

  unsigned i;
  for (i = 0; i < num_devices; i++)
    acc_init_(dev, i);
}

void print_build_log(unsigned device_idx, cl_program program, acc_device_t dev, int num) {
  char * build_log;
  size_t build_log_size;
  clGetProgramBuildInfo(program, acc_runtime.opencl_data->devices[0][device_idx], CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_size);
  if (build_log_size == 0)
    printf("[warning] OpenCL return an empty log...\n");
  else {
    build_log = (char*)malloc(build_log_size);
    if (build_log == NULL) {
      perror("[fatal]   malloc : build_log");
      exit(-1);
    }
    clGetProgramBuildInfo(program, acc_runtime.opencl_data->devices[0][device_idx], CL_PROGRAM_BUILD_LOG, build_log_size, build_log, NULL);
    build_log[build_log_size-1] = '\0';
    printf("Device %s %u Build Log:\n%s\n", acc_device_name[dev], num, build_log);   
    free(build_log);
  }
}

void acc_init_(acc_device_t dev, int num) {
  acc_init_once();

  unsigned first_device = acc_runtime.devices[dev].first;
  size_t num_devices = acc_get_num_devices(dev);

  assert(num >= 0 && num < num_devices);

  unsigned device_idx = first_device + num;

  if (acc_runtime.opencl_data->devices_data[device_idx] == NULL) {
    cl_int status;

    cl_device_id * device = &(acc_runtime.opencl_data->devices[0][device_idx]);

    acc_device_data_t device_data = (acc_device_data_t)malloc(sizeof(struct acc_device_data_t_));
    if (device_data == NULL) {
      perror("[fatal]   malloc : device_data");
      exit(-1);
    }
    acc_runtime.opencl_data->devices_data[device_idx] = device_data;

    device_data->context = clCreateContext(NULL, 1, device, NULL, NULL, &status);
    if (status != CL_SUCCESS || device_data->context == NULL) {
       printf("[error]   clCreateContext : %s, %u return %u : failed\n", acc_device_name[dev], num, status);

       free(device_data);
       device_data = NULL;

       return;
    }

    device_data->programs = (cl_program *)malloc(compiler_data.num_regions * sizeof(cl_program));
    if (device_data->programs == NULL) {
      perror("[fatal]   malloc : device_data->programs");
      exit(-1);
    }
    unsigned i;
    for (i = 0; i < compiler_data.num_regions; i++)
      device_data->programs[i] = NULL;
  }
}

void acc_shutdown(acc_device_t dev) {
  acc_init_once();

  size_t num_devices = acc_get_num_devices(dev);

  assert(num_devices > 0);

  unsigned i;
  for (i = 0; i < num_devices; i++)
    acc_shutdown_(dev, i);
}

void acc_shutdown_(acc_device_t dev, int num) {
  acc_init_once();

  unsigned first_device = acc_runtime.devices[dev].first;
  size_t num_devices = acc_get_num_devices(dev);

  assert(num >= 0 && num < num_devices);

  unsigned device_idx = first_device + num;

  if (acc_runtime.opencl_data->devices_data[device_idx] != NULL) {
    cl_int status;

    status = clReleaseContext(acc_runtime.opencl_data->devices_data[device_idx]->context);
    if (status != CL_SUCCESS)
      printf("[warning] clReleaseContext : %s, %u return %u\n", acc_device_name[dev], num, status);
    if (acc_runtime.opencl_data->devices_data[device_idx]->programs != NULL) {
      unsigned i;
      for (i = 0; i < compiler_data.num_regions; i++)
        if (acc_runtime.opencl_data->devices_data[device_idx]->programs[i] != NULL) {
          status = clReleaseProgram(acc_runtime.opencl_data->devices_data[device_idx]->programs[i]);
          if (status != CL_SUCCESS)
            printf("[warning] clReleaseProgram : %s, %u for region %u return %u\n", acc_device_name[dev], num, i, status);
        }
      free(acc_runtime.opencl_data->devices_data[device_idx]->programs);
      acc_runtime.opencl_data->devices_data[device_idx]->programs = NULL;
    }
    free(acc_runtime.opencl_data->devices_data[device_idx]);

    acc_runtime.opencl_data->devices_data[device_idx] = NULL;
  }
}

int acc_on_device(acc_device_t dev) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return -1;
}

d_void * acc_malloc (size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

void acc_free(d_void * dev_ptr) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void * acc_copyin(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

void * acc_create(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

void * acc_present_or_create(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

void * acc_pcreate(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

void acc_copyout(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_delete(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_update_device(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_update_self(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_map_data(h_void * host_ptr, d_void * dev_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_unmap_data(h_void * host_ptr) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

d_void * acc_deviceptr(h_void * host_ptr) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

h_void * acc_hostptr(d_void * dev_ptr) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return NULL;
}

int acc_is_present(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert(!"NIY"); /// \todo
  return -1;
}

void acc_memcpy_to_device(d_void * dest, h_void * src, size_t bytes) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

void acc_memcpy_from_device(h_void * dest, d_void * src, size_t bytes) {
  acc_init_once();

  assert(!"NIY"); /// \todo
}

// OpenACC private API

int check_flag(uint32_t flag) {
  return (acc_runtime.check_list & flag) == flag;
}

void set_flag(uint32_t flag) {
  acc_runtime.check_list |= flag;
}

void acc_init_once() {
  if (!check_flag(f_alloc)) {
    acc_runtime.opencl_data = (acc_opencl_data_t)malloc(sizeof(struct acc_opencl_data_t_));
    set_flag(f_alloc);
  }
/*
  if (!check_flag(f_ocl_kernels)) {
    acc_init_kernel_first();
    set_flag(f_ocl_kernels);
  }
*/
  if (!check_flag(f_ocl_devices))
    acc_collect_ocl_devices();
  assert(check_flag(f_ocl_devices));

  if (!check_flag(f_acc_devices))
    acc_init_devices();
  assert(check_flag(f_acc_devices));

  if (!check_flag(f_ocl_sources))
    acc_load_ocl_sources();
  assert(check_flag(f_ocl_sources));

  if (!check_flag(f_acc_defaults))
    acc_init_defaults();
  assert(check_flag(f_acc_defaults));
}

void acc_collect_ocl_devices() {
  cl_int status = clGetPlatformIDs(0, NULL, &(acc_runtime.opencl_data->num_platforms));
  if (status != CL_SUCCESS) {
    printf("[error]   clGetPlatformIDs return %u when looking for the number of platforms.\n", status);
    return;
  }

  if (acc_runtime.opencl_data->num_platforms == 0) {
    set_flag(f_ocl_devices);
    return;
  }

  acc_runtime.opencl_data->platforms = (cl_platform_id *)malloc(acc_runtime.opencl_data->num_platforms * sizeof(cl_platform_id));
  if (acc_runtime.opencl_data->platforms == NULL) {
    perror("[fatal]   malloc: acc_runtime.opencl_data->platforms");
    exit(-1);
  }

  acc_runtime.opencl_data->num_devices = (cl_uint *)malloc((acc_runtime.opencl_data->num_platforms + 1) * sizeof(cl_uint)); // last elem contain sum
  if (acc_runtime.opencl_data->platforms == NULL) {
    perror("[fatal]   malloc: acc_runtime.opencl_data->num_devices");
    exit(-1);
  }

  acc_runtime.opencl_data->devices = (cl_device_id **)malloc(acc_runtime.opencl_data->num_platforms * sizeof(cl_device_id *));
  if (acc_runtime.opencl_data->devices == NULL) {
    perror("[fatal]   malloc: acc_runtime.opencl_data->devices");
    exit(-1);
  }

  status = clGetPlatformIDs(acc_runtime.opencl_data->num_platforms, acc_runtime.opencl_data->platforms, NULL);
  if (status != CL_SUCCESS) {
    printf("[error]   clGetPlatformIDs return %u when retrieving %u platforms.\n", status, acc_runtime.opencl_data->num_platforms);
    return;
  }

  unsigned i;
  unsigned num_devices = 0;
  for (i = 0; i < acc_runtime.opencl_data->num_platforms; i++) {
    const cl_platform_id platform = acc_runtime.opencl_data->platforms[i];

    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &(acc_runtime.opencl_data->num_devices[i]));
    if(status != CL_SUCCESS) {
     printf("[warning] clGetDeviceIDs return %u when looking for number of devices for platform %u\n", status, i);

     acc_runtime.opencl_data->num_devices[i] = 0;

     continue;
    }

    num_devices += acc_runtime.opencl_data->num_devices[i];
  }
  acc_runtime.opencl_data->num_devices[acc_runtime.opencl_data->num_platforms] = num_devices;

  cl_device_id * devices = (cl_device_id *)malloc(num_devices * sizeof(cl_device_id));
  if(devices == NULL) {
    perror("[fatal]   malloc: acc_runtime.opencl_data->devices[0]");
    exit(-1);
  }

  for (i = 0; i < acc_runtime.opencl_data->num_platforms; i++) {
    const cl_platform_id platform = acc_runtime.opencl_data->platforms[i];

    if (acc_runtime.opencl_data->num_devices[i] != 0) {
      status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, acc_runtime.opencl_data->num_devices[i], devices, NULL);
      if (status != CL_SUCCESS) {
        printf("[warning] clGetDeviceIDs return %u when retrieving %u devices for platform %u\n", status, acc_runtime.opencl_data->num_devices[i], i);

        acc_runtime.opencl_data->num_devices[i] = 0;
        acc_runtime.opencl_data->devices[i] = NULL;

        continue;
      }

      /// \todo check that devices are sorted by type.

      acc_runtime.opencl_data->devices[i] = devices;
      devices += acc_runtime.opencl_data->num_devices[i] * sizeof(cl_device_id);
    }
    else {
      acc_runtime.opencl_data->devices[i] = NULL;
    }
  }

  acc_runtime.opencl_data->devices_data = (acc_device_data_t *)malloc(num_devices * sizeof(acc_device_data_t));
  if(acc_runtime.opencl_data->devices_data == NULL) {
    perror("[fatal]   malloc: acc_runtime.opencl_data->devices_data");
    exit(-1);
  }

  for (i = 0; i < num_devices; i++)
    acc_runtime.opencl_data->devices_data[i] = NULL;

  set_flag(f_ocl_devices);
}

void acc_load_ocl_sources() {
  unsigned i;

  char ocl_rt_file[50];
  strcpy(ocl_rt_file, compiler_data.acc_runtime_dir);
  strcat(ocl_rt_file, "/");
  strcat(ocl_rt_file, compiler_data.acc_runtime_ocl);
  acc_runtime.opencl_data->runtime_sources = readSource(ocl_rt_file);

  if (compiler_data.num_regions <= 0) {
    printf("[error]   There is not any OpenCL region (parallel/kernel constructs) listed! Check that acc_init_kernel_first is called (and correct).\n");
    return;
  }

  acc_runtime.opencl_data->region_sources = (char **)malloc(compiler_data.num_regions * sizeof(char *));
  if (acc_runtime.opencl_data->region_sources == NULL) {
    perror("[fatal]   malloc: acc_runtime.opencl_data->region_sources");
    exit(-1);
  }

  for (i = 0; i < compiler_data.num_regions; i++)
    acc_runtime.opencl_data->region_sources[i] = readSource(compiler_data.regions[i]->file);

  set_flag(f_ocl_sources);
}

unsigned acc_get_platform_desc(cl_platform_id platform) {
  char name[50];
  unsigned r;

  clGetPlatformInfo(platform, CL_PLATFORM_NAME, sizeof(name), name, NULL);
  for (r = 0; r < NUM_OPENCL_PLATFORMS; r++)
    if (strcmp(name, platforms_desc[r].ocl_name) == 0)
      break;
  assert(r < NUM_OPENCL_PLATFORMS); /// \todo unrecognized CL_PLATFORM_NAME

  return r;
}

unsigned acc_get_device_type_desc(cl_device_id device, unsigned r) {
  cl_device_type ocl_dev_type;
  unsigned s;

  clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(cl_device_type), &ocl_dev_type, NULL);
  for (s = 0; s < platforms_desc[r].num_devices_type; s++)
    if (platforms_desc[r].devices_type_desc[s].ocl_device_type == ocl_dev_type)
      break;
  assert(s < platforms_desc[r].num_devices_type); /// \todo unrecognized CL_DEVICE_TYPE

  return s;
}

unsigned acc_get_device_desc(cl_device_id device, unsigned r, unsigned s) {
  char name[50];
  unsigned t;

  clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(name), name, NULL);
  for (t = 0; t < platforms_desc[r].devices_type_desc[s].num_devices; t++)
    if (strstr(name, platforms_desc[r].devices_type_desc[s].devices_desc[t].ocl_name) != NULL)
      break;
  if (t == platforms_desc[r].devices_type_desc[s].num_devices) {
    printf("[fatal]   Unrecognized OpenCL Device : %s.\n", name);
    exit(-1);
  }

  return t;
}

void acc_register_platform_desc(unsigned r, unsigned first, size_t num) {
  assert(platforms_desc[r].device != acc_device_last); /// \todo unsupported platform

  acc_runtime.devices[platforms_desc[r].device].first = first;
  acc_runtime.devices[platforms_desc[r].device].num   = num;
}

void acc_register_device_type_desc(unsigned r, unsigned s, unsigned first, size_t num) {
  acc_device_t device = platforms_desc[r].devices_type_desc[s].device;

  assert(device != acc_device_last); /// \todo unsupported device type

  acc_runtime.devices[device].first = first;
  acc_runtime.devices[device].num   = num;
}

void acc_register_device_desc(unsigned r, unsigned s, unsigned t, unsigned first, size_t num) {
  acc_device_t device = platforms_desc[r].devices_type_desc[s].devices_desc[t].device;

  assert(device != acc_device_last); /// \todo unsupported device

  acc_runtime.devices[device].first = first;
  acc_runtime.devices[device].num   = num;
}

void acc_init_devices() {
  unsigned i, j;

  for (i = 0; i < acc_device_last; i++) {
    acc_runtime.devices[i].first = 0;
    acc_runtime.devices[i].num   = 0;
  }

  acc_runtime.devices[acc_device_any].first = 0;
  acc_runtime.devices[acc_device_any].num   = acc_runtime.opencl_data->num_devices[acc_runtime.opencl_data->num_platforms];

  /*!
   * \note Mapping devices from OpenCL to OpenACC.
   *
   * This "algorithm" computes the mapping between OpenACC devices and OpenCL devices.
   *
   * Obtained from OpenCL:
   *   - platforms as an array of cl_platform_id
   *   - devices   as an array of cl_device_id
   * Devices are sorted by platform then type (CL_DEVICE_TYPE) and model (CL_DEVICE_NAME).
   * Sorting by platforms is explicit (from OpenCL calls). \todo Check that other order are also true (or enforce)
   *
   * OpenACC devices are organized over 3 levels:
   *   -# Platform : OpenCL Platforms associted to each vendor (Intel, Nvidia, AMD, Altera, ...)
   *   -# Type     : OpenCL type of device : CL_DEVICE_CPU, CL_DEVICE_GPU, CL_DEVICE_ACCELERATOR, ...
   *   -# Model    : OpenCL device's name
   * Each level addresses all devices addressed by its "children".
   * Each [platform], [platform, type], and [platform, type, model] have its own tag in acc_device_t (enumeration).
   *
   * This "algorithm" associates to each of this tag the index of the first matching cl_device_id and the number of matching cl_device_id.
   * 
   * 2 induction variables to go over OpenCL information: (explicit loops)
   *   - i: OpenCL Platforms
   *   - j: OpenCL Devices (for each platform[i])
   *
   * 3 mapping coordinates: (obtained by lookup)
   *   - r: plaform, platforms_desc[r]
   *   - s: type,    platforms_desc[r].devices_type_desc[s]
   *   - t: model,   platforms_desc[r].devices_type_desc[s].devices_desc[t]
   *
   * 5 indexes and counters
   *   - idx : index of the first cl_device_id that match platform[i]
   *   - base_s : index of the first cl_device_id matching platforms_desc[r].devices_type_desc[s]
   *   - cnt_s  : number of devices matching platforms_desc[r].devices_type_desc[s]
   *   - base_t : index of the first cl_device_id matching platforms_desc[r].devices_type_desc[s].devices_desc[t]
   *   - cnt_t  : number of devices matching platforms_desc[r].devices_type_desc[s].devices_desc[t]
   */
  unsigned idx = 0;
  for (i = 0; i < acc_runtime.opencl_data->num_platforms; i++) {

    unsigned r = acc_get_platform_desc(acc_runtime.opencl_data->platforms[i]);

    acc_register_platform_desc(r, idx, acc_runtime.opencl_data->num_devices[i]);

    unsigned base_s = idx;
    unsigned base_t = idx;

    unsigned cnt_s = 1;
    unsigned cnt_t = 1;
 
    // First device

    unsigned s = acc_get_device_type_desc(acc_runtime.opencl_data->devices[i][0], r);
    unsigned t = acc_get_device_desc(acc_runtime.opencl_data->devices[i][0], r, s);

    // Remaining devices, assumes that they are sorted by type then name.

    for (j = 1; j < acc_runtime.opencl_data->num_devices[i]; j++) {
      unsigned prev_s = s;
      unsigned prev_t = t;

      s = acc_get_device_type_desc(acc_runtime.opencl_data->devices[i][0], r);
      t = acc_get_device_desc(acc_runtime.opencl_data->devices[i][0], r, s);

      // When the device type change
      if (s != prev_s) {
        assert(t != prev_t);
        acc_register_device_type_desc(r, prev_s, base_s, cnt_s);
        base_s += cnt_s;
        cnt_s = 0;
      }

      // When the device model change
      if (t != prev_t) {
        acc_register_device_desc(r, prev_s, prev_t, base_t, cnt_t);
        base_t += cnt_t;
        cnt_t = 0;
      }

      cnt_s++;
      cnt_t++;
    }

    acc_register_device_type_desc(r, s, base_s, cnt_s);
    acc_register_device_desc(r, s, t, base_t, cnt_t);

    idx += acc_runtime.opencl_data->num_devices[i];

    assert( (base_s + cnt_s == idx) && (base_t + cnt_t == idx) );
  }

  set_flag(f_acc_devices);
}

void acc_init_defaults() {
  /// \todo Set runtime defaults from environment variables

  acc_runtime.check_list |= f_acc_defaults;
}

acc_region_t acc_build_region(acc_region_desc_t region, size_t num_dims, size_t * num_gang, size_t * num_worker, size_t vector_length) {
  acc_init_once();

  acc_region_t result = (acc_region_t)malloc(sizeof(struct acc_region_t_));

  result->desc          = region;
  result->num_dims      = num_dims;
  result->num_gang      = num_gang;
  result->num_worker    = num_worker;
  result->vector_length = vector_length;

  return result;
}

void acc_region_start(acc_region_t region) {
  acc_init_once();

  acc_region_init(region->desc->id, acc_runtime.curr_device_type, acc_runtime.curr_device_num);

  acc_get_region_defaults(region, acc_runtime.curr_device_type);

  /// \todo acc_region_start : what else?
}

void acc_region_stop(acc_region_t region) {
  acc_init_once();

  /// \todo acc_region_stop : ???
}

acc_kernel_t acc_build_kernel(acc_kernel_desc_t kernel) {
  acc_init_once();

  acc_kernel_t result = (acc_kernel_t)malloc(sizeof(struct acc_kernel_t_));

  result->desc = kernel;

  result->scalar_ptrs = (  void **)malloc(kernel->num_scalars * sizeof(  void *));
  result->data_ptrs   = (d_void **)malloc(kernel->num_datas   * sizeof(d_void *));

  result->loops = (acc_kernel_loop_t)malloc(kernel->num_loops * sizeof(struct acc_kernel_loop_t_));

  return result;
}

void acc_enqueue_kernel(acc_region_t region, acc_kernel_t kernel) {
  acc_init_once();

  assert(region->num_gang > 0 && region->num_worker > 0 && region->vector_length > 0);

  assert(kernel->loops[0]->stride == 1); /// \todo currently only support loops with positive unit stride

  unsigned best_matching_version = 0; // version #0 is always the generic (default) version 
  unsigned best_matching_score = 0;
  unsigned i;
  for (i = 1; i < kernel->desc->num_versions; i++) {
    if (    ( kernel->desc->versions[i]->num_gang == 0      || kernel->desc->versions[i]->num_gang      == region->num_gang      )
         && ( kernel->desc->versions[i]->num_worker == 0    || kernel->desc->versions[i]->num_worker    == region->num_worker    )
         && ( kernel->desc->versions[i]->vector_length == 0 || kernel->desc->versions[i]->vector_length == region->vector_length )
    ) {
      unsigned score = 0;
      if (kernel->desc->versions[i]->num_gang      == region->num_gang     ) score++;
      if (kernel->desc->versions[i]->num_worker    == region->num_worker   ) score++;
      if (kernel->desc->versions[i]->vector_length == region->vector_length) score++;

      if (kernel->desc->versions[i]->loops_tiling != NULL) {
        /// \todo check that tilling is compatible...
        /// \todo tile matching/scoring
      }

      if (score >= best_matching_score) {
        best_matching_version = i;
        best_matching_score = score;
      }
    }
  }

  char * version_suffix = kernel->desc->versions[best_matching_version]->suffix;

  char * kernel_name = (char *)malloc((strlen(kernel->desc->name) + strlen(version_suffix) + 1) * sizeof(char));
  strcpy(kernel_name, kernel->desc->name);
  strcat(kernel_name, version_suffix);

  assert(!"NIY"); /// \todo
}

void acc_dbg_dump_runtime() {
  printf("Device type : ");
  switch (acc_runtime.curr_device_type) {
    case acc_device_any:     printf("ACC_DEVICE_ANY\n");     break;
    case acc_device_nvidia:  printf("ACC_DEVICE_NVIDIA\n");  break;
    case acc_device_radeon:  printf("ACC_DEVICE_RADEON\n");  break;
    case acc_device_xeonphi: printf("ACC_DEVICE_XEONPHI\n"); break;
    default:
      printf("invalid !\n");
  }

  printf("Device Number : %u\n", acc_runtime.curr_device_num);
  
  if (acc_runtime.opencl_data != NULL) {
    printf("OpenCL Data:\n");
    unsigned i, j;
    for (i = 0; i < acc_runtime.opencl_data->num_platforms; i++) {
      char buf[100];
      printf("  Platform %u: \n", i);
      clGetPlatformInfo(acc_runtime.opencl_data->platforms[i], CL_PLATFORM_VENDOR, sizeof(buf), buf, NULL);
      printf("    Vendor: %s\n", buf);
      clGetPlatformInfo(acc_runtime.opencl_data->platforms[i], CL_PLATFORM_NAME, sizeof(buf), buf, NULL);
      printf("    Name: %s\n", buf);
      printf("    Number Devices: %u\n", acc_runtime.opencl_data->num_devices[i]);

      for (j = 0; j < acc_runtime.opencl_data->num_devices[i]; j++) {
        printf("    Device %u: \n", j);
        clGetDeviceInfo((acc_runtime.opencl_data->devices)[i][j], CL_DEVICE_VENDOR, sizeof(buf), buf, NULL);
        printf("      Vendor: %s\n", buf);
        clGetDeviceInfo((acc_runtime.opencl_data->devices)[i][j], CL_DEVICE_NAME, sizeof(buf), buf, NULL);
        printf("      Name: %s\n", buf);
      }
    }
  }
  else
    printf("No OpenCL Data !\n");
}

// OpenACC internal API

acc_context_t acc_create_context(acc_region_t region, acc_kernel_t kernel) {
  return NULL; /// \todo 
}

void acc_region_init(size_t region_id, acc_device_t dev, int num) {
  unsigned first_device = acc_runtime.devices[dev].first;
  size_t num_devices = acc_get_num_devices(dev);

  assert(num >= 0 && num < num_devices);

  unsigned device_idx = first_device + num;

  assert(acc_runtime.opencl_data->devices_data[device_idx] != NULL);

  if (acc_runtime.opencl_data->devices_data[device_idx]->programs[region_id] == NULL) {
    cl_context * context = &(acc_runtime.opencl_data->devices_data[device_idx]->context);
    cl_program * program = &(acc_runtime.opencl_data->devices_data[device_idx]->programs[region_id]);

    cl_int status;

    char * ocl_sources[2] = {
      acc_runtime.opencl_data->runtime_sources,
      acc_runtime.opencl_data->region_sources[region_id]
    };

    *program = clCreateProgramWithSource(
                             *context,
                             2,
                             ocl_sources,
                             NULL,
                             &status
                           );
    if (status != CL_SUCCESS) {
      printf("[fatal]   clCreateProgramWithSource : %s, %u for region %u return %u : failed\n", acc_device_name[dev], num, region_id, status);
      exit(-1);
    }

    char build_options[1024];
    build_options[0] = '\0';
    strcpy(build_options, "-I");
    strcat(build_options, compiler_data.acc_runtime_dir);
    strcat(build_options, "/include/ ");
    strcat(build_options, "-DOPENACC_HOST_RUNTIME_NAME=\"OpenACC for Rose Compiler\" ");
    strcat(build_options, "-DOPENACC_HOST_RUNTIME_VERSION=201310 ");

    assert(compiler_data.regions[region_id]->num_options == 0 || compiler_data.regions[region_id]->options != NULL);

    unsigned i;
    for (i = 0; i < compiler_data.regions[region_id]->num_options; i++) {
      strcat(build_options, compiler_data.regions[region_id]->options[i]);
      strcat(build_options, " ");
    }

    status = clBuildProgram(*program, 1, &(acc_runtime.opencl_data->devices[0][device_idx]), build_options, NULL, NULL);
#if PRINT_BUILD_LOG
    print_build_log(device_idx, *program, dev, num);
#endif
    if (status != CL_SUCCESS) {
      printf("[fatal]   clBuildProgram : %s, %u for region %u return %u : failed\n", acc_device_name[dev], num, region_id, status);
      exit(-1);
    }
  }
}

void acc_get_region_defaults(acc_region_t region, acc_device_t dev) {
  if (region->num_gang == 0) {
    region->num_gang = acc_device_defaults[dev].num_gang;
  }
  if (region->num_worker == 0) {
    region->num_worker = acc_device_defaults[dev].num_worker;
  }
  if (region->vector_length == 0) {
    region->vector_length = acc_device_defaults[dev].vector_length;
  }
}

char * readSource(const char * sourceFilename) {

   FILE *fp;
   int err;
   int size;

   char *source;

   fp = fopen(sourceFilename, "rb");
   if(fp == NULL) {
      printf("Could not open kernel file: %s\n", sourceFilename);
      exit(-1);
   }
   
   err = fseek(fp, 0, SEEK_END);
   if(err != 0) {
      printf("Error seeking to end of file\n");
      exit(-1);
   }

   size = ftell(fp);
   if(size < 0) {
      printf("Error getting file position\n");
      exit(-1);
   }

   err = fseek(fp, 0, SEEK_SET);
   if(err != 0) {
      printf("Error seeking to start of file\n");
      exit(-1);
   }

   source = (char*)malloc(size+1);
   if(source == NULL) {
      printf("Error allocating %d bytes for the program source\n", size+1);
      exit(-1);
   }

   err = fread(source, 1, size, fp);
   if(err != size) {
      printf("only read %d bytes\n", err);
      exit(0);
   }

   source[size] = '\0';

   return source;
}

/** @} */

