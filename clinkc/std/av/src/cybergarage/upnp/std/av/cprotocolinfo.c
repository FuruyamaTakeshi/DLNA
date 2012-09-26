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
*	File: cprotocolinfo.c
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
* cg_upnpav_protocolinfo_new
****************************************/

CgUpnpAvProtocolInfo *cg_upnpav_protocolinfo_new()
{
	CgUpnpAvProtocolInfo *info;

	cg_log_debug_l4("Entering...\n");

	info = (CgUpnpAvProtocolInfo *)malloc(sizeof(CgUpnpAvProtocolInfo));

	if  ( NULL != info ) {
		cg_list_node_init((CgList *)info);

		info->protocol = cg_string_new();
		info->network = cg_string_new();
		info->mimeType = cg_string_new();
		info->additionalInfo = cg_string_new();
		info->string = cg_string_new();

		cg_upnpav_protocolinfo_setprotocol(info, "*");
		cg_upnpav_protocolinfo_setnetwork(info, "*");
		cg_upnpav_protocolinfo_setmimetype(info, "*");
		cg_upnpav_protocolinfo_setadditionainfo(info, "*");
    }
	cg_log_debug_l4("Leaving...\n");

	return info;
}

/****************************************
* cg_upnpav_protocolinfo_delete
****************************************/

void cg_upnpav_protocolinfo_delete(CgUpnpAvProtocolInfo *info)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)info);

	if (info->protocol)
		cg_string_delete(info->protocol);
	if (info->network)
		cg_string_delete(info->network);
	if (info->mimeType)
		cg_string_delete(info->mimeType);
	if (info->additionalInfo)
		cg_string_delete(info->additionalInfo);
	if (info->string)
		cg_string_delete(info->string);

	free(info);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnpav_protocolinfo_getstring
****************************************/

char *cg_upnpav_protocolinfo_getstring(CgUpnpAvProtocolInfo *info)
{
	char protocolInfoBuf[CG_UPNPAV_PROTOCOLINFO_MAXSIZE];

#if defined(WIN32)
	_snprintf(
#else
	snprintf(
#endif
			protocolInfoBuf,
			sizeof(protocolInfoBuf),
			"%s:%s:%s:%s",
		cg_upnpav_protocolinfo_getprotocol(info),
		cg_upnpav_protocolinfo_getnetwork(info),
		cg_upnpav_protocolinfo_getmimetype(info),
		cg_upnpav_protocolinfo_getadditionainfo(info));

	cg_string_setvalue(info->string, protocolInfoBuf);

	return cg_string_getvalue(info->string);
}
