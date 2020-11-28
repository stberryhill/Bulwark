bin_PROGRAMS = snakeit traceasciilogo
snakeit_CFLAGS = -I$(abs_top_builddir)/include -I$(top_srcdir)/include -std=c89 -Wall -Werror
snakeit_SOURCES = \
  SnakeIt.c

snakeit_LDADD = -L$(abs_top_builddir)/src/.libs ../src/libbulwark.la

traceasciilogo_CFLAGS = -I$(abs_top_builddir)/include -I$(top_srcdir)/include -std=c89 -Wall -Werror
traceasciilogo_SOURCES = \
  TraceAsciiLogo.c

traceasciilogo_LDADD = -L$(abs_top_builddir)/src/.libs ../src/libbulwark.la
