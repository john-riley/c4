
AM_CFLAGS = @CFLAGS@ @GCC_CFLAG@
INCLUDES = @BERKELEY_DB_CFLAGS@ @OPENSSL_LIB_CPPFLAGS@

noinst_LIBRARIES = libcfpub.a

libcfpub_a_SOURCES =							\
	getopt.c							\
	getopt1.c							\
	full-write.c							\
	regex.c								\
	getloadavg.c							\
	alloc.c								\
	strlcpy.c							\
	memdup.c							\
	snprintf.c

libcfpub_a_CFLAGS = $(AM_CFLAGS) -fPIC

EXTRA_DIST =								\
	getopt.h							\
	global.h							\
	gnuregex.h							\
	snprintf.h							\
	alloc.h								\
	bool.h								\
	deprecated.h							\
	compiler.h							\
	platform.h							\
	snprintf.h							\
	LICENSE.txt

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~

#
# Get everything removed down to where rebuilding requires:
# "make; make install"
# 
CLEANFILES = *.gcno *.gcda

#
# Get everything removed down to where rebuilding requires:
# "configure; make; make install"
#
DISTCLEANFILES = *~ *.orig *.rej

#
# Get everything removed down to where rebuilding requires:
# "aclocal; autoconf; autoheader; automake --add-missing"
# "configure; make; make install"
#
MAINTAINERCLEANFILES = Makefile.in
