#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT([libOpenACC], [0.1], [vanderbruggentristan@gmail.com])

AM_INIT_AUTOMAKE([libOpenACC], [0.1])

LT_INIT()

AC_CONFIG_SRCDIR([include/OpenACC/openacc.h])

#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_MAKE_SET

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime gethostname memmove strstr])

#####
# OpenCL
#####

AC_ARG_WITH(opencl-include, [  --with-opencl-include    OpenCL include path])
AC_ARG_WITH(opencl-libdir,  [  --with-opencl-libdir     OpenCL library path])

if test ! -z "$with_opencl_include"
then
	AC_MSG_NOTICE([OpenCL include path provided])
        AC_SUBST(opencl_inc_path, $with_opencl_include)
else
  AC_MSG_ERROR([Missing OpenCL include path.])
fi
if test ! -z "$with_opencl_libdir"
then
	AC_MSG_NOTICE([OpenCL library path provided])
        AC_SUBST(opencl_lib_path, $with_opencl_libdir)
else
  AC_MSG_ERROR([Missing OpenCL library path.])
fi
# TODO test OpenCL

#####
# SQLite
#####

AC_ARG_WITH(sqlite-include, [  --with-sqlite-include    SQLite include path])
AC_ARG_WITH(sqlite-libdir,  [  --with-sqlite-libdir     SQLite library path])

if test ! -z "$with_sqlite_include"
then
	AC_MSG_NOTICE([SQLite include path provided])
        AC_SUBST(sqlite_inc_path, $with_sqlite_include)
else
  AC_MSG_ERROR([Missing SQLite include path.])
fi
if test ! -z "$with_sqlite_libdir"
then
	AC_MSG_NOTICE([SQLite library path provided])
        AC_SUBST(sqlite_lib_path, $with_sqlite_libdir)
else
  AC_MSG_ERROR([Missing SQLite library path.])
fi
# TODO test SQLite

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 include/OpenACC/Makefile
                 include/OpenACC/device/Makefile
                 include/OpenACC/internal/Makefile
                 include/OpenACC/private/Makefile
                 include/OpenACC/public/Makefile
                 include/OpenACC/utils/Makefile
                 include/OpenACC/utils/containers/Makefile
                 include/OpenACC/utils/sqlite/Makefile
                 lib/Makefile
                 src/Makefile])
AC_OUTPUT
