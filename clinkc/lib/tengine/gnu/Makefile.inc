 ##################################################
#
#	CyberLink for C
#
#	Copyright (C) Satoshi Konno 2005
#
#	File: Makefile.inc
#
#	Revision:
#
#	03/22/05
#		- first revision
#
##################################################

CLINKCHEADERROOT = $(CLINKCROOT)/include
CLINKCSRCROOT = $(CLINKCROOT)/src/cybergarage

CLINKCUTILSRC = \
	$(CLINKCSRCROOT)/util/clist \
	$(CLINKCSRCROOT)/util/cdebug.c \
	$(CLINKCSRCROOT)/util/cmutex.c \
	$(CLINKCSRCROOT)/util/cstring.c \
	$(CLINKCSRCROOT)/util/cstring_function.c \
	$(CLINKCSRCROOT)/util/cstring_tokenizer.c \
	$(CLINKCSRCROOT)/util/cthread.c \
	$(CLINKCSRCROOT)/util/cthread_list.c \
	$(CLINKCSRCROOT)/util/ctime.c

CLINKCNETSRC = \
	$(CLINKCSRCROOT)/net/cdatagram_packet.c \
	$(CLINKCSRCROOT)/net/cinterface.c \
	$(CLINKCSRCROOT)/net/cinterface_function.c \
	$(CLINKCSRCROOT)/net/cinterface_list.c \
	$(CLINKCSRCROOT)/net/cnet_function.c \
	$(CLINKCSRCROOT)/net/csocket.c \
	$(CLINKCSRCROOT)/net/curi.c \
	$(CLINKCSRCROOT)/net/curl.c

CLINKCHTTP = \
	$(CLINKCSRCROOT)/http/chttp.c \
	$(CLINKCSRCROOT)/http/chttp_date \
	$(CLINKCSRCROOT)/http/chttp_header.c \
	$(CLINKCSRCROOT)/http/chttp_header_list.c \
	$(CLINKCSRCROOT)/http/chttp_packet.c \
	$(CLINKCSRCROOT)/http/chttp_persistent_connection.c \	
	$(CLINKCSRCROOT)/http/chttp_request.c \
	$(CLINKCSRCROOT)/http/chttp_response.c \
	$(CLINKCSRCROOT)/http/chttp_server.c \
	$(CLINKCSRCROOT)/http/chttp_server_list.c
	
CLINKCXMLSRC = \
	$(CLINKCSRCROOT)/xml/cxml_attribute.c \
	$(CLINKCSRCROOT)/xml/cxml_attribute_list.c \
	$(CLINKCSRCROOT)/xml/cxml_function.c \
	$(CLINKCSRCROOT)/xml/cxml_node.c \
	$(CLINKCSRCROOT)/xml/cxml_node_list.c \
	$(CLINKCSRCROOT)/xml/cxml_parser.c \
	$(CLINKCSRCROOT)/xml/cxml_parser_expat.c
	
CLINKCSOAP = \
	$(CLINKCSRCROOT)/soap/csoap.c \
	$(CLINKCSRCROOT)/soap/csoap_request.c \
	$(CLINKCSRCROOT)/soap/csoap_response.c 

CLINKCUPNP = \
	$(CLINKCSRCROOT)/upnp/caction.c \
	$(CLINKCSRCROOT)/upnp/caction_list.c \
	$(CLINKCSRCROOT)/upnp/cargument.c \
	$(CLINKCSRCROOT)/upnp/cargument_list.c \
	$(CLINKCSRCROOT)/upnp/ccontrolpoint.c \
	$(CLINKCSRCROOT)/upnp/ccontrolpoint_event.c \
	$(CLINKCSRCROOT)/upnp/ccontrolpoint_http_server.c \
	$(CLINKCSRCROOT)/upnp/cdevice.c \
	$(CLINKCSRCROOT)/upnp/cdevice_list.c \
	$(CLINKCSRCROOT)/upnp/cdevice_advertiser.c \
	$(CLINKCSRCROOT)/upnp/cdevice_http_server.c \
	$(CLINKCSRCROOT)/upnp/cdevice_ssdp_server.c \
	$(CLINKCSRCROOT)/upnp/cservice_ssdp_server.c \
	$(CLINKCSRCROOT)/upnp/cicon.c \
	$(CLINKCSRCROOT)/upnp/cicon_list.c \
	$(CLINKCSRCROOT)/upnp/cservice.c \
	$(CLINKCSRCROOT)/upnp/cservice_list.c \
	$(CLINKCSRCROOT)/upnp/cservice_notify.c \
	$(CLINKCSRCROOT)/upnp/cservicestate_table.c \
	$(CLINKCSRCROOT)/upnp/cstatevariable.c \
	$(CLINKCSRCROOT)/upnp/cupnp_function.c \
	$(CLINKCSRCROOT)/upnp/cupnp_status.c \
	$(CLINKCSRCROOT)/upnp/control/caction_ctrl.c \
	$(CLINKCSRCROOT)/upnp/control/caction_request.c \
	$(CLINKCSRCROOT)/upnp/control/caction_response.c \
	$(CLINKCSRCROOT)/upnp/control/ccontrol_function.c \
	$(CLINKCSRCROOT)/upnp/control/cquery_ctrl.c \
	$(CLINKCSRCROOT)/upnp/control/cquery_request.c \
	$(CLINKCSRCROOT)/upnp/control/cquery_response.c \
	$(CLINKCSRCROOT)/upnp/event/ceventlistener_list.c \
	$(CLINKCSRCROOT)/upnp/event/cnotify_request.c \
	$(CLINKCSRCROOT)/upnp/event/cproperty.c \
	$(CLINKCSRCROOT)/upnp/event/cproperty_list.c \
	$(CLINKCSRCROOT)/upnp/event/csubscriber.c \
	$(CLINKCSRCROOT)/upnp/event/csubscriber_list.c \
	$(CLINKCSRCROOT)/upnp/event/csubscription.c \
	$(CLINKCSRCROOT)/upnp/event/csubscription_request.c \
	$(CLINKCSRCROOT)/upnp/event/csubscription_response.c \
	$(CLINKCSRCROOT)/upnp/ssdp/chttpmu_socket.c \
	$(CLINKCSRCROOT)/upnp/ssdp/chttpu_socket.c \
	$(CLINKCSRCROOT)/upnp/ssdp/cssdp.c \
	$(CLINKCSRCROOT)/upnp/ssdp/cssdp_server.c \
	$(CLINKCSRCROOT)/upnp/ssdp/cssdp_server_list.c \
	$(CLINKCSRCROOT)/upnp/ssdp/cssdp_packet.c \
	$(CLINKCSRCROOT)/upnp/ssdp/cssdp_request.c \
	$(CLINKCSRCROOT)/upnp/ssdp/cssdp_response.c \
	$(CLINKCSRCROOT)/upnp/ssdp/cssdp_socket.c \
	$(CLINKCSRCROOT)/upnp/ssdp/cssdp_response_server.c \
	$(CLINKCSRCROOT)/upnp/ssdp/cssdp_response_server_list.c
	
EXPATSRC = \
	$(EXPATROOT)/lib/xmlparse.c \
	$(EXPATROOT)/lib/xmlrole.c \
	$(EXPATROOT)/lib/xmltok.c
#	$(EXPATROOT)/lib/xmltok_ns.c
#	$(EXPATROOT)/lib/xmltok_impl.c \

unexport CLINKCSRC =
CLINKCSRC = \
	$(CLINKCUTILSRC) \
	$(CLINKCNETSRC) \
	$(CLINKCHTTP) \
	$(CLINKCXMLSRC) \
	$(CLINKCSOAP) \
	$(CLINKCUPNP) \
	$(EXPATSRC)

# for Expat
CFLAGS += -DHAVE_MEMMOVE

