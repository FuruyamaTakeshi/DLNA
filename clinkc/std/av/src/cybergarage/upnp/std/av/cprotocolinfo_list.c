/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2009
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cprotocolinfolist.c
*
*	Revision:
*
*	07/27/09
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/std/av/cprotocolinfo.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_protocolinflist_new
****************************************/

CgUpnpAvProtocolInfoList *cg_upnpav_protocolinfolist_new()
{
	CgUpnpAvProtocolInfo *protocolinfoList;

	cg_log_debug_l4("Entering...\n");

	protocolinfoList = (CgUpnpAvProtocolInfo *)malloc(sizeof(CgUpnpAvProtocolInfo));

	if ( NULL != protocolinfoList )
		cg_list_header_init((CgList *)protocolinfoList);

	return protocolinfoList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnpav_protocolinfor_delete
****************************************/

void cg_upnpav_protocolinfolist_delete(CgUpnpAvProtocolInfoList *protocolinfoList)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)protocolinfoList);
	free(protocolinfoList);

	cg_log_debug_l4("Leaving...\n");
}
