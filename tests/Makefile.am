TESTS = \
  unittests

check_PROGRAMS = $(TESTS)

AM_CFLAGS = \
  -Wall -Werror
  # -I$(top_srcdir)/include \
  # -I$(abs_top_builddir)/include

AM_LDFLAGS = \
  -L $(top_buidldir)/src/ \
  -Wl --start-group

unittests_SOURCES = \
  ../src/*.c \ ## Recommended to have as few of the sources here as possible.
  UnitTests.c

AUTOMAKE_OPTIONS = subdir-objects