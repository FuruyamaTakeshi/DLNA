/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cresource.h
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifndef _CG_CLINKCAV_RESOURCE_H_
#define _CG_CLINKCAV_RESOURCE_H_

#include <cybergarage/util/clist.h>
#include <cybergarage/xml/cxml.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Constant
****************************************/

#define CG_UPNPAV_RESOURCE_NAME "res"
#define CG_UPNPAV_RESOURCE_PROTOCOLINFO "protocolInfo"
#define CG_UPNPAV_RESOURCE_PROTOCOLINFO_SIZE "size"
#define CG_UPNPAV_RESOURCE_PROTOCOLINFO_DURATION "duration"
#define CG_UPNPAV_RESOURCE_PROTOCOLINFO_RESOLUTION "resolution"

/****************************************
* Struct
****************************************/

typedef CgXmlNode CgUpnpAvResource;
typedef CgXmlNodeList CgUpnpAvResourceList;

typedef struct _CgUpnpAvResourceData {
	CgString *mimeType;
	CgString *dlnaAttr;
} CgUpnpAvResourceData;

/****************************************
* Resource
****************************************/

#define cg_upnpav_resourcelist_new() cg_xml_nodelist_new()
#define cg_upnpav_resourcelist_delete(resList) cg_xml_nodelist_delete(resList)
#define cg_upnpav_resourcelist_clear(resList) cg_xml_nodelist_clear(resList)
#define cg_upnpav_resourcelist_size(resList) cg_xml_nodelist_size(resList)
#define cg_upnpav_resourcelist_gets(resList) cg_xml_nodelist_gets(resList)
#define cg_upnpav_resourcelist_add(resList, res) cg_xml_nodelist_add(resList, res)

CgUpnpAvResource *cg_upnpav_resource_new();
void cg_upnpav_resource_delete(CgUpnpAvResource *res);
#define cg_upnpav_resource_next(res) cg_xml_node_next(res)
#define cg_upnpav_resource_remove(res) cg_xml_node_remove(res)

void cg_upnpav_resource_copy(CgUpnpAvResource *destRes, CgUpnpAvResource *srcRes);

void cg_upnpav_resource_setmimetype(CgUpnpAvResource *res, char *mimeType);
void cg_upnpav_resource_setdlnaattribute(CgUpnpAvResource *res, char *attr);
#define cg_upnpav_resource_seturl(res, val) cg_xml_node_setvalue(res, val)
void cg_upnpav_resource_setsize(CgUpnpAvResource *res, long value);

#define cg_upnpav_resource_getmimetype(res) cg_string_getvalue(((CgUpnpAvResourceData *)(cg_xml_node_getuserdata(res)))->mimeType)
#define cg_upnpav_resource_getdlnaattribute(res) cg_string_getvalue(((CgUpnpAvResourceData *)(cg_xml_node_getuserdata(res)))->dlnaAttr)
#define cg_upnpav_resource_geturl(res) cg_xml_node_getvalue(res)
#define cg_upnpav_resource_getsize(res) cg_str2long(cg_xml_node_getattributevalue(res, CG_UPNPAV_RESOURCE_PROTOCOLINFO_SIZE))
#define cg_upnpav_resource_getduration(res) cg_xml_node_getattributevalue(res, CG_UPNPAV_RESOURCE_PROTOCOLINFO_DURATION)
#define cg_upnpav_resource_getresolution(res) cg_xml_node_getattributevalue(res, CG_UPNPAV_RESOURCE_PROTOCOLINFO_RESOLUTION)

CgUpnpAvResourceData *cg_upnpav_resource_data_new();
void cg_upnpav_resource_data_delete(CgUpnpAvResourceData *resData);
void cg_upnpav_resource_data_copy(CgUpnpAvResourceData *destResData, CgUpnpAvResourceData *srcResData);

char *cg_upnpav_resource_getdlnaattributesbymimetype(char *mimeType, char *dlnaAttr, int dlnaAttrSize);
char *cg_upnpav_resource_getdlnaattributes(CgUpnpAvResource *res, char *dlnaAttr, int dlnaAttrSize);

#ifdef __cplusplus
}
#endif

#endif
