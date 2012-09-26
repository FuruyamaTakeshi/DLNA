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
*	File: cdevice_list.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_devicelist_new
****************************************/

CgUpnpDeviceList *cg_upnp_devicelist_new()
{
	CgUpnpDeviceList *devList;

	cg_log_debug_l4("Entering...\n");

	devList = (CgUpnpDeviceList *)malloc(sizeof(CgUpnpDeviceList));

	if ( NULL != devList )
		cg_list_header_init((CgList *)devList);

	return devList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_devicelist_delete
****************************************/

void cg_upnp_devicelist_delete(CgUpnpDeviceList *devList)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_devicelist_clear(devList);
	free(devList);

	cg_log_debug_l4("Leaving...\n");
}
