## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Bulwark],[0.2],[sean.tyler.berryhill@gmail.com])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_TESTDIR([src/tests])

#Initialize automake
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_AR

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h sys/ioctl.h termios.h])

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit])

# Runs config.status in these makefiles to regnerate them from their templates if there are any changes.
AC_CONFIG_FILES([Makefile
                 examples/Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT

# TODO:
#
# AM_CONDITIONAL(OS_IS_LINUX, test [ "$OSTYPE" == "linux_gnu"* ])
# AM_CONDITIONAL(OS_IS_MACOS, test [ "$OSTYPE" = "darwin"* ])
# AM_CONDITIONAL(OS_IS_WINDOWS, test [ "$OSTYPE" == "msys" ] || [ "$OSTYPE" == "cygwin" ])

# LT_INIT([dlopen shared])