
#ifndef __LIBOPENACC_PROFILING_H__
#define __LIBOPENACC_PROFILING_H__

#include "sqlite3.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <CL/cl.h>
#include <stdbool.h>

extern sqlite3 * profiling_db_file;

void init_profiling();

void exit_profiling();

void print_warning (const char *input_string);
void print_log (const char *fmt, ...);
void print_error (const char *file, uint line, const char *input_string);

int Dbcallback (void *NotUsed, int argc, char **argv, char **azColName);

void PrintDeviceInfo (cl_device_id dev, cl_uint DeviceId, cl_uint PlatformId,
                 	bool DbExist);
void CledInsertIntoEvent (cl_command_queue command_queue, const char *FuncName);
void fatal_CL (cl_int error, int line_no);
void DeviceQuery (void);

cl_int GetDeviceIdFromCmdQueue (cl_command_queue command_queue);
/// \todo [profiling] add profiling function for kernel and data transfert

#endif /* __LIBOPENACC_PROFILING_H__ */

