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
AC_PROG_MAKE_SET

# Checks for libraries.

AC_ARG_WITH(opencl,        [  --with-opencl            OpenCL path])
if test ! -z "$with_opencl"
then
	AC_MSG_NOTICE([OpenCL path provided])
        AC_SUBST(opencl_path, "$with_opencl")
else
  AC_MSG_ERROR([Missing OpenCL headers path.])
fi
# TODO test OpenCL

AC_ARG_WITH(sqlite,        [  --with-sqlite            SQLite path])
if test ! -z "$with_sqlite"
then
	AC_MSG_NOTICE([SQLite path provided])
        AC_SUBST(sqlite_path, "$with_sqlite")
else
  AC_MSG_ERROR([Missing SQLite headers path.])
fi
# TODO test SQLite

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

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 include/OpenACC/Makefile
                 include/OpenACC/device/Makefile
                 include/OpenACC/internal/Makefile
                 include/OpenACC/private/Makefile
                 include/OpenACC/public/Makefile
                 include/OpenACC/utils/Makefile
                 include/OpenACC/utils/containers/Makefile
                 lib/Makefile])
AC_OUTPUT
