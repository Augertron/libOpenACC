
#ifndef __LIBOPENACC_PROFILING_H__
#define __LIBOPENACC_PROFILING_H__

#include "sqlite3.h"

extern char * profiling_db_file;

void init_profiling();

void exit_profiling();

/// \todo [profiling] add profiling function for kernel and data transfert

#endif /* __LIBOPENACC_PROFILING_H__ */

