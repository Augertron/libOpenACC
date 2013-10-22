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

#include <assert.h>

acc_runtime_t acc_runtime = {0, NULL, acc_device_any, 0}; // by default, we use the first of any device

const char * acc_device_env_name [acc_device_last] = {
  "ANY",
  "NVIDIA",
  "AMD",
  "RADEON",
  "INTEL",
  "CORE",
  "I7-3610QM",
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
  "Intel(R) XeonPhi(TM)"
};

device_type_desc_t nvidia_devices_type_desc [3] = {
  { CL_DEVICE_TYPE_CPU         , acc_device_last    , 0, NULL },
  { CL_DEVICE_TYPE_GPU         , acc_device_last    , 0, NULL }, ///< \todo NVidia GPU
  { CL_DEVICE_TYPE_ACCELERATOR , acc_device_last    , 0, NULL }
};

device_type_desc_t amd_devices_type_desc [3] = {
  { CL_DEVICE_TYPE_CPU         , acc_device_last    , 0, NULL },
  { CL_DEVICE_TYPE_GPU         , acc_device_radeon  , 0, NULL }, ///< Radeon
  { CL_DEVICE_TYPE_ACCELERATOR , acc_device_last    , 0, NULL }
};

device_desc_t intel_cpu_devices_desc [1] = {
  { "Intel(R) Core(TM) i7-3610QM CPU" , acc_device_i7_3610QM } ///< My laptop
};

device_type_desc_t intel_devices_type_desc [3] = {
  { CL_DEVICE_TYPE_CPU         , acc_device_core    , 1, intel_cpu_devices_desc }, ///< Core
  { CL_DEVICE_TYPE_GPU         , acc_device_last    , 0, NULL                   },
  { CL_DEVICE_TYPE_ACCELERATOR , acc_device_xeonphi , 0, NULL                   }  ///< XeonPhi
};

device_type_desc_t altera_devices_type_desc [3] = {
  { CL_DEVICE_TYPE_CPU         , acc_device_last    , 0, NULL },
  { CL_DEVICE_TYPE_GPU         , acc_device_last    , 0, NULL },
  { CL_DEVICE_TYPE_ACCELERATOR , acc_device_last    , 0, NULL }
};

platform_desc_t platforms_desc[NUM_OPENCL_PLATFORMS] = {
  { "Nvidia",          acc_device_nvidia, 3, nvidia_devices_type_desc }, /// \todo find actual platform name for Nvidia
  { "AMD",             acc_device_amd,    3, amd_devices_type_desc    }, /// \todo find actual platform name for AMD
  { "Intel(R) OpenCL", acc_device_intel,  3, intel_devices_type_desc  },
  { "Altera",          acc_device_altera, 3, altera_devices_type_desc }  /// \todo find actual platform name for Altera
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

  assert("NIY"); /// \todo
  return -1;
}

int acc_async_test_all() {
  acc_init_once();

  assert("NIY"); /// \todo
  return -1;
}

void acc_async_wait(int id) {
  acc_init_once();

  assert("NIY"); /// \todo
}

void acc_async_wait_async(int id_wait, int id_async) {
  acc_init_once();

  assert("NIY"); /// \todo
}

void acc_async_wait_all() {
  acc_init_once();

  assert("NIY"); /// \todo
}

void acc_async_wait_all_async(int id) {
  acc_init_once();

  assert("NIY"); /// \todo
}

void acc_init(acc_device_t dev) {
  acc_init_once();

  size_t num_devices = acc_get_num_devices(dev);

  assert(num_devices > 0);

  unsigned i;
  for (i = 0; i < num_devices; i++)
    acc_init_(dev, i);
}

void acc_init_(acc_device_t dev, int num) {
  acc_init_once();

  unsigned first_device = acc_runtime.devices[dev].first;
  size_t num_devices = acc_get_num_devices(dev);

  assert(num >= 0 && num < num_devices);

  unsigned device_idx = first_device + num;

  if (acc_runtime.opencl_data->devices_data[device_idx] == NULL) {
    cl_int status;

    cl_device_id * device = acc_runtime.opencl_data->devices[device_idx];

    acc_device_data_t device_data = (acc_device_data_t)malloc(sizeof(struct acc_device_data_t_));
    if (device_data == NULL) {
      printf("[fatal]   malloc : device_data %s, %u\n", acc_device_name[dev], num);
      exit(-1);
    }
    acc_runtime.opencl_data->devices_data[device_idx] = device_data;

    device_data->context = clCreateContext(NULL, 1, device, NULL, NULL, &status);
    if (status != CL_SUCCESS || device_data->context == NULL) {
       printf("[error]   clCreateContext : %s, %u failed\n", acc_device_name[dev], num);

       free(device_data);
       device_data = NULL;

       return;
    }

    /// \todo OpenCL program

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
    /// \todo delete OpenCL context and program

    free(acc_runtime.opencl_data->devices_data[device_idx]);

    acc_runtime.opencl_data->devices_data[device_idx] = NULL;
  }
}

int acc_on_device(acc_device_t dev) {
  acc_init_once();

  assert("NIY"); /// \todo
  return -1;
}

d_void * acc_malloc (size_t n) {
  acc_init_once();

  assert("NIY"); /// \todo
  return NULL;
}

void acc_free(d_void * dev_ptr) {
  acc_init_once();

  assert("NIY"); /// \todo
}

void * acc_copyin(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert("NIY"); /// \todo
  return NULL;
}

void * acc_create(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert("NIY"); /// \todo
  return NULL;
}

void * acc_present_or_create(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert("NIY"); /// \todo
  return NULL;
}

void * acc_pcreate(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert("NIY"); /// \todo
  return NULL;
}

void acc_copyout(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert("NIY"); /// \todo
}

void acc_delete(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert("NIY"); /// \todo
}

void acc_update_device(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert("NIY"); /// \todo
}

void acc_update_self(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert("NIY"); /// \todo
}

void acc_map_data(h_void * host_ptr, d_void * dev_ptr, size_t n) {
  acc_init_once();

  assert("NIY"); /// \todo
}

void acc_unmap_data(h_void * host_ptr) {
  acc_init_once();

  assert("NIY"); /// \todo
}

d_void * acc_deviceptr(h_void * host_ptr) {
  acc_init_once();

  assert("NIY"); /// \todo
  return NULL;
}

h_void * acc_hostptr(d_void * dev_ptr) {
  acc_init_once();

  assert("NIY"); /// \todo
  return NULL;
}

int acc_is_present(h_void * host_ptr, size_t n) {
  acc_init_once();

  assert("NIY"); /// \todo
  return -1;
}

void acc_memcpy_to_device(d_void * dest, h_void * src, size_t bytes) {
  acc_init_once();

  assert("NIY"); /// \todo
}

void acc_memcpy_from_device(h_void * dest, d_void * src, size_t bytes) {
  acc_init_once();

  assert("NIY"); /// \todo
}

// OpenACC private API

void acc_fail_if_error(acc_error_t err) {
  assert("NIY"); /// \todo
}

int check_flag(uint32_t flag) {
  return (acc_runtime.check_list & flag) == flag;
}

void set_flag(uint32_t flag) {
  acc_runtime.check_list |= flag;
}

void acc_init_once() {
  if (!check_flag(f_alloc)) {
    acc_runtime.opencl_data = (acc_runtime_ocl_t)malloc(sizeof(struct acc_runtime_ocl_t_));
    set_flag(f_alloc);
  }

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
  acc_runtime.opencl_data->status = clGetPlatformIDs(0, NULL, &(acc_runtime.opencl_data->num_platforms));
  if (acc_runtime.opencl_data->status != CL_SUCCESS) {
    printf("[error]   clGetPlatformIDs(0, NULL, &(acc_runtime.opencl_data->num_platforms)) failed\n");
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

  acc_runtime.opencl_data->status = clGetPlatformIDs(acc_runtime.opencl_data->num_platforms, acc_runtime.opencl_data->platforms, NULL);
  if (acc_runtime.opencl_data->status != CL_SUCCESS) {
    printf("[error]   clGetPlatformIDs(acc_runtime.opencl_data->num_platforms, acc_runtime.opencl_data->platforms, NULL) failed\n");
    return;
  }

  unsigned i;
  unsigned num_devices = 0;
  for (i = 0; i < acc_runtime.opencl_data->num_platforms; i++) {
    const cl_platform_id platform = acc_runtime.opencl_data->platforms[i];

    acc_runtime.opencl_data->status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &(acc_runtime.opencl_data->num_devices[i]));
    if(acc_runtime.opencl_data->status != CL_SUCCESS) {
     printf("[warning] clGetDeviceIDs(platform[%u], CL_DEVICE_TYPE_ALL, 0, NULL, acc_runtime.opencl_data->num_devices[%u]) failed [continue]\n", i, i);

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
      acc_runtime.opencl_data->status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, acc_runtime.opencl_data->num_devices[i], devices, NULL);
      if (acc_runtime.opencl_data->status != CL_SUCCESS) {
        printf("[warning] clGetDeviceIDs(platform[%u], CL_DEVICE_TYPE_ALL, acc_runtime.opencl_data->num_devices[%u], devices, NULL) failed\n", i, i);

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

  acc_runtime.opencl_data->num_ocl_sources = 0;
  acc_runtime.opencl_data->opencl_sources = NULL;

  if (compiler_data.num_ocl_files <= 0) {
    printf("[error]   No OpenCL source file found listed!\n");
    return;
  }

  size_t num_ocl_sources = compiler_data.num_ocl_files + 1;

  char ** opencl_sources = (char **)malloc(num_ocl_sources * sizeof(char *));
  if (opencl_sources == NULL) {
    perror("[fatal]   malloc: acc_runtime.opencl_data->opencl_sources");
    exit(-1);
  }

  char ocl_rt_file[50];
  strcpy(ocl_rt_file, compiler_data.acc_runtime_dir);
  strcat(ocl_rt_file, "/");
  strcat(ocl_rt_file, compiler_data.acc_runtime_ocl);
  opencl_sources[0] = readSource(ocl_rt_file);

  for (i = 1; i < num_ocl_sources; i++) {
    opencl_sources[i] = readSource(compiler_data.opencl_files[i-1]);
  }

  acc_runtime.opencl_data->num_ocl_sources = num_ocl_sources;
  acc_runtime.opencl_data->opencl_sources = opencl_sources;

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
  assert(t < platforms_desc[r].devices_type_desc[s].num_devices); /// \todo unrecognized CL_DEVICE_NAME

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

acc_parallel_t acc_build_parallel(unsigned num_dims, unsigned long * num_gang, unsigned long * num_worker, unsigned num_vector) {
  assert("NIY"); /// \todo
  return NULL;
}

acc_error_t acc_parallel_start(acc_parallel_t region) {
  assert("NIY"); /// \todo
  return -1;
}

acc_error_t acc_parallel_stop (acc_parallel_t region) {
  assert("NIY"); /// \todo
  return -1;
}

acc_kernel_t acc_build_kernel(unsigned kernel_id) {
  assert("NIY"); /// \todo
  return NULL;
}

acc_error_t acc_enqueue_kernel(acc_parallel_t region, acc_kernel_t kernel) {
  assert("NIY"); /// \todo
  return -1;
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

acc_context_t acc_create_context(acc_parallel_t region, acc_kernel_t kernel) {
  return NULL; /// \todo 
}

char* readSource(const char *sourceFilename) {

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

