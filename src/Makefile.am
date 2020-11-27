lib_LTLIBRARIES = libbulwark.la
libbulwark_la_CFLAGS = -I(abs_top_builddir)/include -std=c89 -Wall -Werror

libbulwark_la_SOURCES = \
  *.c

bin_PROGRAMS = devapp
devapp_CFLAGS = -I(abs_top_builddir)/include -std=c89 -Wall -Werror
devapp_SOURCES = \
  main.c
devapp_LDADD = -L(abs_top_builddir)/src/.libs libbulwark.la
