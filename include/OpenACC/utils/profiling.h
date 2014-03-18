
#ifndef __LIBOPENACC_PROFILING_H__
#define __LIBOPENACC_PROFILING_H__

#include "sqlite3.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

extern sqlite3 * profiling_db_file;

void init_profiling();

void exit_profiling();

/// \todo [profiling] add profiling function for kernel and data transfert

#endif /* __LIBOPENACC_PROFILING_H__ */

