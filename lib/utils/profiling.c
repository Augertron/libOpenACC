
#include "OpenACC/utils/profiling.h"
#include "OpenACC/private/runtime.h"
#include "OpenACC/private/debug.h"

#include "sqlite3.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <assert.h>

#undef ENABLE_LOGGING

sqlite3 *profiling_db_file;
char *profiling_event_table_name;

int
acc_profiling_database_callback (void *NotUsed, int argc, char **argv, char **azColName)
{
  int i;
  for (i = 0; i < argc; i++)
    {
      printf ("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
  printf ("\n");
  return 0;
}

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

void
acc_profiling_init ()
{
  int DbErr;
  char *DbErrMsg;
  char Dbstr[8192];

  //Step1: Give a name to profiling_db_file and using it to create a database file
  char db_file_name[128];
  static char event_table_name[128];
  char hostname[64];
  char username[64];

  char year[5];
  char month[3];
  char day[3];
  char hour[3];
  char minute[3];
  char second[3];

  time_t rawtime;
  struct tm *info;
  time (&rawtime);
  info = localtime (&rawtime);

  char randnum_str[5];
  srand (time (NULL));
  int randnum = rand () % 9999;
  sprintf (randnum_str, "%04d", randnum);

  gethostname (hostname, 64);
  getlogin_r (username, 64);

  sprintf (year, "%4d", info->tm_year + 1900);
  sprintf (month, "%02d", info->tm_mon + 1);
  sprintf (day, "%02d", info->tm_mday);
  sprintf (hour, "%02d", info->tm_hour);
  sprintf (minute, "%02d", info->tm_min);
  sprintf (second, "%02d", info->tm_sec);
  
  strcpy (db_file_name, username);
  strcat (db_file_name, "_");
  strcat (db_file_name, hostname);
  strcat (db_file_name, ".sl3");

  strcpy (event_table_name, "Event");	//5
  strcat (event_table_name, year);	//4
  strcat (event_table_name, month);	//2
  strcat (event_table_name, day);	//2
  strcat (event_table_name, hour);	//2
  strcat (event_table_name, minute);	//2
  strcat (event_table_name, second);	//2
  strcat (event_table_name, randnum_str);	//4
  event_table_name[23] = '\0';

  profiling_event_table_name = &event_table_name[0];

  printf ("profiling_db_file: %s\n", db_file_name);
  printf ("profiling_event_table_name:_%s_\n", profiling_event_table_name);

  bool newly_created_db_file = true;
  FILE *file;
  if (file = fopen (db_file_name, "r"))
    {
      newly_created_db_file = false;
    }

  DbErr = sqlite3_open (db_file_name, &profiling_db_file);
  assert (DbErr == SQLITE_OK);

  //Step2: Create Platform, Device tables if database file is newly created
  if (newly_created_db_file)
    {
      sprintf (Dbstr, "CREATE TABLE Platform(  \
             ID INT, \
             CL_PLATFORM_NAME CHAR(100), \
             CL_PLATFORM_VERSION CHAR(100), \
             CL_DEVICE_TYPE_ALL INT \
             );");

      DbErr =
	sqlite3_exec (profiling_db_file, Dbstr, acc_profiling_database_callback, 0, &DbErrMsg);
      assert (DbErr == SQLITE_OK);

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

      DbErr =
	sqlite3_exec (profiling_db_file, Dbstr, acc_profiling_database_callback, 0, &DbErrMsg);
      assert (DbErr == SQLITE_OK);

      //Step3: fill out Platform and Device tables
      acc_profiling_platform_init ();
    }				//r \todo [profiling] init_profiling()

      //Step3: Create Event tables in the database file. 
      sprintf (Dbstr, "CREATE TABLE '%s'(  \
             DEVICE_ID INT, \
             COMMAND_NAME CHAR(128), \
             CL_PROFILING_COMMAND_QUEUED REAL, \
             CL_PROFILING_COMMAND_SUBMIT REAL, \
             CL_PROFILING_COMMAND_START  REAL, \
             CL_PROFILING_COMMAND_END    REAL  \
             );", profiling_event_table_name);

      DbErr =
	sqlite3_exec (profiling_db_file, Dbstr, acc_profiling_database_callback, 0, &DbErrMsg);
      assert (DbErr == SQLITE_OK);

}

void
acc_profiling_exit ()
{
  assert (profiling_db_file != NULL);
  /// \todo [profiling] exit_profiling()
}

void
acc_profiling_ocl_event_callback (cl_event event,
				  cl_int event_command_exec_status,
				  void *user_data)
{
  struct acc_profiling_event_data_t_ *event_data =
    (struct acc_profiling_event_data_t_ *) user_data;

  cl_ulong queued, submit, start, end;

  cl_int status = clGetEventProfilingInfo (event, CL_PROFILING_COMMAND_QUEUED,
					   sizeof (cl_ulong), &queued,
					   NULL);
  if (status != CL_SUCCESS)
    {
      const char *status_str = acc_ocl_status_to_char (status);
      printf ("[fatal]   clGetEventProfilingInfo return %s.\n", status_str);
      exit (-1);		/// \todo error code
    }

  status =
    clGetEventProfilingInfo (event, CL_PROFILING_COMMAND_SUBMIT,
			     sizeof (cl_ulong), &submit, NULL);
  if (status != CL_SUCCESS)
    {
      const char *status_str = acc_ocl_status_to_char (status);
      printf ("[fatal]   clGetEventProfilingInfo return %s.\n", status_str);
      exit (-1);		/// \todo error code
    }

  status =
    clGetEventProfilingInfo (event, CL_PROFILING_COMMAND_START,
			     sizeof (cl_ulong), &start, NULL);
  if (status != CL_SUCCESS)
    {
      const char *status_str = acc_ocl_status_to_char (status);
      printf ("[fatal]   clGetEventProfilingInfo return %s.\n", status_str);
      exit (-1);		/// \todo error code
    }

  status =
    clGetEventProfilingInfo (event, CL_PROFILING_COMMAND_END,
			     sizeof (cl_ulong), &end, NULL);
  if (status != CL_SUCCESS)
    {
      const char *status_str = acc_ocl_status_to_char (status);
      printf ("[fatal]   clGetEventProfilingInfo return %s.\n", status_str);
      exit (-1);		/// \todo error code
    }

  char *command_name;
  switch (event_data->kind)
    {
    case e_acc_memcpy_to_device:
      command_name = "acc_memcpy_to_device";
      break;
    case e_acc_memcpy_from_device:
      command_name = "acc_memcpy_from_device";
      break;
    case e_acc_kernel_launch:
      command_name = "acc_kernel_launch";
      break;
    }

  char Dbstr[8192];
  sprintf (Dbstr,
	   "INSERT INTO '%s' VALUES ( '%d', '%s', '%.3f', '%.3f', '%.3f', '%.3f');",
	   profiling_event_table_name, event_data->device_idx, command_name,
	   queued * 1.0e-3, submit * 1.0e-3, start * 1.0e-3, end * 1.0e-3);

  char *DbErrMsg;
  int DbErr =
    sqlite3_exec (profiling_db_file, Dbstr, acc_profiling_database_callback, 0, &DbErrMsg);
  assert (DbErr == SQLITE_OK);

  free (event_data);
}

// Prints a string version of the specified OpenCL error code
void
acc_profiling_ocl_error (cl_int error, int line_no)
{
  printf ("Fatal at line %d\t%s\n", line_no, acc_ocl_status_to_char (error));
  exit (error);
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
	      sqlite3_exec (profiling_db_file, Dbstr, acc_profiling_database_callback, 0,
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
	sqlite3_exec (profiling_db_file, Dbstr, acc_profiling_database_callback, 0, &DbErrMsg);
      assert (DbErr == SQLITE_OK);
}
