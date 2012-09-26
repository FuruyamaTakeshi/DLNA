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

#include <cybergarage/upnp/ssdp/cssdp_server.h>
#include <cybergarage/net/cinterface.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_ssdp_server_new
****************************************/

CgUpnpSSDPServer *cg_upnp_ssdp_server_new()
{
	CgUpnpSSDPServer *server;

	cg_log_debug_l4("Entering...\n");

	server = (CgUpnpSSDPServer *)malloc(sizeof(CgUpnpSSDPServer));

	if ( NULL != server )
	{
		cg_list_node_init((CgList *)server);

		server->httpmuSock = NULL;
		server->recvThread = NULL;

		cg_upnp_ssdp_server_setlistener(server, NULL);
		cg_upnp_ssdp_server_setuserdata(server, NULL);
	}
	
	cg_log_debug_l4("Leaving...\n");

	return server;
}

/****************************************
* cg_upnp_ssdp_server_delete
****************************************/

void cg_upnp_ssdp_server_delete(CgUpnpSSDPServer *server)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_ssdp_server_stop(server);
	cg_upnp_ssdp_server_close(server);
	
	cg_list_remove((CgList *)server);

	free(server);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_server_open
****************************************/

BOOL cg_upnp_ssdp_server_open(CgUpnpSSDPServer *server, char *bindAddr)
{
	char *ssdpAddr = CG_UPNP_SSDP_ADDRESS;

	cg_log_debug_l4("Entering...\n");

	if (cg_upnp_ssdp_server_isopened(server) == TRUE)
		return FALSE;
		
	if (cg_net_isipv6address(bindAddr) == TRUE)
		ssdpAddr = cg_upnp_ssdp_getipv6address();
	
	server->httpmuSock = cg_upnp_httpmu_socket_new();
	if (cg_upnp_httpmu_socket_bind(server->httpmuSock, ssdpAddr, CG_UPNP_SSDP_PORT, bindAddr) == FALSE) {
		cg_upnp_httpmu_socket_delete(server->httpmuSock);
		server->httpmuSock = NULL;
		return FALSE;
	}
	
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_upnp_ssdp_server_close
****************************************/

BOOL cg_upnp_ssdp_server_close(CgUpnpSSDPServer *server)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_ssdp_server_stop(server);
	
	if (server->httpmuSock != NULL) {
		cg_upnp_httpmu_socket_close(server->httpmuSock);
		cg_upnp_httpmu_socket_delete(server->httpmuSock);
		server->httpmuSock = NULL;
	}
	
	cg_log_debug_l4("Leaving...\n");
	return TRUE;
}

/****************************************
* cg_upnp_ssdp_server_performlistener
****************************************/

void cg_upnp_ssdp_server_performlistener(CgUpnpSSDPServer *server, CgUpnpSSDPPacket *ssdpPkt)
{
	CG_UPNP_SSDP_LISTNER listener;

	cg_log_debug_l4("Entering...\n");

	listener = cg_upnp_ssdp_server_getlistener(server);
	if (listener == NULL)
		return;
	listener(ssdpPkt);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_server_thread
****************************************/

static void cg_upnp_ssdp_server_thread(CgThread *thread)
{
	CgUpnpSSDPServer *server;
	CgUpnpSSDPPacket *ssdpPkt;
	void *userData;
	
	cg_log_debug_l4("Entering...\n");

	server = (CgUpnpSSDPServer *)cg_thread_getuserdata(thread);
	userData = cg_upnp_ssdp_server_getuserdata(server);
	
	if (cg_upnp_ssdp_server_isopened(server) == FALSE)
		return;

	ssdpPkt = cg_upnp_ssdp_packet_new();
	cg_upnp_ssdp_packet_setuserdata(ssdpPkt, userData);
	
	while (cg_thread_isrunnable(thread) == TRUE) {
		if (cg_upnp_httpmu_socket_recv(server->httpmuSock, ssdpPkt) <= 0)
			break;

		cg_upnp_ssdp_server_performlistener(server, ssdpPkt);
		cg_upnp_ssdp_packet_clear(ssdpPkt);
	}
	
	cg_upnp_ssdp_packet_delete(ssdpPkt);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_server_start
****************************************/

BOOL cg_upnp_ssdp_server_start(CgUpnpSSDPServer *server)
{
	if (server->recvThread != NULL)
		return FALSE;
		
	cg_log_debug_l4("Entering...\n");

	server->recvThread = cg_thread_new();
	cg_thread_setaction(server->recvThread, cg_upnp_ssdp_server_thread);
	cg_thread_setuserdata(server->recvThread, server);
	if (cg_thread_start(server->recvThread) == FALSE) {	
		cg_thread_delete(server->recvThread);
		server->recvThread = NULL;
		return FALSE;
	}

	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_server_stop
****************************************/

BOOL cg_upnp_ssdp_server_stop(CgUpnpSSDPServer *server)
{
	cg_log_debug_l4("Entering...\n");

	if (server->recvThread != NULL) {
		cg_thread_stop(server->recvThread);
		cg_thread_delete(server->recvThread);
		server->recvThread = NULL;
	}
	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

