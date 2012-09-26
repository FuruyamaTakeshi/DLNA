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
*	File: cprotocolinfo.h
*
*	Revision:
*
*	07/27/09
*		- first revision
*
******************************************************************/

#ifndef _CG_CLINKCAV_PROTOCOLINFO_H_
#define _CG_CLINKCAV_PROTOCOLINFO_H_

#include <cybergarage/util/cstring.h>
#include <cybergarage/util/clist.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Constant
****************************************/

#define CG_UPNPAV_PROTOCOLINFO_MAXSIZE 512
#define CG_UPNPAV_PROTOCOLINFO_HTTPGET "http-get"

/****************************************
* Data Type
****************************************/
/**
 * The generic protocolinfo struct
 */
typedef struct _CgUpnpAvProtocolInfo {
	BOOL headFlag;
	struct _CgUpnpAvProtocolInfo *prev;
	struct _CgUpnpAvProtocolInfo *next;
	CgString *protocol;
	CgString *network;
	CgString *mimeType;
	CgString *additionalInfo;
	CgString *string;
} CgUpnpAvProtocolInfo, CgUpnpAvProtocolInfoList;

/****************************************
* Function (ProtocolInfo)
****************************************/

/**
 * Create a new protocolinfo struct
 */
CgUpnpAvProtocolInfo *cg_upnpav_protocolinfo_new();

/**
 * Delete an protocolinfo struct
 *
 * \param dev ProtocolInfo to delete
 */
void cg_upnpav_protocolinfo_delete(CgUpnpAvProtocolInfo *protocolinfo);

/**
 * Get the next protocolinfo in list. Use as an iterator.
 * 
 * \param protocolinfo Node in a list of protocolinfos
 */
#define cg_upnpav_protocolinfo_next(protocolinfo) (CgUpnpAvProtocolInfo *)cg_list_next((CgList *)protocolinfo)

/*****************************************************************************
 * Protocol
 *****************************************************************************/
/**
 * Set the protocolinfo's protocol type
 * 
 * \param protocolinfo ProtocolInfo in question
 * \param value Protocol
 */
#define cg_upnpav_protocolinfo_setprotocol(protocolinfo, value) cg_string_setvalue(protocolinfo->protocol, value)

/**
 * Get the protocolinfo's MIME type
 * 
 */
#define cg_upnpav_protocolinfo_getprotocol(protocolinfo) cg_string_getvalue(protocolinfo->protocol)

/*****************************************************************************
 * Network
 *****************************************************************************/
/**
 * Set the protocolinfo's network type
 * 
 * \param protocolinfo ProtocolInfo in question
 * \param value Network type
 */
#define cg_upnpav_protocolinfo_setnetwork(protocolinfo, value) cg_string_setvalue(protocolinfo->network, value)

/**
 * Get the protocolinfo's MIME type
 * 
 */
#define cg_upnpav_protocolinfo_getnetwork(protocolinfo) cg_string_getvalue(protocolinfo->network)

/*****************************************************************************
 * mimeType
 *****************************************************************************/
/**
 * Set the protocolinfo's MIME type
 * 
 * \param protocolinfo ProtocolInfo in question
 * \param value MIME type
 */
#define cg_upnpav_protocolinfo_setmimetype(protocolinfo, value) cg_string_setvalue(protocolinfo->mimeType, value)

/**
 * Get the protocolinfo's MIME type
 * 
 */
#define cg_upnpav_protocolinfo_getmimetype(protocolinfo) cg_string_getvalue(protocolinfo->mimeType)

/*****************************************************************************
 * additionalInfo
 *****************************************************************************/
/**
 * Set the protocolinfo's additional info type
 * 
 * \param protocolinfo ProtocolInfo in question
 * \param value Additiona lInfo
 */
#define cg_upnpav_protocolinfo_setadditionainfo(protocolinfo, value) cg_string_setvalue(protocolinfo->additionalInfo, value)

/**
 * Get the protocolinfo's MIME type
 * 
 */
#define cg_upnpav_protocolinfo_getadditionainfo(protocolinfo) cg_string_getvalue(protocolinfo->additionalInfo)

/*****************************************************************************
 * String
 *****************************************************************************/

/**
 * Get the protocolinfo's string
 * 
 */
char *cg_upnpav_protocolinfo_getstring(CgUpnpAvProtocolInfo *protocolinfo);

/*****************************************************************************
 * Function (ProtocolInfoList)
 *****************************************************************************/

/**
 * Create a new list of protocolinfos
 * 
 */
CgUpnpAvProtocolInfoList *cg_upnpav_protocolinfolist_new();

/**
 * Delete a list of protocolinfos
 * 
 * \param protocolinfoList List of protocolinfos
 */
void cg_upnpav_protocolinfolist_delete(CgUpnpAvProtocolInfoList *protocolinfoList);

/**
 * Clear the contents of an protocolinfo list
 * 
 * \param protocolinfoList List of protocolinfos
 */
#define cg_upnpav_protocolinfolist_clear(protocolinfoList) cg_list_clear((CgList *)protocolinfoList, (CG_LIST_DESTRUCTORFUNC)cg_upnpav_protocolinfo_delete)

/**
 * Get the size (number of elements) of an protocolinfo list
 * 
 * \param protocolinfoList List of protocolinfos
 */
#define cg_upnpav_protocolinfolist_size(protocolinfoList) cg_list_size((CgList *)protocolinfoList)

/**
 * \todo Get the first protocolinfo from the list for iteration
 * 
 * \param protocolinfoList List of protocolinfos
 */
#define cg_upnpav_protocolinfolist_gets(protocolinfoList) (CgUpnpAvProtocolInfo *)cg_list_next((CgList *)protocolinfoList)

/**
 * Add an protocolinfo to an protocolinfo list
 * 
 * \param protocolinfoList List of protocolinfos
 * \param protocolinfo ProtocolInfo to add
 */
#define cg_upnpav_protocolinfolist_add(protocolinfoList, protocolinfo) cg_list_add((CgList *)protocolinfoList, (CgList *)protocolinfo)

#ifdef  __cplusplus

} /* extern "C" */

#endif

#endif
