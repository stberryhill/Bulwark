TESTS = \
  unittests

check_PROGRAMS = $(TESTS)

AM_CFLAGS = \
  -Wall -Werror \
  -I$(top_srcdir)/include \
  -I$(abs_top_builddir)/include

AM_LDFLAGS = \
  -L$(top_buidldir)/src/ \
  ../src/libbulwark.la \
  -W

## Recommended to have as few of the sources here as possible.
unittests_SOURCES = \
  ImmediateFunctionTests.c

AUTOMAKE_OPTIONS = subdir-objects
