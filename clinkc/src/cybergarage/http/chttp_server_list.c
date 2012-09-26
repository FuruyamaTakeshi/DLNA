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
*	File: chttp_server_list.c
*
*	Revision:
*
*	01/25/05
*		- first revision
*	12/14/05
*		- fixed to fail if opening assigned port fails
*
******************************************************************/

#include <cybergarage/http/chttp.h>
#include <cybergarage/net/cinterface.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_http_serverlist_new
****************************************/

CgHttpServerList *cg_http_serverlist_new()
{
	CgHttpServerList *httpServerList;

	cg_log_debug_l4("Entering...\n");

	httpServerList = (CgHttpServerList *)malloc(sizeof(CgHttpServerList));

	if ( NULL != httpServerList )
	{
		cg_list_header_init((CgList *)httpServerList);
		
		httpServerList->sock = NULL;
		httpServerList->acceptThread = NULL;
		httpServerList->listener = NULL;
		httpServerList->userData = NULL;
		httpServerList->mutex = NULL;
	}
	
	cg_log_debug_l4("Leaving...\n");

	return httpServerList;
}

/****************************************
* cg_http_serverlist_delete
****************************************/

void cg_http_serverlist_delete(CgHttpServerList *httpServerList)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_serverlist_clear(httpServerList);
	free(httpServerList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_serverlist_open
****************************************/

BOOL cg_http_serverlist_open(CgHttpServerList *httpServerList, int port)
{
	CgNetworkInterfaceList *netIfList;
	CgNetworkInterface *netIf;
	CgHttpServer *httpServer;
	char *bindAddr;
	BOOL result =FALSE;
	
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
		httpServer = cg_http_server_new();

		if (cg_http_server_open(httpServer, port, bindAddr) == FALSE) {
			/* Theo Beisch - why break off, 
			   we may be running ok on at least some IF??? 
			   (at least WINCE does...) */
			continue;
		}
		cg_http_serverlist_add(httpServerList, httpServer);
		result = TRUE; //at least one IF found ok
	}

	
	if (result==FALSE) cg_http_serverlist_clear(httpServerList);
	cg_net_interfacelist_delete(netIfList);

	return result;	
}

/****************************************
* cg_http_serverlist_close
****************************************/

BOOL cg_http_serverlist_close(CgHttpServerList *httpServerList)
{
	CgHttpServer *httpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (httpServer = cg_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = cg_http_server_next(httpServer))
		cg_http_server_close(httpServer);

	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_serverlist_start
****************************************/

BOOL cg_http_serverlist_start(CgHttpServerList *httpServerList)
{
	CgHttpServer *httpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (httpServer = cg_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = cg_http_server_next(httpServer))
		cg_http_server_start(httpServer);
	
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_http_serverlist_stop
****************************************/

BOOL cg_http_serverlist_stop(CgHttpServerList *httpServerList)
{
	CgHttpServer *httpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (httpServer = cg_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = cg_http_server_next(httpServer))
		cg_http_server_stop(httpServer);
		
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_http_serverlist_setlistener
****************************************/

void cg_http_serverlist_setlistener(CgHttpServerList *httpServerList, CG_HTTP_LISTENER listener)
{
	CgHttpServer *httpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (httpServer = cg_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = cg_http_server_next(httpServer))
		cg_http_server_setlistener(httpServer, listener);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_serverlist_setuserdata
****************************************/

void cg_http_serverlist_setuserdata(CgHttpServerList *httpServerList, void *value)
{
	CgHttpServer *httpServer;
	
	cg_log_debug_l4("Entering...\n");

	for (httpServer = cg_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = cg_http_server_next(httpServer))
		cg_http_server_setuserdata(httpServer, value);

	cg_log_debug_l4("Leaving...\n");
}
