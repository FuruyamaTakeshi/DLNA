samplesdir = $(datadir)/doc/$(PACKAGE)-$(VERSION)/samples

samples_PROGRAMS = upnpigddump

INCLUDES = -I$(top_srcdir)/include -I$(top_builddir)/include -I../ -I../../common @XML_CFLAGS@ @CLOG_DEBUG_FLAGS@

noinst_HEADERS = \
	../../common/kb_util.h

upnpigddump_SOURCES = \
	../upnpigddump.c \
	../../common/kb_util.c

#if HAVE_LIBTOOL
#upnpigddump_LDADD = ../../../lib/unix/libclinkc.la @XML_LIBS@
#else
upnpigddump_LDADD = ../../../lib/unix/libclinkc.a @XML_LIBS@
#endif
