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
*	02/13/06 Theo Beisch
*		- cg_upnp_ssdp_serverlist_open now returns FALSE
*		  if no interface had successful open
*
******************************************************************/

#include <cybergarage/upnp/ssdp/cssdp_server.h>
#include <cybergarage/net/cinterface.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_ssdp_serverlist_new
****************************************/

CgUpnpSSDPServerList *cg_upnp_ssdp_serverlist_new()
{
	CgUpnpSSDPServerList *ssdpServerList;

	cg_log_debug_l4("Entering...\n");

	ssdpServerList = (CgUpnpSSDPServerList *)malloc(sizeof(CgUpnpSSDPServerList));

	if ( NULL != ssdpServerList )
	{
		cg_list_header_init((CgList *)ssdpServerList);
		
		ssdpServerList->httpmuSock = NULL;
		ssdpServerList->recvThread = NULL;
		ssdpServerList->listener = NULL;
	}
	
	return ssdpServerList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_serverlist_delete
****************************************/

void cg_upnp_ssdp_serverlist_delete(CgUpnpSSDPServerList *ssdpServerList)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_ssdp_serverlist_clear(ssdpServerList);
	
	free(ssdpServerList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_serverlist_open
****************************************/

BOOL cg_upnp_ssdp_serverlist_open(CgUpnpSSDPServerList *ssdpServerList)
{
	CgNetworkInterfaceList *netIfList;
	CgNetworkInterface *netIf;
	CgUpnpSSDPServer *ssdpServer;
	char *bindAddr;
	
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
		ssdpServer = cg_upnp_ssdp_server_new();
		if (cg_upnp_ssdp_server_open(ssdpServer, bindAddr) == FALSE) {
			cg_upnp_ssdp_server_delete(ssdpServer);
			continue;
		}
		cg_upnp_ssdp_serverlist_add(ssdpServerList, ssdpServer);
	}

	cg_net_interfacelist_delete(netIfList);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;	
}

/****************************************
* cg_upnp_ssdp_serverlist_close
****************************************/

BOOL cg_upnp_ssdp_serverlist_close(CgUpnpSSDPServerList *ssdpServerList)
{
	CgUpnpSSDPServer *ssdpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (ssdpServer = cg_upnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = cg_upnp_ssdp_server_next(ssdpServer))
		cg_upnp_ssdp_server_close(ssdpServer);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_upnp_ssdp_serverlist_start
****************************************/

BOOL cg_upnp_ssdp_serverlist_start(CgUpnpSSDPServerList *ssdpServerList)
{
	CgUpnpSSDPServer *ssdpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (ssdpServer = cg_upnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = cg_upnp_ssdp_server_next(ssdpServer))
		cg_upnp_ssdp_server_start(ssdpServer);
	
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_upnp_ssdp_serverlist_stop
****************************************/

BOOL cg_upnp_ssdp_serverlist_stop(CgUpnpSSDPServerList *ssdpServerList)
{
	CgUpnpSSDPServer *ssdpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (ssdpServer = cg_upnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = cg_upnp_ssdp_server_next(ssdpServer))
		cg_upnp_ssdp_server_stop(ssdpServer);
		
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_upnp_ssdp_serverlist_setlistener
****************************************/

void cg_upnp_ssdp_serverlist_setlistener(CgUpnpSSDPServerList *ssdpServerList, CG_UPNP_SSDP_LISTNER listener)
{
	CgUpnpSSDPServer *ssdpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (ssdpServer = cg_upnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = cg_upnp_ssdp_server_next(ssdpServer))
		cg_upnp_ssdp_server_setlistener(ssdpServer, listener);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_serverlist_setuserdata
****************************************/

void cg_upnp_ssdp_serverlist_setuserdata(CgUpnpSSDPServerList *ssdpServerList, void *data)
{
	CgUpnpSSDPServer *ssdpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (ssdpServer = cg_upnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = cg_upnp_ssdp_server_next(ssdpServer))
		cg_upnp_ssdp_server_setuserdata(ssdpServer, data);

	cg_log_debug_l4("Leaving...\n");
}

