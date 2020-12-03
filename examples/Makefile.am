bin_PROGRAMS = colors

colors_CFLAGS = -I$(abs_top_builddir)/include -I$(top_srcdir)/include -std=c89 -Wall -Werror
colors_SOURCES = \
  Colors.c
colors_LDADD = -L$(abs_top_builddir)/src/.libs ../src/libbulwark.la
