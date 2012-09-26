/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cresource_data.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include <cybergarage/upnp/std/av/cresource.h>
#include <cybergarage/util/cstring.h>

/****************************************
* cg_upnpav_resource_data_new
****************************************/

CgUpnpAvResourceData *cg_upnpav_resource_data_new()
{
	CgUpnpAvResourceData *resData;
	
	resData = (CgUpnpAvResourceData *)malloc(sizeof(CgUpnpAvResourceData));
	resData->mimeType = cg_string_new();
	resData->dlnaAttr = cg_string_new();

	return resData;
}

/****************************************
* cg_upnpav_resource_data_delete
****************************************/

void cg_upnpav_resource_data_delete(CgUpnpAvResourceData *resData)
{
	cg_string_delete(resData->mimeType);
	cg_string_delete(resData->dlnaAttr);
	free(resData);
}

/****************************************
* cg_upnpav_resource_data_copy
****************************************/

void cg_upnpav_resource_data_copy(CgUpnpAvResourceData *destResData, CgUpnpAvResourceData *srcResData)
{
	cg_string_setvalue(destResData->mimeType, cg_string_getvalue(srcResData->mimeType));
}
