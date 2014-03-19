
#include "OpenACC/utils/profiling.h"
#include "OpenACC/private/runtime.h"
#include <assert.h>
//#define undef

#undef ENABLE_WARNINGS
#undef ENABLE_LOGGING




sqlite3 * profiling_db_file;
char * profiling_db_file_name;
int DbErr;
char *DbErrMsg;
char Dbstr[8192];

cl_event cxEvents[32];
int EventIdx;
int NumEventReleased;


void init_profiling() {
  //Init some parameters
  EventIdx = -1;
  NumEventReleased = 0;


  //Step1: Give a name to profiling_db_file and using it to create a database file
  char db_file_name[256];
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
  time( &rawtime );
  info = localtime( &rawtime );

  gethostname (hostname, 64);
  getlogin_r (username, 64);

  sprintf(year, "%4d",info->tm_year+1900);
  sprintf(month, "%02d",info->tm_mon+1);
  sprintf(day, "%02d",info->tm_mday);
  sprintf(hour, "%02d",info->tm_hour);
  sprintf(minute, "%02d",info->tm_min);
  sprintf(second, "%02d",info->tm_sec);

  strcpy (db_file_name, username);
  strcat (db_file_name, "@");
  strcat (db_file_name, hostname);
  strcat (db_file_name, "YYYYMMDDHHMMSS");
  strcat (db_file_name, year);
  strcat (db_file_name, month);
  strcat (db_file_name, day);
  strcat (db_file_name, hour);
  strcat (db_file_name, minute);
  strcat (db_file_name, second);
  strcat (db_file_name, ".sl3");  

  profiling_db_file_name = &db_file_name[0];
  print_log("profiling_db_file: %s\n",profiling_db_file_name);


  FILE *file;
  if (file = fopen (profiling_db_file_name, "r"))
    {   
      if (remove (profiling_db_file_name) != 0)
        print_error (__FILE__,__LINE__,"Can not delete file");
      else
        print_log ("File successfully deleted\n");
    }


  DbErr = sqlite3_open (profiling_db_file_name, &profiling_db_file);
  if (DbErr)
    {   
      print_error (__FILE__,__LINE__,"Can not open database");
    }   
  else
    {   
      print_log ("Opened database successfully\n");
    }


  //Step2: Create Platform, Device and Event tables in the database file. 
  sprintf (Dbstr, "CREATE TABLE Platform(  \
             ID INT, \
             CL_PLATFORM_NAME CHAR(100), \
             CL_PLATFORM_VERSION CHAR(100), \
             CL_DEVICE_TYPE_ALL INT \
             );");

  DbErr = sqlite3_exec (profiling_db_file, Dbstr, Dbcallback, 0, &DbErrMsg);
  if (DbErr != SQLITE_OK)
    {
      print_error (__FILE__,__LINE__,DbErrMsg);
      sqlite3_free (DbErrMsg);
    }
  else
    {
      print_log ("Table Platform created successfully\n");
    }

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

  DbErr = sqlite3_exec (profiling_db_file, Dbstr, Dbcallback, 0, &DbErrMsg);
  if (DbErr != SQLITE_OK)
    {
      print_error (__FILE__,__LINE__,DbErrMsg);
      sqlite3_free (DbErrMsg);
    }
  else
    {
      print_log ("Table Device created successfully\n");
    }

  sprintf (Dbstr, "CREATE TABLE Event(  \
             ID INT, \
             DEVICEID INT, \
             FUNCTIONNAME CHAR(128), \
             TIME REAL \
             );");

  DbErr = sqlite3_exec (profiling_db_file, Dbstr, Dbcallback, 0, &DbErrMsg);
  if (DbErr != SQLITE_OK)
    {
      print_error (__FILE__,__LINE__,DbErrMsg);
      sqlite3_free (DbErrMsg);
    }
  else
    {
      print_log ("Table Event created successfully\n");
    }

  //Step3: fill out Platform and Device tables
  DeviceQuery();

  sprintf (Dbstr, "INSERT INTO Event VALUES (\
                            '%d',\
                            '%d',\
                            '%s',\
                            '%.3f');",\
                                1,\
                                2,\
                                "Testing",\
                                1.23456);

  DbErr = sqlite3_exec (profiling_db_file, Dbstr, Dbcallback, 0, &DbErrMsg);
  if (DbErr != SQLITE_OK)
    {   
      print_error (__FILE__,__LINE__,DbErrMsg);
      sqlite3_free (DbErrMsg);
      print_error (__FILE__,__LINE__,"SQL error\n");
    }


}//r \todo [profiling] init_profiling()

void exit_profiling() {
  //assert(profiling_db_file != NULL);

  /// \todo [profiling] exit_profiling()
}

/// \todo [profiling] add profiling function for kernel and data transfert



// Prints a string version of the specified OpenCL error code
void
fatal_CL (cl_int error, int line_no)
{
  printf ("Error at line %d: ", line_no);

  // Print 
  switch (error)
    {
    case CL_SUCCESS:
      printf ("CL_SUCCESS\n");
      break;
    case CL_DEVICE_NOT_FOUND:
      printf ("CL_DEVICE_NOT_FOUND\n");
      break;
    case CL_DEVICE_NOT_AVAILABLE:
      printf ("CL_DEVICE_NOT_AVAILABLE\n");
      break;
    case CL_COMPILER_NOT_AVAILABLE:
      printf ("CL_COMPILER_NOT_AVAILABLE\n");
      break;
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
      printf ("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
      break;
    case CL_OUT_OF_RESOURCES:
      printf ("CL_OUT_OF_RESOURCES\n");
      break;
    case CL_OUT_OF_HOST_MEMORY:
      printf ("CL_OUT_OF_HOST_MEMORY\n");
      break;
    case CL_PROFILING_INFO_NOT_AVAILABLE:
      printf ("CL_PROFILING_INFO_NOT_AVAILABLE\n");
      break;
    case CL_MEM_COPY_OVERLAP:
      printf ("CL_MEM_COPY_OVERLAP\n");
      break;
    case CL_IMAGE_FORMAT_MISMATCH:
      printf ("CL_IMAGE_FORMAT_MISMATCH\n");
      break;
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
      printf ("CL_IMAGE_FORMAT_NOT_SUPPORTED\n");
      break;
    case CL_BUILD_PROGRAM_FAILURE:
      printf ("CL_BUILD_PROGRAM_FAILURE\n");
      break;
    case CL_MAP_FAILURE:
      printf ("CL_MAP_FAILURE\n");
      break;
    case CL_INVALID_VALUE:
      printf ("CL_INVALID_VALUE\n");
      break;
    case CL_INVALID_DEVICE_TYPE:
      printf ("CL_INVALID_DEVICE_TYPE\n");
      break;
    case CL_INVALID_PLATFORM:
      printf ("CL_INVALID_PLATFORM\n");
      break;
    case CL_INVALID_DEVICE:
      printf ("CL_INVALID_DEVICE\n");
      break;
    case CL_INVALID_CONTEXT:
      printf ("CL_INVALID_CONTEXT\n");
      break;
    case CL_INVALID_QUEUE_PROPERTIES:
      printf ("CL_INVALID_QUEUE_PROPERTIES\n");
      break;
    case CL_INVALID_COMMAND_QUEUE:
      printf ("CL_INVALID_COMMAND_QUEUE\n");
      break;
    case CL_INVALID_HOST_PTR:
      printf ("CL_INVALID_HOST_PTR\n");
      break;
    case CL_INVALID_MEM_OBJECT:
      printf ("CL_INVALID_MEM_OBJECT\n");
      break;
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
      printf ("CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
      break;
    case CL_INVALID_IMAGE_SIZE:
      printf ("CL_INVALID_IMAGE_SIZE\n");
      break;
    case CL_INVALID_SAMPLER:
      printf ("CL_INVALID_SAMPLER\n");
      break;
    case CL_INVALID_BINARY:
      printf ("CL_INVALID_BINARY\n");
      break;
    case CL_INVALID_BUILD_OPTIONS:
      printf ("CL_INVALID_BUILD_OPTIONS\n");
      break;
    case CL_INVALID_PROGRAM:
      printf ("CL_INVALID_PROGRAM\n");
      break;
    case CL_INVALID_PROGRAM_EXECUTABLE:
      printf ("CL_INVALID_PROGRAM_EXECUTABLE\n");
      break;
    case CL_INVALID_KERNEL_NAME:
      printf ("CL_INVALID_KERNEL_NAME\n");
      break;
    case CL_INVALID_KERNEL_DEFINITION:
      printf ("CL_INVALID_KERNEL_DEFINITION\n");
      break;
    case CL_INVALID_KERNEL:
      printf ("CL_INVALID_KERNEL\n");
      break;
    case CL_INVALID_ARG_INDEX:
      printf ("CL_INVALID_ARG_INDEX\n");
      break;
    case CL_INVALID_ARG_VALUE:
      printf ("CL_INVALID_ARG_VALUE\n");
      break;
    case CL_INVALID_ARG_SIZE:
      printf ("CL_INVALID_ARG_SIZE\n");
      break;
    case CL_INVALID_KERNEL_ARGS:
      printf ("CL_INVALID_KERNEL_ARGS\n");
      break;
    case CL_INVALID_WORK_DIMENSION:
      printf ("CL_INVALID_WORK_DIMENSION\n");
      break;
    case CL_INVALID_WORK_GROUP_SIZE:
      printf ("CL_INVALID_WORK_GROUP_SIZE\n");
      break;
    case CL_INVALID_WORK_ITEM_SIZE:
      printf ("CL_INVALID_WORK_ITEM_SIZE\n");
      break;
    case CL_INVALID_GLOBAL_OFFSET:
      printf ("CL_INVALID_GLOBAL_OFFSET\n");
      break;
    case CL_INVALID_EVENT_WAIT_LIST:
      printf ("CL_INVALID_EVENT_WAIT_LIST\n");
      break;
    case CL_INVALID_EVENT:
      printf ("CL_INVALID_EVENT\n");
      break;
    case CL_INVALID_OPERATION:
      printf ("CL_INVALID_OPERATION\n");
      break;
    case CL_INVALID_GL_OBJECT:
      printf ("CL_INVALID_GL_OBJECT\n");
      break;
    case CL_INVALID_BUFFER_SIZE:
      printf ("CL_INVALID_BUFFER_SIZE\n");
      break;
    case CL_INVALID_MIP_LEVEL:
      printf ("CL_INVALID_MIP_LEVEL\n");
      break;
    case CL_INVALID_GLOBAL_WORK_SIZE:
      printf ("CL_INVALID_GLOBAL_WORK_SIZE\n");
      break;

#ifdef CL_VERSION_1_1
    case CL_MISALIGNED_SUB_BUFFER_OFFSET:
      printf ("CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
      break;
    case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
      printf ("CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST\n");
      break;
    case CL_INVALID_PROPERTY:
      printf ("CL_INVALID_PROPERTY\n");
      break;
#endif

    default:
      printf ("Invalid OpenCL error code\n");
    }

  exit (error);
}

void
DeviceQuery (void)
{
  int DbErr;
  char *DbErrMsg;
  char Dbstr[8192];
  bool DbExist = false;

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
    fatal_CL (ciErrNum, __LINE__);

  // Get the list of platforms
  cl_platform_id *platforms =
    (cl_platform_id *) malloc (sizeof (cl_platform_id) * num_platforms);
  ciErrNum = clGetPlatformIDs (num_platforms, platforms, NULL);
  if (ciErrNum != CL_SUCCESS)
    fatal_CL (ciErrNum, __LINE__);

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
	clGetPlatformInfo (platform, CL_PLATFORM_NAME, sizeof (PlatformName),
			   PlatformName, NULL);
      if (ciErrNum != CL_SUCCESS)
	fatal_CL (ciErrNum, __LINE__);
      print_log (" CL_PLATFORM_NAME: \t%s\n", PlatformName);

      ciErrNum =
	clGetPlatformInfo (platform, CL_PLATFORM_VERSION,
			   sizeof (PlatformVersion), PlatformVersion, NULL);
      if (ciErrNum != CL_SUCCESS)
	fatal_CL (ciErrNum, __LINE__);
      print_log (" CL_PLATFORM_VERSION: \t%s\n", PlatformVersion);

      cl_uint ciDeviceCount;
      cl_device_id *devices;
      print_log ("OpenCL Device Info:\n");
      ciErrNum =
	clGetDeviceIDs (platform, CL_DEVICE_TYPE_ALL, 0, NULL,
			&ciDeviceCount);

      // check for 0 devices found or errors...

      if (ciDeviceCount == 0)
	{
	  print_error (__FILE__, __LINE__,
		       "No devices found supporting OpenCL");
	}
      else if (ciErrNum != CL_SUCCESS)
	{
	  print_error (__FILE__, __LINE__, "clGetDeviceIDs call");
	}
      else
	{
	  if (DbExist == false)
	    {
	      //Store into SQLite3 database Platform
	      sprintf (Dbstr, "INSERT INTO Platform VALUES (\
                            '%d',\
                            '%s',\
                            '%s',\
                            '%d');", i, PlatformName, PlatformVersion, ciDeviceCount);
	      DbErr =
		sqlite3_exec (profiling_db_file, Dbstr, Dbcallback, 0,
			      &DbErrMsg);
	      if (DbErr != SQLITE_OK)
		{
		  print_error (__FILE__, __LINE__, DbErrMsg);
		  sqlite3_free (DbErrMsg);
		}
	      else
		{
		  print_log ("Table Platform inserted successfully\n");
		}
	    }
	  // Get and log the OpenCL device ID's
	  print_log (" %u devices found supporting OpenCL:\n", ciDeviceCount);
	  if ((devices =
	       (cl_device_id *) malloc (sizeof (cl_device_id) *
					ciDeviceCount)) == NULL)
	    {
	      print_error (__FILE__, __LINE__,
			   "Failed to allocate memory for devices");
	    }
	  ciErrNum =
	    clGetDeviceIDs (platform, CL_DEVICE_TYPE_ALL, ciDeviceCount,
			    devices, &ciDeviceCount);
	  if (ciErrNum == CL_SUCCESS)
	    {
	      //Create a context for the devices
	      cl_context cxGPUContext =
		clCreateContext (0, ciDeviceCount, devices, NULL, NULL,
				 &ciErrNum);
	      if (ciErrNum != CL_SUCCESS)
		{
		  print_error (__FILE__, __LINE__, "clCreateContext");
		}
	      else
		{
		  // show info for each device in the context
		  uint i;
		  for (i = 0; i < ciDeviceCount; ++i)
		    {
		      PrintDeviceInfo
			(devices[i], GlobalDeviceId, PlatformId, DbExist);
		      GlobalDeviceId++;
		    }
		}
	    }
	}
      print_log ("\n\n");
    }
  //----------------------Platform check---------------------------
}


void
PrintDeviceInfo (cl_device_id dev, cl_uint DeviceId, cl_uint PlatformId,
		 bool DbExist)
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
  clGetDeviceInfo (dev, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof (workitem_size),
		   &workitem_size, NULL);
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
  clGetDeviceInfo (dev, CL_DEVICE_QUEUE_PROPERTIES, sizeof (queue_properties),
		   &queue_properties, NULL);
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
		   sizeof (cl_device_image_support), &cl_device_image_support,
		   NULL);
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

  if (DbExist == false)
    {
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
	sqlite3_exec (profiling_db_file, Dbstr, Dbcallback, 0, &DbErrMsg);
      if (DbErr != SQLITE_OK)
	{
	  print_error (__FILE__, __LINE__, DbErrMsg);
	  sqlite3_free (DbErrMsg);
	}
      else
	{
	  print_log ("Table Device inserted successfully\n");
	}
    }				//if(DbExist==false)
}




  void print_warning (const char *input_string)
  {
#ifdef ENABLE_WARNINGS
    printf ("WARNING: %s line %d: %s\n", __FILE__, __LINE__, input_string);
#endif
  }

  void print_log (const char *fmt, ...)
  {
#ifdef ENABLE_LOGGING
    va_list args;
    va_start (args, fmt);
    vprintf (fmt, args);
    va_end (args);
#endif
  }

  void print_error (const char *file, uint line, const char *input_string)
  {
    fprintf (stderr, "Error: %s line %d: %s\n", file, line,
             input_string);
    exit (-1);
  }

int
Dbcallback (void *NotUsed, int argc, char **argv, char **azColName)
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
CledInsertIntoEvent (cl_command_queue command_queue,
                     const char *FuncName)
{
  int DbErr;
  char *DbErrMsg;
  char Dbstr[8192];
  sprintf (Dbstr, "INSERT INTO Event VALUES (\
                            '%d',\
                            '%d',\
                            '%s',\
                            '%.3f');",\
                                EventIdx,\
                                GetDeviceIdFromCmdQueue (command_queue),\
                                FuncName,\
                                0.0);

  DbErr = sqlite3_exec (profiling_db_file, Dbstr, Dbcallback, 0, &DbErrMsg);
  if (DbErr != SQLITE_OK)
    {   
      print_error (__FILE__,__LINE__,DbErrMsg);
      sqlite3_free (DbErrMsg);
      print_error (__FILE__,__LINE__,"SQL error\n");
    }   


  print_log\
    ("Event %d,\tDevice %d,\tFunction %s,\tTime %lld\n",
     EventIdx, GetDeviceIdFromCmdQueue (command_queue), FuncName, 0); 
}


cl_int
GetDeviceIdFromCmdQueue (cl_command_queue command_queue)
{
  int Id = -1; 
  int i = 0;
  int num_platforms = acc_runtime.opencl_data->num_platforms;
  int num_devices = acc_runtime.opencl_data->num_devices[num_platforms];
    
  for (i; i < num_devices; i++)
    {   
      if (acc_runtime.opencl_data->devices_data[i]->command_queue == command_queue)
        {
          Id = i;
          break;
        }
    }   
  if (Id != -1) 
    {   
      return Id; 
    }   
  else
    {   
      print_error (__FILE__,__LINE__,"GetDeviceIdFromCmdQueue error\n");
    } 
}
