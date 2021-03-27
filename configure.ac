## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Bulwark],[0.3],[sean.tyler.berryhill@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_TESTDIR([src/tests])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_AR

# Init libtool since we want to build shared lib
LT_INIT([dlopen shared])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h sys/ioctl.h termios.h])

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit])

# Conditionals
AM_CONDITIONAL([BUILD_EXAMPLES], [test "$BUILD_EXAMPLES" = yes])

# Runs config.status in these makefiles to regnerate them from their templates if there are any changes.
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile
                 examples/Makefile])
AC_OUTPUT
