#if HAVE_LIBTOOL
#lib_LTLIBRARIES = libclinkc.la
#else
lib_LIBRARIES = libclinkc.a
#endif

INCLUDES = -I$(top_srcdir)/include -I$(top_builddir)/include @XML_CFLAGS@ @HTTP_CFLAGS@ @CLOG_DEBUG_FLAGS@

########################################
# CyberLink
########################################

clinkc_source = \
	../../src/cybergarage/io/cfile.c \
	../../src/cybergarage/io/cfile_list.c \
	../../src/cybergarage/util/clist.c \
	../../src/cybergarage/util/cmutex.c \
	../../src/cybergarage/util/ccond.c \
	../../src/cybergarage/util/cdictionary.c \
	../../src/cybergarage/util/cdictionary_elem.c \
	../../src/cybergarage/util/cstring.c \
	../../src/cybergarage/util/cstring_tokenizer.c \
	../../src/cybergarage/util/cstring_function.c \
	../../src/cybergarage/util/cthread.c \
	../../src/cybergarage/util/cthread_list.c \
	../../src/cybergarage/util/ctime.c \
	../../src/cybergarage/util/clog.c \
	../../src/cybergarage/net/cdatagram_packet.c \
	../../src/cybergarage/net/csocket.c \
	../../src/cybergarage/net/cinterface.c \
	../../src/cybergarage/net/cinterface_list.c \
	../../src/cybergarage/net/cinterface_function.c \
	../../src/cybergarage/net/cnet_function.c \
	../../src/cybergarage/net/curi.c \
	../../src/cybergarage/net/curl.c \
	../../src/cybergarage/http/chttp.c \
	../../src/cybergarage/http/chttp_date.c \
	../../src/cybergarage/http/chttp_header.c \
	../../src/cybergarage/http/chttp_header_list.c \
	../../src/cybergarage/http/chttp_packet.c \
	../../src/cybergarage/http/chttp_persistent_connection.c \
	../../src/cybergarage/http/chttp_request.c \
	../../src/cybergarage/http/chttp_response.c \
	../../src/cybergarage/http/chttp_server.c \
	../../src/cybergarage/http/chttp_server_list.c \
	../../src/cybergarage/soap/csoap.c \
	../../src/cybergarage/soap/csoap_request.c \
	../../src/cybergarage/soap/csoap_response.c \
	../../src/cybergarage/upnp/caction.c \
	../../src/cybergarage/upnp/caction_list.c \
	../../src/cybergarage/upnp/cargument.c \
	../../src/cybergarage/upnp/cargument_list.c \
	../../src/cybergarage/upnp/ccontrolpoint.c \
	../../src/cybergarage/upnp/ccontrolpoint_event.c \
	../../src/cybergarage/upnp/ccontrolpoint_http_server.c \
	../../src/cybergarage/upnp/cdevice.c \
	../../src/cybergarage/upnp/cdevice_advertiser.c \
	../../src/cybergarage/upnp/cdevice_list.c \
	../../src/cybergarage/upnp/cdevice_http_server.c \
	../../src/cybergarage/upnp/cdevice_ssdp_server.c \
	../../src/cybergarage/upnp/cservice_ssdp_server.c \
	../../src/cybergarage/upnp/cicon.c \
	../../src/cybergarage/upnp/cicon_list.c \
	../../src/cybergarage/upnp/cservice.c \
	../../src/cybergarage/upnp/cservice_list.c \
	../../src/cybergarage/upnp/cservice_notify.c \
	../../src/cybergarage/upnp/cstatevariable.c \
	../../src/cybergarage/upnp/cservicestate_table.c \
	../../src/cybergarage/upnp/cupnp_function.c \
	../../src/cybergarage/upnp/cupnp_status.c \
	../../src/cybergarage/upnp/control/caction_ctrl.c \
	../../src/cybergarage/upnp/control/caction_request.c \
	../../src/cybergarage/upnp/control/caction_response.c \
	../../src/cybergarage/upnp/control/ccontrol_function.c \
	../../src/cybergarage/upnp/control/cquery_ctrl.c \
	../../src/cybergarage/upnp/control/cquery_request.c \
	../../src/cybergarage/upnp/control/cquery_response.c \
	../../src/cybergarage/upnp/event/ceventlistener_list.c \
	../../src/cybergarage/upnp/event/cnotify_request.c \
	../../src/cybergarage/upnp/event/cproperty.c \
	../../src/cybergarage/upnp/event/cproperty_list.c \
	../../src/cybergarage/upnp/event/csubscriber.c \
	../../src/cybergarage/upnp/event/csubscriber_list.c \
	../../src/cybergarage/upnp/event/csubscription.c \
	../../src/cybergarage/upnp/event/csubscription_request.c \
	../../src/cybergarage/upnp/event/csubscription_response.c \
	../../src/cybergarage/upnp/ssdp/chttpmu_socket.c \
	../../src/cybergarage/upnp/ssdp/chttpu_socket.c \
	../../src/cybergarage/upnp/ssdp/cssdp.c \
	../../src/cybergarage/upnp/ssdp/cssdp_server.c \
	../../src/cybergarage/upnp/ssdp/cssdp_server_list.c \
	../../src/cybergarage/upnp/ssdp/cssdp_packet.c \
	../../src/cybergarage/upnp/ssdp/cssdp_request.c \
	../../src/cybergarage/upnp/ssdp/cssdp_response.c \
	../../src/cybergarage/upnp/ssdp/cssdp_socket.c \
	../../src/cybergarage/upnp/ssdp/cssdp_response_server.c \
	../../src/cybergarage/upnp/ssdp/cssdp_response_server_list.c \
	../../src/cybergarage/xml/cxml_attribute.c \
	../../src/cybergarage/xml/cxml_attribute_list.c \
	../../src/cybergarage/xml/cxml_function.c \
	../../src/cybergarage/xml/cxml_node.c \
	../../src/cybergarage/xml/cxml_node_list.c \
	../../src/cybergarage/xml/cxml_parser.c \
	../../src/cybergarage/xml/cxml_parser_expat.c \
	../../src/cybergarage/xml/cxml_parser_libxml2.c

########################################
# Library
########################################

libclinkcincludedir = $(includedir)/cybergarage
nobase_libclinkcinclude_HEADERS =  \
        $(clinkc_header)

#if HAVE_LIBTOOL
#libclinkc_la_SOURCES = \
#        $(clinkc_source)
#libclinkc_la_LIBADD = @HTTP_LIBS@ @XML_LIBS@
#else
libclinkc_a_SOURCES = \
        $(clinkc_source)
libclinkc_a_CFLAGS = $(AM_CFLAGS)
libclinkc_a_LIBADD = @HTTP_LIBS@ @XML_LIBS@
#endif
