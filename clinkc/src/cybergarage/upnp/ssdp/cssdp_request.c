/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cssdp_request.c
*
*	Revision:
*
*	03/05/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/ssdp/cssdp.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_ssdprequest_new
****************************************/

CgUpnpSSDPRequest *cg_upnp_ssdprequest_new()
{
	CgUpnpSSDPRequest *ssdpReq;

	cg_log_debug_l4("Entering...\n");

	ssdpReq = cg_http_request_new();
	
	cg_http_request_seturi(ssdpReq, "*");
	cg_http_request_setversion(ssdpReq, CG_HTTP_VER11);
	cg_http_request_setcontentlength(ssdpReq, 0);

	cg_log_debug_l4("Leaving...\n");
	
	return ssdpReq;
}

/****************************************
* cg_upnp_ssdprequest_delete
****************************************/

void cg_upnp_ssdprequest_delete(CgUpnpSSDPRequest *ssdpReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_request_delete(ssdpReq);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdprequest_setleasetime
****************************************/

void cg_upnp_ssdprequest_setleasetime(CgUpnpSSDPRequest *ssdpReq, int value)
{
	char buf[sizeof(CG_HTTP_MAX_AGE) + 1 + CG_STRING_INTEGER_BUFLEN];
	
	cg_log_debug_l4("Entering...\n");

	sprintf(buf, "%s=%d", CG_HTTP_MAX_AGE, value);
	cg_http_packet_setheadervalue((CgHttpPacket*)ssdpReq, CG_HTTP_CACHE_CONTROL, buf);

	cg_log_debug_l4("Leaving...\n");
}
		
/****************************************
* cg_upnp_ssdprequest_getleasetime
****************************************/

int cg_upnp_ssdprequest_getleasetime(CgUpnpSSDPRequest *ssdpReq)
{
	char *cacheCtrl;

	cg_log_debug_l4("Entering...\n");

	cacheCtrl = cg_http_packet_getheadervalue((CgHttpPacket*)ssdpReq, CG_HTTP_CACHE_CONTROL);
	
	return cg_upnp_ssdp_getleasetime(cacheCtrl);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdprequest_tostring
****************************************/

char *cg_upnp_ssdprequest_tostring(CgUpnpSSDPRequest *ssdpReq, CgString *ssdpMsg)
{
	CgHttpHeader *header;
	char *name;
	char *value;
	
	cg_log_debug_l4("Entering...\n");

	cg_string_addvalue(ssdpMsg, cg_http_request_getmethod(ssdpReq));
	cg_string_addvalue(ssdpMsg, CG_HTTP_SP);
	cg_string_addvalue(ssdpMsg, cg_http_request_geturi(ssdpReq));
	cg_string_addvalue(ssdpMsg, CG_HTTP_SP);
	cg_string_addvalue(ssdpMsg, cg_http_request_getversion(ssdpReq));
	cg_string_addvalue(ssdpMsg, CG_HTTP_CRLF);
	
	for (header = cg_http_packet_getheaders((CgHttpPacket *)ssdpReq); header != NULL; header = cg_http_header_next(header)) {
		name = cg_http_header_getname(header);
		value = cg_http_header_getvalue(header);
		cg_string_addvalue(ssdpMsg, name);
		cg_string_addvalue(ssdpMsg, CG_HTTP_COLON);
		cg_string_addvalue(ssdpMsg, CG_HTTP_SP);
		cg_string_addvalue(ssdpMsg, value);
		cg_string_addvalue(ssdpMsg, CG_HTTP_CRLF);
	}
	cg_string_addvalue(ssdpMsg, CG_HTTP_CRLF);
	
	return cg_string_getvalue(ssdpMsg);

	cg_log_debug_l4("Leaving...\n");
}
