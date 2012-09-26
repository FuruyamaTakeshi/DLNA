/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license with patent exclusion,
*       see file COPYING.
*
*	File: csocket_list.c
*
*	Revision:
*
*	07/20/05
*		- first revision
*
******************************************************************/

#include <cybergarage/net/csocket.h>
#include <cybergarage/util/clog.h>

#if defined(CG_NET_USE_SOCKET_LIST)

/****************************************
* cg_socketlist_new
****************************************/

CgSocketList *cg_socketlist_new()
{
	cg_log_debug_l4("Entering...\n");

	CgSocketList *socketList = (CgSocketList *)malloc(sizeof(CgSocketList));

	if ( NULL != socketList )
		cg_list_header_init((CgList *)socketList);

	return socketList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_socketlist_delete
****************************************/

void cg_socketlist_delete(CgSocketList *socketList)
{
	cg_log_debug_l4("Entering...\n");

	cg_socketlist_clear(socketList);
	free(socketList);

	cg_log_debug_l4("Leaving...\n");
}

#endif
