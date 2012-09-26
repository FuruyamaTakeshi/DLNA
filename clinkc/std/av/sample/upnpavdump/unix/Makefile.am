samples_PROGRAMS = upnpavdump

INCLUDES = -I$(top_srcdir)/include -I../ -I../../../common

noinst_HEADERS = \
	../../../common/ckeyboard.h

upnpavdump_SOURCES = \
	../../../common/ckeyboard.c \
	../upnpavdump.c

if HAVE_LIBTOOL
upnpavdump_LDADD = -lclinkc ../../../../lib/unix/libclinkcav.la
else
upnpavdump_LDADD = -lclinkc ../../../../lib/unix/libclinkcav.a
endif
cdms_filesys