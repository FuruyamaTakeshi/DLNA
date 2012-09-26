samplesdir = $(datadir)/doc/$(PACKAGE)-$(VERSION)/samples

samples_PROGRAMS = upnpclock

INCLUDES = -I$(top_srcdir)/include -I$(top_builddir)/include -I$(srcdir)/../ -I$(srcdir)/../../common @XML_CFLAGS@ @CLOG_DEBUG_FLAGS@

noinst_HEADERS = \
	../clock.h \
	../clock_device.h \
	../../common/kb_util.h

upnpclock_SOURCES = \
	clock_main.c \
	../clock.c \
	../clock_device.c \
	../../common/kb_util.c

#if HAVE_LIBTOOL
#upnpclock_LDADD = ../../../lib/unix/libclinkc.la @XML_LIBS@
#else
upnpclock_LDADD = ../../../lib/unix/libclinkc.a @XML_LIBS@
#endif
