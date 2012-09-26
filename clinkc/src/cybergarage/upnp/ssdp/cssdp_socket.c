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
*	File: cssdp_socket.c
*
*	Revision:
*
*	02/18/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cservice.h>
#include <cybergarage/upnp/cupnp_function.h>
#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/upnp/ssdp/cssdp.h>
#include <cybergarage/net/cinterface.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_ssdp_socket_notify
****************************************/

static BOOL cg_upnp_ssdp_socket_notify(CgUpnpSSDPSocket *ssdpSock, CgUpnpSSDPRequest *ssdpReq, char *ssdpAddr)
{
	CgString *ssdpMsg;
	int sentLen;

	cg_log_debug_l4("Entering...\n");

	cg_upnp_ssdprequest_setmethod(ssdpReq, CG_HTTP_NOTIFY);
	
	ssdpMsg = cg_string_new();
	cg_upnp_ssdprequest_tostring(ssdpReq, ssdpMsg);

	sentLen = cg_socket_sendto(ssdpSock, ssdpAddr, CG_UPNP_SSDP_PORT, cg_string_getvalue(ssdpMsg), cg_string_length(ssdpMsg));
	cg_string_delete(ssdpMsg);
	
	cg_log_debug_l4("Leaving...\n");

	return (0 < sentLen) ? TRUE : FALSE;
}

/****************************************
* cg_upnp_ssdp_socket_notifyfrom
****************************************/

BOOL cg_upnp_ssdp_socket_notifyfrom(CgUpnpSSDPSocket *ssdpSock, CgUpnpSSDPRequest *ssdpReq, char *bindAddr)
{
	char *ssdpAddr;
	
	cg_log_debug_l4("Entering...\n");

	ssdpAddr = cg_upnp_ssdp_gethostaddress(bindAddr);
	cg_upnp_ssdprequest_sethost(ssdpReq, ssdpAddr, CG_UPNP_SSDP_PORT);
	return cg_upnp_ssdp_socket_notify(ssdpSock, ssdpReq, ssdpAddr);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_socket_postresponse
****************************************/

BOOL cg_upnp_ssdp_socket_postresponse(CgUpnpSSDPSocket *ssdpSock, CgUpnpSSDPResponse *ssdpRes, char *host, int port)
{
	CgString *ssdpMsg;
	BOOL postSuccess;

	cg_log_debug_l4("Entering...\n");

	ssdpMsg = cg_string_new();
	cg_upnp_ssdpresponse_tostring(ssdpRes, ssdpMsg);
	postSuccess = cg_socket_sendto(ssdpSock, host, port, cg_string_getvalue(ssdpMsg), cg_string_length(ssdpMsg));
	cg_string_delete(ssdpMsg);

	cg_log_debug_l4("Leaving...\n");

	return postSuccess;
}
