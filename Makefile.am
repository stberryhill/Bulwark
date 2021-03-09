## Process this file with automake to produce Makefile.in
# SUBDIRS = [src/tests src/examples] <- these are other sbudirs with makefiles. will be built first.
dist_doc_DATA = README.md
AUTOMAKE_OPTIONS = subdir-objects
AM_LDFLAGS = -shared -O3
ACLOCAL_AMFLAGS = -I m4

if BUILD_EXAMPLES
  optionalSubdirs = examples
endif

SUBDIRS = src tests $(optionalSubdirs)
