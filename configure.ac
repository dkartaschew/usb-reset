#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([usbreset], [1.0.0], [darran.kartaschew@outlook.com])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_AUX_DIR([config])
AC_CANONICAL_HOST

AC_SUBST(ARCH,[host_cpu])

AM_INIT_AUTOMAKE([1.10 no-define foreign])
# Checks for programs.
AC_PROG_CC([gcc cc])
AC_PROG_CC_C99
AC_PROG_INSTALL
AC_PROG_LN_S

AC_CHECK_HEADERS([stdlib.h stdio.h string.h unistd.h stdbool.h ctype.h inttypes.h])

# Stick in "-Werror" if you want to be more aggressive.
# (No need to use AC_SUBST on this default substituted environment variable.)
# Only add these additional CFLAGS if we are using GCC. Other C compilers may
# not support them.
if test x"$GCC" == "xyes" ; then
    AX_CHECK_COMPILE_FLAG([ -Wall -Wextra -Wpedantic ], 
        [CFLAGS="$CFLAGS -Wall -Wextra -Wpedantic "])
fi

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Some common needed definitions.
AC_DEFINE([_STDC_C99], [], [Ensure libraries are C99])
AC_DEFINE([_POSIX_C_SOURCE], [200112L], [Set POSIX Level])
AC_DEFINE([_XOPEN_SOURCE], [600], [Set XOPEN Source Level])

# Common libraries
PKG_CHECK_MODULES([LIBUSB], [libusb-1.0 >= 1.0.0])

AC_CONFIG_FILES([
Makefile 
src/Makefile 
doc/Makefile])
AC_OUTPUT
