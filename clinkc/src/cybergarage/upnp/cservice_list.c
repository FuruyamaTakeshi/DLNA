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
*	File: cservice_list.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cservice.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_servicelist_new
****************************************/

CgUpnpServiceList *cg_upnp_servicelist_new()
{
	CgUpnpServiceList *serviceList;

	cg_log_debug_l4("Entering...\n");

	serviceList = (CgUpnpServiceList *)malloc(sizeof(CgUpnpServiceList));

	if ( NULL != serviceList )
		cg_list_header_init((CgList *)serviceList);

	return serviceList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_servicelist_delete
****************************************/

void cg_upnp_servicelist_delete(CgUpnpServiceList *serviceList)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_servicelist_clear(serviceList);
	free(serviceList);

	cg_log_debug_l4("Leaving...\n");
}
