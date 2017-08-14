#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([sold], [0.4], [neolynx@gmail.com])
AC_CONFIG_SRCDIR([sold.c])

# Checks for programs.
AC_PROG_CC
AM_INIT_AUTOMAKE

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([bzero socket])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
