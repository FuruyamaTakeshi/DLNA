noinst_PROGRAMS = cdms_filesys

INCLUDES = -I$(top_srcdir)/include -I../ -I../../../common

noinst_HEADERS = \
	../../../common/ckeyboard.h

cdms_filesys_SOURCES = \
	../../../common/ckeyboard.c \
	../cdms_filesys.c \
	../cdms_filesys_data.c \
	../cdms_filesys_http.c \
	../cdms_filesys_main.c

if HAVE_LIBTOOL
cdms_filesys_LDADD = -lclinkc ../../../../lib/unix/libclinkcav.la
else
cdms_filesys_LDADD = -lclinkc ../../../../lib/unix/libclinkcav.a
endif
