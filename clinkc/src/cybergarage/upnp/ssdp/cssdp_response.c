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
#include <cybergarage/util/cstring.h>
#include <cybergarage/util/clog.h>

#define CG_UPNP_SSDP_MAXAGE_LEN 7

/****************************************
* cg_upnp_ssdpresponse_new
****************************************/

CgUpnpSSDPResponse *cg_upnp_ssdpresponse_new()
{
	CgUpnpSSDPResponse*ssdpRes;

	cg_log_debug_l4("Entering...\n");

	ssdpRes = cg_http_response_new();
	
	cg_http_response_setversion(ssdpRes, CG_HTTP_VER11);
	cg_http_response_setstatuscode(ssdpRes, CG_HTTP_STATUS_OK);

	return ssdpRes;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdpresponse_delete
****************************************/

void cg_upnp_ssdpresponse_delete(CgUpnpSSDPResponse *ssdpRes)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_response_delete(ssdpRes);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdpresponse_setleasetime
****************************************/

void cg_upnp_ssdpresponse_setleasetime(CgUpnpSSDPResponse *ssdpRes, int value)
{
	char buf[CG_UPNP_SSDP_MAXAGE_LEN + 1 + CG_STRING_INTEGER_BUFLEN];

	cg_log_debug_l4("Entering...\n");

	sprintf(buf, "%s=%d", CG_HTTP_MAX_AGE, value);
	cg_http_packet_setheadervalue((CgHttpPacket*)ssdpRes, CG_HTTP_CACHE_CONTROL, buf);

	cg_log_debug_l4("Leaving...\n");
}
		
/****************************************
* cg_upnp_ssdpresponse_getleasetime
****************************************/

int cg_upnp_ssdpresponse_getleasetime(CgUpnpSSDPResponse *ssdpRes)
{
	char *cacheCtrl;

	cg_log_debug_l4("Entering...\n");

	cacheCtrl = cg_http_packet_getheadervalue((CgHttpPacket*)ssdpRes, CG_HTTP_CACHE_CONTROL);
	return cg_upnp_ssdp_getleasetime(cacheCtrl);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdpresponse_tostring
****************************************/

char *cg_upnp_ssdpresponse_tostring(CgUpnpSSDPResponse *ssdpRes, CgString *ssdpMsg)
{
	CgHttpHeader *header;
	char statusCodeBuf[CG_STRING_INTEGER_BUFLEN];
	char *name;
	char *value;
	
	cg_log_debug_l4("Entering...\n");

	cg_string_addvalue(ssdpMsg, cg_http_response_getversion(ssdpRes));
	cg_string_addvalue(ssdpMsg, CG_HTTP_SP);
	cg_string_addvalue(ssdpMsg, cg_int2str(cg_http_response_getstatuscode(ssdpRes), statusCodeBuf, sizeof(statusCodeBuf)));
	cg_string_addvalue(ssdpMsg, CG_HTTP_SP);
	cg_string_addvalue(ssdpMsg, cg_http_response_getreasonphrase(ssdpRes));
	cg_string_addvalue(ssdpMsg, CG_HTTP_CRLF);
	
	for (header = cg_http_packet_getheaders((CgHttpPacket *)ssdpRes); header != NULL; header = cg_http_header_next(header)) {
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

