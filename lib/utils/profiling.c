
#include "OpenACC/utils/profiling.h"

#include "OpenACC/private/runtime.h"
#include "OpenACC/private/debug.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <assert.h>

#undef ENABLE_LOGGING

#ifdef PRINT_INFO
# define PRINT_INFO 0
#endif

acc_profiler_t acc_profiler = NULL;

void acc_profiling_platform_init();

void acc_profiling_device_init(cl_device_id dev, cl_uint DeviceId, cl_uint PlatformId);

void acc_profiling_ocl_error (cl_int error, int line_no);

void
print_log (const char *fmt, ...)
{
#ifdef ENABLE_LOGGING
  va_list args;
  va_start (args, fmt);
  vprintf (fmt, args);
  va_end (args);
#endif
}

struct acc_event_data_t {
  size_t device_idx;
  char * command_name;
  size_t command_id;
  size_t run_id;
};

const char * acc_command_memcpy_to_device = "memcpy_to_device";
const char * acc_command_memcpy_from_device = "memcpy_from_device";
const char * acc_command_kernel_launch = "kernel_launch";

static int cl_event_cmp(const cl_event * e0, const cl_event * e1) {
  return *e0 < *e1;
}

static int inc_line_cnt(int * cnt, int argc, char **argv, char **azColName) {
  (*cnt)++;
  return 0;
}

void acc_profiling_release_event(cl_event event, struct acc_event_data_t * event_data);

void acc_profiling_set_experiment(char * user_fields) {
  char * err_msg;
  char * query = "SELECT * FROM sqlite_master WHERE name ='Runs' and type='table';";
  int cnt = 0;
  int status = sqlite3_exec (acc_profiler->db_file, query, &inc_line_cnt, &cnt, &err_msg);
  assert (status == SQLITE_OK);

  if (cnt == 0) {
    query = (char*)malloc((strlen(user_fields) + 24) * sizeof(char));
    sprintf(query, "CREATE TABLE Runs ( %s );", user_fields);
    status = sqlite3_exec (acc_profiler->db_file, query, NULL, NULL, &err_msg);
    assert (status == SQLITE_OK);
    free(query);
  }
}

void acc_profiling_new_run(char * user_data) {
  char * err_msg;
  char * query = (char*)malloc((strlen(user_data) + 30) * sizeof(char));
  sprintf(query, "INSERT INTO Runs VALUES ( %s );", user_data);
  int status = sqlite3_exec (acc_profiler->db_file, query, NULL, NULL, &err_msg);
  assert (status == SQLITE_OK);

  acc_profiler->run_id = sqlite3_last_insert_rowid(acc_profiler->db_file);
}

void acc_profiling_create_event_table();
void acc_profiling_create_platform_table();
void acc_profiling_create_device_table();

void acc_profiling_init_db() {
  // Check if DB file already exist
  struct stat buffer;   
  int file_existed = stat(acc_profiler->db_file_name, &buffer) == 0;

  // Open DB file
  int DbErr = sqlite3_open (acc_profiler->db_file_name, &acc_profiler->db_file);
  assert (DbErr == SQLITE_OK);

  if (!file_existed)
    {
      acc_profiling_create_platform_table();
      acc_profiling_create_device_table();

      acc_profiling_platform_init();

      acc_profiling_create_event_table();
    }
}

/// Build DB file name (get filename from environment or build $USER_$HOSTNAME.sl3)
void acc_profiling_get_db_file_names() {
  char * env_prof_db = getenv("ACC_PROFILING_DB");
  if (env_prof_db != NULL && env_prof_db[0] != '\0') {
    assert(strlen(env_prof_db) < 140);
    acc_profiler->db_file_name = (char *)malloc((strlen(env_prof_db) + 1) * sizeof(char));
    strcpy (acc_profiler->db_file_name, env_prof_db);
  }
  else {
    char hostname[64];
      gethostname (hostname, 64);
    char username[64];
      getlogin_r (username, 64);

    size_t length = strlen(username) + strlen(hostname) + 5;
    acc_profiler->db_file_name = (char *)malloc((length + 1) * sizeof(char));
    strcpy (acc_profiler->db_file_name, username);
    strcat (acc_profiler->db_file_name, "_");
    strcat (acc_profiler->db_file_name, hostname);
    strcat (acc_profiler->db_file_name, ".sl3");
    acc_profiler->db_file_name[length] = '\0';
  }
}

void
acc_profiling_init ()
{
  assert(acc_profiler == NULL);
  acc_profiler = malloc(sizeof(struct acc_profiler_t_));

  acc_profiling_get_db_file_names();

  acc_profiler->run_id = -1;

  acc_profiling_init_db();

  acc_profiler->events = map_alloc(42, sizeof(cl_event), sizeof(struct acc_event_data_t *), &cl_event_cmp);
}

void
acc_profiling_exit ()
{
  if (acc_profiler != NULL) {
    size_t i;
    for (i = 0; i < acc_profiler->events->count; i++) {
      acc_profiling_release_event(
        *(cl_event *)(acc_profiler->events->datas + i * (sizeof(cl_event) + sizeof(struct acc_event_data_t *))),
        *(struct acc_event_data_t **)(acc_profiler->events->datas + i * (sizeof(cl_event) + sizeof(struct acc_event_data_t *)) + sizeof(cl_event))
      );
    }

    if (acc_profiler->db_file_name != NULL)
      free(acc_profiler->db_file_name);
    if (acc_profiler->db_file != NULL) {
      /// \todo close DB 
    }

    free(acc_profiler);
  }
}

void acc_profiling_get_profile_from_event(cl_event event, cl_ulong * queued, cl_ulong * submit, cl_ulong * start, cl_ulong * end) {
  cl_int status = clGetEventProfilingInfo (event, CL_PROFILING_COMMAND_QUEUED,
					   sizeof (cl_ulong), queued,
					   NULL);
  if (status != CL_SUCCESS)
    {
      const char *status_str = acc_ocl_status_to_char (status);
      printf ("[fatal]   clGetEventProfilingInfo return %s.\n", status_str);
      exit (-1);		/// \todo error code
    }

  status =
    clGetEventProfilingInfo (event, CL_PROFILING_COMMAND_SUBMIT,
			     sizeof (cl_ulong), submit, NULL);
  if (status != CL_SUCCESS)
    {
      const char *status_str = acc_ocl_status_to_char (status);
      printf ("[fatal]   clGetEventProfilingInfo return %s.\n", status_str);
      exit (-1);		/// \todo error code
    }

  status =
    clGetEventProfilingInfo (event, CL_PROFILING_COMMAND_START,
			     sizeof (cl_ulong), start, NULL);
  if (status != CL_SUCCESS)
    {
      const char *status_str = acc_ocl_status_to_char (status);
      printf ("[fatal]   clGetEventProfilingInfo return %s.\n", status_str);
      exit (-1);		/// \todo error code
    }

  status =
    clGetEventProfilingInfo (event, CL_PROFILING_COMMAND_END,
			     sizeof (cl_ulong), end, NULL);
  if (status != CL_SUCCESS)
    {
      const char *status_str = acc_ocl_status_to_char (status);
      printf ("[fatal]   clGetEventProfilingInfo return %s.\n", status_str);
      exit (-1);		/// \todo error code
    }
}

void acc_profiling_release_event(cl_event event, struct acc_event_data_t * event_data) {
  cl_ulong queued, submit, start, end;

#if PRINT_INFO
  printf("[info]    acc_profiling_release_event\n");
#endif

  acc_profiling_get_profile_from_event(event, &queued, &submit, &start, &end);

  char Dbstr[8192];
  sprintf (Dbstr,
	   "INSERT INTO Events VALUES ( '%zd', '%zd', '%s', '%zd', '%lu', '%lu', '%lu', '%lu');",
	   event_data->run_id, event_data->device_idx, event_data->command_name, event_data->command_id,
           queued, submit, start, end);

  char *DbErrMsg;
  int DbErr =
    sqlite3_exec (acc_profiler->db_file, Dbstr, NULL, 0, &DbErrMsg);
  assert (DbErr == SQLITE_OK);

  free(event_data);
}

void acc_profiling_register_memcpy_to_device(cl_event event, size_t device_idx, h_void * host_ptr, d_void * dev_ptr, size_t size) {
  struct acc_event_data_t * event_data = (struct acc_event_data_t*)malloc(sizeof(struct acc_event_data_t));
    event_data->device_idx = device_idx;
    event_data->command_name = acc_command_memcpy_to_device;
    event_data->command_id = -1;
    event_data->run_id = acc_profiler->run_id;
  map_insert(acc_profiler->events, &event, &event_data);

  /// \todo save command
#if PRINT_INFO
  printf("[info]    acc_profiling_register_memcpy_to_device\n");
#endif
/*
  cl_int status = clSetEventCallback(event, CL_COMPLETE, &acc_profiling_event_callback, event_data);
  if (status != CL_SUCCESS) {
    const char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal]   clSetEventCallback return %s.\n", status_str);
    exit(-1); /// \todo error code
  }
*/
}

void acc_profiling_register_memcpy_from_device(cl_event event, size_t device_idx, h_void * host_ptr, d_void * dev_ptr, size_t size) {
  struct acc_event_data_t * event_data = (struct acc_event_data_t*)malloc(sizeof(struct acc_event_data_t));
    event_data->device_idx = device_idx;
    event_data->command_name = acc_command_memcpy_from_device;
    event_data->command_id = -1;
    event_data->run_id = acc_profiler->run_id;
  map_insert(acc_profiler->events, &event, &event_data);

  /// \todo save command
#if PRINT_INFO
  printf("[info]    acc_profiling_register_memcpy_from_device\n");
#endif
/*
  cl_int status = clSetEventCallback(event, CL_COMPLETE, &acc_profiling_event_callback, event_data);
  if (status != CL_SUCCESS) {
    const char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal]   clSetEventCallback return %s.\n", status_str);
    exit(-1); /// \todo error code
  }
*/
}

void acc_profiling_register_kernel_launch(cl_event event, size_t device_idx, size_t region_id, size_t kernel_id) {
  struct acc_event_data_t * event_data = (struct acc_event_data_t*)malloc(sizeof(struct acc_event_data_t));
    event_data->device_idx = device_idx;
    event_data->command_name = acc_command_kernel_launch;
    event_data->command_id = -1;
    event_data->run_id = acc_profiler->run_id;
  map_insert(acc_profiler->events, &event, &event_data);

  /// \todo save command
#if PRINT_INFO
  printf("[info]    acc_profiling_register_kernel_launch\n");
#endif
/*
  cl_int status = clSetEventCallback(event, CL_COMPLETE, &acc_profiling_event_callback, event_data);
  if (status != CL_SUCCESS) {
    const char * status_str = acc_ocl_status_to_char(status);
    printf("[fatal]   clSetEventCallback return %s.\n", status_str);
    exit(-1); /// \todo error code
  }
*/
}

// Prints a string version of the specified OpenCL error code
void
acc_profiling_ocl_error (cl_int error, int line_no)
{
  printf ("Fatal at line %d\t%s\n", line_no, acc_ocl_status_to_char (error));
  exit (error);
}

// Create Event tables in the database file. 
void acc_profiling_create_event_table() {
  int DbErr;
  char *DbErrMsg;
  char Dbstr[8192];

  sprintf(Dbstr, "CREATE TABLE Events (  \
                    RUN_ID INT, \
                    DEVICE_ID INT, \
                    COMMAND_NAME CHAR(128), \
                    COMMAND_ID INT, \
                    CL_PROFILING_COMMAND_QUEUED BIGINT, \
                    CL_PROFILING_COMMAND_SUBMIT BIGINT, \
                    CL_PROFILING_COMMAND_START  BIGINT, \
                    CL_PROFILING_COMMAND_END    BIGINT  \
                  );");

  DbErr = sqlite3_exec (acc_profiler->db_file, Dbstr, NULL, 0, &DbErrMsg);
  assert (DbErr == SQLITE_OK);
}

void acc_profiling_create_platform_table() {
  int DbErr;
  char *DbErrMsg;
  char Dbstr[8192];

  sprintf (Dbstr, "CREATE TABLE Platform(  \
                     ID INT, \
                     CL_PLATFORM_NAME CHAR(100), \
                     CL_PLATFORM_VERSION CHAR(100), \
                     CL_DEVICE_TYPE_ALL INT \
                   );");

  DbErr = sqlite3_exec (acc_profiler->db_file, Dbstr, NULL, 0, &DbErrMsg);
  assert (DbErr == SQLITE_OK);
}

void acc_profiling_create_device_table() {
  int DbErr;
  char *DbErrMsg;
  char Dbstr[8192];

  sprintf (Dbstr, "CREATE TABLE Device(  \
                     DEVICE_ID INT, \
                     PLATFORM_ID INT, \
                     CL_DEVICE_NAME CHAR(128),\
                     CL_DEVICE_VENDOR CHAR(128),\
                     CL_DRIVER_VERSION CHAR(128),\
                     CL_DEVICE_VERSION CHAR(128),\
                     CL_DEVICE_TYPE CHAR(32),\
                     CL_DEVICE_MAX_COMPUTE_UNITS INT,\
                     CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS INT,\
                     CL_DEVICE_MAX_WORK_ITEM_SIZES CHAR(64),\
                     CL_DEVICE_MAX_WORK_GROUP_SIZE INT,\
                     CL_DEVICE_MAX_CLOCK_FREQUENCY INT,\
                     CL_DEVICE_ADDRESS_BITS INT,\
                     CL_DEVICE_MAX_MEM_ALLOC_SIZE INT,\
                     CL_DEVICE_GLOBAL_MEM_SIZE INT ,\
                     CL_DEVICE_ERROR_CORRECTION_SUPPORT BOOL,\
                     CL_DEVICE_LOCAL_MEM_TYPE CHAR(32),\
                     CL_DEVICE_LOCAL_MEM_SIZE INT,\
                     CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE INT,\
                     CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE BOOL,\
                     CL_QUEUE_PROFILING_ENABLE BOOL,\
                     CL_DEVICE_IMAGE_SUPPORT BOOL,\
                     CL_DEVICE_MAX_READ_IMAGE_ARGS INT,\
                     CL_DEVICE_MAX_WRITE_IMAGE_ARGS INT,\
                     CL_FP_DENORM BOOL,\
                     CL_FP_INF_NAN BOOL,\
                     CL_FP_ROUND_TO_NEAREST BOOL,\
                     CL_FP_ROUND_TO_ZERO BOOL,\
                     CL_FP_ROUND_TO_INF BOOL,\
                     CL_FP_FMA BOOL\
                   );");

  DbErr = sqlite3_exec (acc_profiler->db_file, Dbstr, NULL, 0, &DbErrMsg);
  assert (DbErr == SQLITE_OK);
}

void
acc_profiling_platform_init(void)
{
  int DbErr;
  char *DbErrMsg;
  char Dbstr[8192];

  //Create database file
  cl_bool cl_queue_out_of_order_exec_mode_enable = false;
  cl_bool cl_queue_profiling_enable = false;
  cl_bool cl_device_image_support = false;
  cl_uint cl_device_max_read_image_args;
  cl_uint cl_device_max_write_image_args;
  cl_bool cl_fp_denorm = false;
  cl_bool cl_fp_inf_nan = false;
  cl_bool cl_fp_round_to_nearest = false;
  cl_bool cl_fp_round_to_zero = false;
  cl_bool cl_fp_round_to_inf = false;
  cl_bool cl_fp_fma = false;

  //----------------------Platform check---------------------------
  cl_int ciErrNum = CL_SUCCESS;
  cl_uint num_platforms;
  // Get the number of platforms
  ciErrNum = clGetPlatformIDs (0, NULL, &num_platforms);
  if (ciErrNum != CL_SUCCESS)
    acc_profiling_ocl_error (ciErrNum, __LINE__);

  // Get the list of platforms
  cl_platform_id *platforms =
    (cl_platform_id *) malloc (sizeof (cl_platform_id) * num_platforms);
  ciErrNum = clGetPlatformIDs (num_platforms, platforms, NULL);
  if (ciErrNum != CL_SUCCESS)
    acc_profiling_ocl_error (ciErrNum, __LINE__);

  print_log ("There are %d platforms\n\n", num_platforms);
  cl_platform_id platform;

  char pbuf[2048];
  cl_uint GlobalDeviceId = 0;
  int i;
  for (i = 0; i < num_platforms; i++)
    {
      cl_uint PlatformId = i;
      char PlatformName[100];
      char PlatformVersion[100];

      platform = platforms[i];
      print_log
	("-----------------------------------Platform%d:-----------------------------------\n",
	 PlatformId);
      ciErrNum =
	clGetPlatformInfo (platform, CL_PLATFORM_NAME,
			   sizeof (PlatformName), PlatformName, NULL);
      if (ciErrNum != CL_SUCCESS)
	acc_profiling_ocl_error (ciErrNum, __LINE__);
      print_log (" CL_PLATFORM_NAME: \t%s\n", PlatformName);

      ciErrNum =
	clGetPlatformInfo (platform, CL_PLATFORM_VERSION,
			   sizeof (PlatformVersion), PlatformVersion, NULL);
      if (ciErrNum != CL_SUCCESS)
	acc_profiling_ocl_error (ciErrNum, __LINE__);
      print_log (" CL_PLATFORM_VERSION: \t%s\n", PlatformVersion);

      cl_uint ciDeviceCount;
      cl_device_id *devices;
      print_log ("OpenCL Device Info:\n");
      ciErrNum =
	clGetDeviceIDs (platform, CL_DEVICE_TYPE_ALL, 0, NULL,
			&ciDeviceCount);

      // check for 0 devices found or errors...

      assert (ciDeviceCount > 0);
      assert (ciErrNum == CL_SUCCESS);
	    //Store into SQLite3 database Platform
	    sprintf (Dbstr, "INSERT INTO Platform VALUES (\
                            '%d',\
                            '%s',\
                            '%s',\
                            '%d');", i, PlatformName, PlatformVersion, ciDeviceCount);
	    DbErr =
	      sqlite3_exec (acc_profiler->db_file, Dbstr, NULL, 0,
			    &DbErrMsg);
	    assert (DbErr == SQLITE_OK);
	// Get and log the OpenCL device ID's
	print_log (" %u devices found supporting OpenCL:\n", ciDeviceCount);
	devices =
	  (cl_device_id *) malloc (sizeof (cl_device_id) * ciDeviceCount);
	assert (devices != NULL);

	ciErrNum =
	  clGetDeviceIDs (platform, CL_DEVICE_TYPE_ALL, ciDeviceCount,
			  devices, &ciDeviceCount);
	if (ciErrNum != CL_SUCCESS)
	  acc_profiling_ocl_error (ciErrNum, __LINE__);
	//Create a context for the devices
	cl_context cxGPUContext =
	  clCreateContext (0, ciDeviceCount, devices, NULL, NULL,
			   &ciErrNum);
	if (ciErrNum != CL_SUCCESS)
	  acc_profiling_ocl_error (ciErrNum, __LINE__);
	// show info for each device in the context
	uint i;
	for (i = 0; i < ciDeviceCount; ++i)
	  {
	    acc_profiling_device_init (devices[i], GlobalDeviceId, PlatformId);
	    GlobalDeviceId++;
	  }
      print_log ("\n\n");
    }
  //----------------------Platform check---------------------------
}


void
acc_profiling_device_init(cl_device_id dev, cl_uint DeviceId, cl_uint PlatformId)
{
  int DbErr;
  char *DbErrMsg;
  char Dbstr[8192];
  char device_string[128];
  char cl_device_name[128];
  char cl_device_vendor[128];
  char cl_driver_version[128];
  char cl_device_version[128];
  char CL_device_type[32];
  cl_uint cl_device_max_compute_units;
  cl_uint cl_device_max_work_item_dimensions;
  char cl_device_max_work_item_sizes[64];
  cl_uint cl_device_max_work_group_size;
  cl_uint cl_device_max_clock_frequency;
  cl_uint cl_device_address_bits;
  cl_ulong cl_device_max_mem_alloc_size;
  cl_ulong cl_device_global_mem_size;
  cl_bool cl_device_error_correction_support;
  char CL_device_local_mem_type[32];
  cl_ulong cl_device_local_mem_size;
  unsigned long long cl_device_max_constant_buffer_size;
  cl_bool cl_queue_out_of_order_exec_mode_enable = false;
  cl_bool cl_queue_profiling_enable = false;
  cl_bool cl_device_image_support = false;
  cl_uint cl_device_max_read_image_args;
  cl_uint cl_device_max_write_image_args;
  cl_bool cl_fp_denorm = false;
  cl_bool cl_fp_inf_nan = false;
  cl_bool cl_fp_round_to_nearest = false;
  cl_bool cl_fp_round_to_zero = false;
  cl_bool cl_fp_round_to_inf = false;
  cl_bool cl_fp_fma = false;

  // CL_DEVICE_NAME
  clGetDeviceInfo (dev, CL_DEVICE_NAME, sizeof (cl_device_name),
		   &cl_device_name, NULL);
  print_log ("\n  CL_DEVICE_NAME: \t\t\t\t%s\n", cl_device_name);

  // CL_DEVICE_VENDOR
  clGetDeviceInfo (dev, CL_DEVICE_VENDOR, sizeof (cl_device_vendor),
		   &cl_device_vendor, NULL);
  print_log ("\tCL_DEVICE_VENDOR: \t\t\t%s\n", cl_device_vendor);

  // CL_DRIVER_VERSION
  clGetDeviceInfo (dev, CL_DRIVER_VERSION, sizeof (cl_driver_version),
		   &cl_driver_version, NULL);
  print_log ("\tCL_DRIVER_VERSION: \t\t\t%s\n", cl_driver_version);

  // CL_DEVICE_VERSION
  clGetDeviceInfo (dev, CL_DEVICE_VERSION, sizeof (cl_device_version),
		   &cl_device_version, NULL);
  print_log ("\tCL_DEVICE_VERSION: \t\t\t%s\n", cl_device_version);

  // CL_DEVICE_OPENCL_C_VERSION (if CL_DEVICE_VERSION version > 1.0)
  if (strncmp ("OpenCL 1.0", device_string, 10) != 0)
    {
      // This code is unused for devices reporting OpenCL 1.0, but a def is needed anyway to allow compilation using v 1.0 headers
      // This constant isn't #defined in 1.0
#ifndef CL_DEVICE_OPENCL_C_VERSION
#define CL_DEVICE_OPENCL_C_VERSION 0x103D
#endif

      clGetDeviceInfo (dev, CL_DEVICE_OPENCL_C_VERSION,
		       sizeof (device_string), &device_string, NULL);
      print_log ("\tCL_DEVICE_OPENCL_C_VERSION: \t\t%s\n", device_string);
    }

  // CL_DEVICE_TYPE
  cl_device_type type;
  clGetDeviceInfo (dev, CL_DEVICE_TYPE, sizeof (type), &type, NULL);
  if (type & CL_DEVICE_TYPE_CPU)
    {
      print_log ("\tCL_DEVICE_TYPE:\t\t\t\t%s\n", "CL_DEVICE_TYPE_CPU");
      sprintf (CL_device_type, "CPU");
    };
  if (type & CL_DEVICE_TYPE_GPU)
    {
      print_log ("\tCL_DEVICE_TYPE:\t\t\t\t%s\n", "CL_DEVICE_TYPE_GPU");
      sprintf (CL_device_type, "GPU");
    };
  if (type & CL_DEVICE_TYPE_ACCELERATOR)
    {
      print_log ("\tCL_DEVICE_TYPE:\t\t\t\t%s\n",
		 "CL_DEVICE_TYPE_ACCELERATOR");
      sprintf (CL_device_type, "ACCELERATOR");
    };
//  if (type & CL_DEVICE_TYPE_DEFAULT)
//    {
//      print_log ("\tCL_DEVICE_TYPE:\t\t\t\t%s\n", "CL_DEVICE_TYPE_DEFAULT");
//      sprintf (CL_device_type, "DEFAULT");
//    };

  // CL_DEVICE_MAX_COMPUTE_UNITS
  clGetDeviceInfo (dev, CL_DEVICE_MAX_COMPUTE_UNITS,
		   sizeof (cl_device_max_compute_units),
		   &cl_device_max_compute_units, NULL);
  print_log ("\tCL_DEVICE_MAX_COMPUTE_UNITS:\t\t%u\n",
	     cl_device_max_compute_units);

  // CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS
  clGetDeviceInfo (dev, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
		   sizeof (cl_device_max_work_item_dimensions),
		   &cl_device_max_work_item_dimensions, NULL);
  print_log ("\tCL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:\t%u\n",
	     (int) cl_device_max_work_item_dimensions);

  // CL_DEVICE_MAX_WORK_ITEM_SIZES
  size_t workitem_size[3];
  clGetDeviceInfo (dev, CL_DEVICE_MAX_WORK_ITEM_SIZES,
		   sizeof (workitem_size), &workitem_size, NULL);
  print_log ("\tCL_DEVICE_MAX_WORK_ITEM_SIZES:\t\t%u / %u / %u \n",
	     (int) workitem_size[0], (int) workitem_size[1],
	     (int) workitem_size[2]);
  sprintf (cl_device_max_work_item_sizes, "%u / %u / %u",
	   (cl_uint) workitem_size[0], (cl_uint) workitem_size[1],
	   (cl_uint) workitem_size[2]);

  // CL_DEVICE_MAX_WORK_GROUP_SIZE
  clGetDeviceInfo (dev, CL_DEVICE_MAX_WORK_GROUP_SIZE,
		   sizeof (cl_device_max_work_group_size),
		   &cl_device_max_work_group_size, NULL);
  print_log ("\tCL_DEVICE_MAX_WORK_GROUP_SIZE:\t\t%u\n",
	     (int) cl_device_max_work_group_size);

  // CL_DEVICE_MAX_CLOCK_FREQUENCY
  clGetDeviceInfo (dev, CL_DEVICE_MAX_CLOCK_FREQUENCY,
		   sizeof (cl_device_max_clock_frequency),
		   &cl_device_max_clock_frequency, NULL);
  print_log ("\tCL_DEVICE_MAX_CLOCK_FREQUENCY:\t\t%u MHz\n",
	     cl_device_max_clock_frequency);

  // CL_DEVICE_ADDRESS_BITS
  clGetDeviceInfo (dev, CL_DEVICE_ADDRESS_BITS,
		   sizeof (cl_device_address_bits), &cl_device_address_bits,
		   NULL);
  print_log ("\tCL_DEVICE_ADDRESS_BITS:\t\t\t%u\n", cl_device_address_bits);

  // CL_DEVICE_MAX_MEM_ALLOC_SIZE
  clGetDeviceInfo (dev, CL_DEVICE_MAX_MEM_ALLOC_SIZE,
		   sizeof (cl_device_max_mem_alloc_size),
		   &cl_device_max_mem_alloc_size, NULL);
  print_log ("\tCL_DEVICE_MAX_MEM_ALLOC_SIZE:\t\t%u MByte\n",
	     (uint) (cl_device_max_mem_alloc_size / (1024 * 1024)));
  cl_device_max_mem_alloc_size =
    (uint) (cl_device_max_mem_alloc_size / (1024 * 1024));

  // CL_DEVICE_GLOBAL_MEM_SIZE
  clGetDeviceInfo (dev, CL_DEVICE_GLOBAL_MEM_SIZE,
		   sizeof (cl_device_global_mem_size),
		   &cl_device_global_mem_size, NULL);
  print_log ("\tCL_DEVICE_GLOBAL_MEM_SIZE:\t\t%u MByte\n",
	     (uint) (cl_device_global_mem_size / (1024 * 1024)));
  cl_device_global_mem_size =
    (uint) (cl_device_global_mem_size / (1024 * 1024));

  clGetDeviceInfo (dev, CL_DEVICE_ERROR_CORRECTION_SUPPORT,
		   sizeof (cl_device_error_correction_support),
		   &cl_device_error_correction_support, NULL);
  print_log ("\tCL_DEVICE_ERROR_CORRECTION_SUPPORT:\t%s\n",
	     cl_device_error_correction_support == CL_TRUE ? "yes" : "no");

  // CL_DEVICE_LOCAL_MEM_TYPE
  cl_device_local_mem_type local_mem_type;
  clGetDeviceInfo (dev, CL_DEVICE_LOCAL_MEM_TYPE, sizeof (local_mem_type),
		   &local_mem_type, NULL);
  print_log ("\tCL_DEVICE_LOCAL_MEM_TYPE:\t\t%s\n",
	     local_mem_type == 1 ? "local" : "global");
  sprintf (CL_device_local_mem_type,
	   (local_mem_type == 1) ? "local" : "global");

  // CL_DEVICE_LOCAL_MEM_SIZE
  clGetDeviceInfo (dev, CL_DEVICE_LOCAL_MEM_SIZE,
		   sizeof (cl_device_local_mem_size),
		   &cl_device_local_mem_size, NULL);
  print_log ("\tCL_DEVICE_LOCAL_MEM_SIZE:\t\t%u KByte\n",
	     (uint) (cl_device_local_mem_size / 1024));
  cl_device_local_mem_size = (uint) (cl_device_local_mem_size / 1024);

  // CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE
  clGetDeviceInfo (dev, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,
		   sizeof (cl_device_max_constant_buffer_size),
		   &cl_device_max_constant_buffer_size, NULL);
  print_log ("\tCL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:\t%llu KByte\n",
	     (unsigned long long) (cl_device_max_constant_buffer_size /
				   1024));
  cl_device_max_constant_buffer_size =
    (unsigned long long) (cl_device_max_constant_buffer_size / 1024);

  // CL_DEVICE_QUEUE_PROPERTIES
  cl_command_queue_properties queue_properties;
  clGetDeviceInfo (dev, CL_DEVICE_QUEUE_PROPERTIES,
		   sizeof (queue_properties), &queue_properties, NULL);
  if (queue_properties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE)
    {
      print_log ("\tCL_DEVICE_QUEUE_PROPERTIES:\t\t%s\n",
		 "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE");
      cl_queue_out_of_order_exec_mode_enable = true;
    };
  if (queue_properties & CL_QUEUE_PROFILING_ENABLE)
    {
      print_log ("\tCL_DEVICE_QUEUE_PROPERTIES:\t\t%s\n",
		 "CL_QUEUE_PROFILING_ENABLE");
      cl_queue_profiling_enable = true;
    }

  // CL_DEVICE_IMAGE_SUPPORT
  clGetDeviceInfo (dev, CL_DEVICE_IMAGE_SUPPORT,
		   sizeof (cl_device_image_support),
		   &cl_device_image_support, NULL);
  print_log ("\tCL_DEVICE_IMAGE_SUPPORT:\t\t%u\n", cl_device_image_support);

  // CL_DEVICE_MAX_READ_IMAGE_ARGS
  clGetDeviceInfo (dev, CL_DEVICE_MAX_READ_IMAGE_ARGS,
		   sizeof (cl_device_max_read_image_args),
		   &cl_device_max_read_image_args, NULL);
  print_log ("\tCL_DEVICE_MAX_READ_IMAGE_ARGS:\t\t%u\n",
	     cl_device_max_read_image_args);

  // CL_DEVICE_MAX_WRITE_IMAGE_ARGS
  clGetDeviceInfo (dev, CL_DEVICE_MAX_WRITE_IMAGE_ARGS,
		   sizeof (cl_device_max_write_image_args),
		   &cl_device_max_write_image_args, NULL);
  print_log ("\tCL_DEVICE_MAX_WRITE_IMAGE_ARGS:\t\t%u\n",
	     cl_device_max_write_image_args);


  // CL_DEVICE_SINGLE_FP_CONFIG
  cl_device_fp_config fp_config;
  clGetDeviceInfo (dev, CL_DEVICE_SINGLE_FP_CONFIG,
		   sizeof (cl_device_fp_config), &fp_config, NULL);
  print_log ("\tCL_DEVICE_SINGLE_FP_CONFIG:\t\t%s%s%s%s%s%s\n",
	     fp_config & CL_FP_DENORM ? "denorms " : "",
	     fp_config & CL_FP_INF_NAN ? "INF-quietNaNs " : "",
	     fp_config & CL_FP_ROUND_TO_NEAREST ? "round-to-nearest " : "",
	     fp_config & CL_FP_ROUND_TO_ZERO ? "round-to-zero " : "",
	     fp_config & CL_FP_ROUND_TO_INF ? "round-to-inf " : "",
	     fp_config & CL_FP_FMA ? "fma " : "");

  if (fp_config & CL_FP_DENORM)
    {
      cl_fp_denorm = true;
    };
  if (fp_config & CL_FP_INF_NAN)
    {
      cl_fp_inf_nan = true;
    };
  if (fp_config & CL_FP_ROUND_TO_NEAREST)
    {
      cl_fp_round_to_nearest = true;
    };
  if (fp_config & CL_FP_ROUND_TO_ZERO)
    {
      cl_fp_round_to_zero = true;
    };
  if (fp_config & CL_FP_ROUND_TO_INF)
    {
      cl_fp_round_to_inf = true;
    };
  if (fp_config & CL_FP_FMA)
    {
      cl_fp_fma = true;
    };

      //Store into SQLite3 database Platform
      sprintf (Dbstr, "INSERT INTO Device VALUES (\
                        '%d',\
                        '%d',\
                        '%s',\
                        '%s',\
                        '%s',\
                        '%s',\
                        '%s',\
                        '%d',\
                        '%d',\
                        '%s',\
                        '%d',\
                        '%d',\
                        '%d',\
                        '%lu',\
                        '%lu',\
                        '%d',\
                        '%s',\
                        '%lu',\
                        '%llu',\
                        '%d',\
                        '%d',\
                        '%d',\
                        '%d',\
                        '%d',\
                        '%d',\
                        '%d',\
                        '%d',\
                        '%d',\
                        '%d',\
                        '%d');", DeviceId, PlatformId, cl_device_name, cl_device_vendor, cl_driver_version, cl_device_version, CL_device_type, cl_device_max_compute_units, cl_device_max_work_item_dimensions, cl_device_max_work_item_sizes, cl_device_max_work_group_size, cl_device_max_clock_frequency, cl_device_address_bits, cl_device_max_mem_alloc_size, cl_device_global_mem_size, cl_device_error_correction_support, CL_device_local_mem_type, cl_device_local_mem_size, cl_device_max_constant_buffer_size, cl_queue_out_of_order_exec_mode_enable, cl_queue_profiling_enable, cl_device_image_support, cl_device_max_read_image_args, cl_device_max_write_image_args, cl_fp_denorm, cl_fp_inf_nan, cl_fp_round_to_nearest, cl_fp_round_to_zero, cl_fp_round_to_inf, cl_fp_fma);

      DbErr =
	sqlite3_exec (acc_profiler->db_file, Dbstr, NULL, 0, &DbErrMsg);
      assert (DbErr == SQLITE_OK);
}

