/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cssdp_notify_server_list.c
*
*	Revision:
*
*	05/31/05
*		- first revision
*
*	10/31/05
*		- Changed "continue;" to "return FALSE;" in serverlist_open
*		  to indicate that port allocation was unsuccessful
*	12/14/05
*		- Fixed memory leak situation, which was introduced by 
*		  the previous patch
*	02/13/06 Theo Beisch
*		- cg_upnp_ssdpresponse_serverlist_open now returns true if
*		  at least a single open was successfull (WINCE support)
*
******************************************************************/

#include <cybergarage/upnp/ssdp/cssdp_server.h>
#include <cybergarage/net/cinterface.h>
#include <cybergarage/util/clog.h>

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_CONTROLPOINT)

/****************************************
* cg_upnp_ssdpresponse_serverlist_new
****************************************/

CgUpnpSSDPResponseServerList *cg_upnp_ssdpresponse_serverlist_new()
{
	CgUpnpSSDPResponseServerList *ssdpServerList;

	cg_log_debug_l4("Entering...\n");

	ssdpServerList = (CgUpnpSSDPResponseServerList *)malloc(sizeof(CgUpnpSSDPResponseServerList));

	if  ( NULL != ssdpServerList )
	{
		cg_list_header_init((CgList *)ssdpServerList);
		
		ssdpServerList->httpuSock = NULL;
		ssdpServerList->recvThread = NULL;
		ssdpServerList->listener = NULL;
	}
	
	return ssdpServerList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdpresponse_serverlist_delete
****************************************/

void cg_upnp_ssdpresponse_serverlist_delete(CgUpnpSSDPResponseServerList *ssdpServerList)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_ssdpresponse_serverlist_clear(ssdpServerList);
	
	free(ssdpServerList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdpresponse_serverlist_open
****************************************/

BOOL cg_upnp_ssdpresponse_serverlist_open(CgUpnpSSDPResponseServerList *ssdpServerList, int bindPort)
{
	CgNetworkInterfaceList *netIfList;
	CgNetworkInterface *netIf;
	CgUpnpSSDPResponseServer *ssdpServer;
	char *bindAddr;
	BOOL result = FALSE;	

	cg_log_debug_l4("Entering...\n");

	netIfList = cg_net_interfacelist_new();
#ifndef CG_NET_USE_ANYADDR
	cg_net_gethostinterfaces(netIfList);
#else
	netIf = cg_net_interface_getany();
	cg_net_interfacelist_add(netIfList, netIf);
#endif
	for (netIf = cg_net_interfacelist_gets(netIfList); netIf; netIf = cg_net_interface_next(netIf)) {
		bindAddr = cg_net_interface_getaddress(netIf);
		if (cg_strlen(bindAddr) <= 0)
			continue;
		ssdpServer = cg_upnp_ssdpresponse_server_new();
		if (cg_upnp_ssdpresponse_server_open(ssdpServer, bindPort, bindAddr) == FALSE) {
			cg_upnp_ssdpresponse_server_delete(ssdpServer);
			continue;
		}
		cg_upnp_ssdpresponse_serverlist_add(ssdpServerList, ssdpServer);
		result = TRUE;
	}

	if (result == FALSE) cg_upnp_ssdpresponse_serverlist_clear(ssdpServerList);

	cg_net_interfacelist_delete(netIfList);

	return result;	
}

/****************************************
* cg_upnp_ssdpresponse_serverlist_close
****************************************/

BOOL cg_upnp_ssdpresponse_serverlist_close(CgUpnpSSDPResponseServerList *ssdpServerList)
{
	CgUpnpSSDPResponseServer *ssdpServer;

	cg_log_debug_l4("Entering...\n");
	
	for (ssdpServer = cg_upnp_ssdpresponse_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = cg_upnp_ssdpresponse_server_next(ssdpServer))
		cg_upnp_ssdpresponse_server_close(ssdpServer);

	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdpresponse_serverlist_start
****************************************/

BOOL cg_upnp_ssdpresponse_serverlist_start(CgUpnpSSDPResponseServerList *ssdpServerList)
{
	CgUpnpSSDPResponseServer *ssdpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (ssdpServer = cg_upnp_ssdpresponse_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = cg_upnp_ssdpresponse_server_next(ssdpServer))
		cg_upnp_ssdpresponse_server_start(ssdpServer);
	
	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdpresponse_serverlist_stop
****************************************/

BOOL cg_upnp_ssdpresponse_serverlist_stop(CgUpnpSSDPResponseServerList *ssdpServerList)
{
	CgUpnpSSDPResponseServer *ssdpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (ssdpServer = cg_upnp_ssdpresponse_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = cg_upnp_ssdpresponse_server_next(ssdpServer))
		cg_upnp_ssdpresponse_server_stop(ssdpServer);
		
	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdpresponse_serverlist_setlistener
****************************************/

void cg_upnp_ssdpresponse_serverlist_setlistener(CgUpnpSSDPResponseServerList *ssdpServerList, CG_UPNP_SSDP_LISTNER listener)
{
	CgUpnpSSDPResponseServer *ssdpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (ssdpServer = cg_upnp_ssdpresponse_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = cg_upnp_ssdpresponse_server_next(ssdpServer))
		cg_upnp_ssdpresponse_server_setlistener(ssdpServer, listener);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdpresponse_serverlist_setuserdata
****************************************/

void cg_upnp_ssdpresponse_serverlist_setuserdata(CgUpnpSSDPResponseServerList *ssdpServerList, void *data)
{
	CgUpnpSSDPResponseServer *ssdpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (ssdpServer = cg_upnp_ssdpresponse_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = cg_upnp_ssdpresponse_server_next(ssdpServer))
		cg_upnp_ssdpresponse_server_setuserdata(ssdpServer, data);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdpresponse_serverlist_post
****************************************/

BOOL cg_upnp_ssdpresponse_serverlist_post(CgUpnpSSDPResponseServerList *ssdpServerList, CgUpnpSSDPRequest *ssdpReq)
{
	CgUpnpSSDPResponseServer *ssdpServer;
	BOOL success = TRUE;
	
	cg_log_debug_l4("Entering...\n");

	for (ssdpServer = cg_upnp_ssdpresponse_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = cg_upnp_ssdpresponse_server_next(ssdpServer))
		success &= cg_upnp_ssdpresponse_server_post(ssdpServer, ssdpReq);
		
	return success;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (End)
****************************************/

#endif
