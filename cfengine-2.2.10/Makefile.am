if BUILD_DOC
DOC_DIR = doc
else
DOC_DIR =
endif

SUBDIRS = pub src contrib inputs $(DOC_DIR)

EXTRA_DIST = acconfig.h doc/cfengine.8 COPYING ChangeLog INSTALL NEWS README LICENSE

docdir = $(pkgdatadir)
doc_DATA = README ChangeLog INSTALL NEWS

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~

#
# Get everything removed down to where rebuilding requires:
# "make; make install"
# 
CLEANFILES =

#
# Get everything removed down to where rebuilding requires:
# "configure; make; make install"
#
DISTCLEANFILES = 

#
# Get everything removed down to where rebuilding requires:
# "aclocal; autoconf; autoheader; automake --add-missing"
# "configure; make; make install"
#
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.sub \
	configure install-sh missing mkinstalldirs depcomp ylwrap \
	ltmain.sh mdate-sh
