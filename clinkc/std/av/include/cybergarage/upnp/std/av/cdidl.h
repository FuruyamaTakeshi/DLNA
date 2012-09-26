/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2004-2006
*
*	File: cdlna.h
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifndef _CG_CLINKCAV_DIDL_H_
#define _CG_CLINKCAV_DIDL_H_

#include <cybergarage/xml/cxml.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Function
****************************************/

CgXmlNode *cg_upnpav_didl_node_new();

#define cg_upnpav_didl_node_delete(node) cg_xml_node_delete(node)
#define cg_upnpav_didl_node_tostring(node, str) cg_xml_node_tostring(node, TRUE, str)

#ifdef __cplusplus
}
#endif

#endif
