if HAVE_LIBTOOL
lib_LTLIBRARIES = libclinkcav.la
else
lib_LIBRARIES = libclinkcav.a
endif

INCLUDES = -I$(top_srcdir)/include

########################################
# Sources
########################################

clinkcav_source = \
	../../src/cybergarage/upnp/std/av/ccontent.c \
	../../src/cybergarage/upnp/std/av/ccontent_list.c \
	../../src/cybergarage/upnp/std/av/cdidl.c \
	../../src/cybergarage/upnp/std/av/cmd5.c \
	../../src/cybergarage/upnp/std/av/cresource.c \
	../../src/cybergarage/upnp/std/av/cresource_list.c \
	../../src/cybergarage/upnp/std/av/cresource_data.c \
	../../src/cybergarage/upnp/std/av/server/cconnectionmgr_service.c \
	../../src/cybergarage/upnp/std/av/server/ccontentdir_service.c \
	../../src/cybergarage/upnp/std/av/server/cmediaserver_device.c

########################################
# Library
########################################

libclinkcavincludedir = $(includedir)/cybergarage
nobase_libclinkcavinclude_HEADERS =  \
        $(clinkcav_header)

if HAVE_LIBTOOL
libclinkcav_la_SOURCES = \
        $(clinkcav_source)
libclinkcav_a_CFLAGS = $(AM_CFLAGS)
libclinkcav_la_LIBADD =
else
libclinkcav_a_SOURCES = \
        $(clinkcav_source)
libclinkcav_a_CFLAGS = $(AM_CFLAGS)
libclinkcav_a_LIBADD =
endif
