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
*	File: cicon.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cicon.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_icon_new
****************************************/

CgUpnpIcon *cg_upnp_icon_new()
{
	CgUpnpIcon *dev;

	cg_log_debug_l4("Entering...\n");

	dev = (CgUpnpIcon *)malloc(sizeof(CgUpnpIcon));

	if  ( NULL != dev )
		cg_list_node_init((CgList *)dev);

	cg_log_debug_l4("Leaving...\n");

	return dev;
}

/****************************************
* cg_upnp_icon_delete
****************************************/

void cg_upnp_icon_delete(CgUpnpIcon *dev)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)dev);
	free(dev);

	cg_log_debug_l4("Leaving...\n");
}
