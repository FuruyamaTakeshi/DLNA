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
*	File: cicon_list.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cicon.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_iconr_new
****************************************/

CgUpnpIconList *cg_upnp_iconlist_new()
{
	CgUpnpIcon *iconList;

	cg_log_debug_l4("Entering...\n");

	iconList = (CgUpnpIcon *)malloc(sizeof(CgUpnpIcon));

	if ( NULL != iconList )
		cg_list_header_init((CgList *)iconList);

	return iconList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_iconr_delete
****************************************/

void cg_upnp_iconlist_delete(CgUpnpIconList *iconList)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)iconList);
	free(iconList);

	cg_log_debug_l4("Leaving...\n");
}
