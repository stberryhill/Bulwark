## Process this file with automake to produce Makefile.in
# SUBDIRS = [src/tests src/examples] <- these are other sbudirs with makefiles. will be built first.
dist_doc_DATA = Readme.md
EXTRA_DIST = autogen.sh ## Include autogen.sh in tarball distributions
AUTOMAKE_OPTIONS = subdir-objects ## Have to have this if we use subdirectories
AM_LDFLAGS = -shared -O3
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src tests

# if OS_IS_WINDOWS
#   SUBDIRS += \
#     src/win
# endif

# if OS_IS_LINUX
#   SUBDIRS += \
#     src/unix
#     src/linux
# endif

# if OS_IS_MACOS
#   SUBDIRS += \
#     src/unix
#     src/macos
# endif
