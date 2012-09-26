########################################
# CyberLinkC
########################################

clinkc_header = \
	cybergarage/typedef.h \
	cybergarage/io/cfile.h \
	cybergarage/util/clist.h \
	cybergarage/util/cmutex.h \
	cybergarage/util/ccond.h \
	cybergarage/util/cdictionary.h \
	cybergarage/util/cstring.h \
	cybergarage/util/cthread.h \
	cybergarage/util/ctime.h \
	cybergarage/util/clog.h \
	cybergarage/net/csocket.h \
	cybergarage/net/cinterface.h \
	cybergarage/net/curi.h \
	cybergarage/net/curl.h \
	cybergarage/http/chttp.h \
	cybergarage/soap/csoap.h \
	cybergarage/upnp/caction.h \
	cybergarage/upnp/cargument.h \
	cybergarage/upnp/ccontrolpoint.h \
	cybergarage/upnp/cdevice.h \
	cybergarage/upnp/cicon.h \
	cybergarage/upnp/cservice.h \
	cybergarage/upnp/cstatevariable.h \
	cybergarage/upnp/cupnp.h \
	cybergarage/upnp/cupnp_function.h \
	cybergarage/upnp/cupnp_limit.h \
	cybergarage/upnp/cupnp_status.h \
	cybergarage/upnp/control/ccontrol.h \
	cybergarage/upnp/event/cevent.h \
	cybergarage/upnp/event/cnotify.h \
	cybergarage/upnp/event/cproperty.h \
	cybergarage/upnp/event/csubscriber.h \
	cybergarage/upnp/event/csubscription.h \
	cybergarage/upnp/ssdp/cssdp.h \
	cybergarage/upnp/ssdp/cssdp_server.h \
	cybergarage/xml/cxml.h

cybergarage/upnp/cupnp.h: $(top_builddir)/config.h
	mkdir -p cybergarage/upnp
	sed -e 's/CG_CLINK_VER/CG_CLINK_VER \"$(VERSION)\"/' \
	$(srcdir)/cybergarage/upnp/cupnp.h.in > $(srcdir)/cybergarage/upnp/cupnp.h

########################################
# Library
########################################

nobase_include_HEADERS = \
	$(clinkc_header)
