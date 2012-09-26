/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: ccontent.h
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifndef _CG_CLINKCAV_CONTENT_H_
#define _CG_CLINKCAV_CONTENT_H_

#include <cybergarage/upnp/std/av/cresource.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Constant
****************************************/

#define CG_UPNPAV_CONTENT_NONE (0)
#define CG_UPNPAV_CONTENT_CONTAINER (1)
#define CG_UPNPAV_CONTENT_ITEM (2)

#define CG_UPNPAV_CONTENT_ID "id"
#define CG_UPNPAV_CONTENT_PARENTID "parentID"
#define CG_UPNPAV_CONTENT_CHILDCOUNT "childCount"
#define CG_UPNPAV_CONTENT_RESTRICTED "restricted"
#define CG_UPNPAV_CONTENT_TITLE "dc:title"
#define CG_UPNPAV_CONTENT_UPNPCLASS "upnp:class"
#define CG_UPNPAV_CONTENT_UPNPALBUMARTURI "upnp:albumArtURI"

#define CG_UPNPAV_ROOT_CONTENT_TITLE "ROOT"
#define CG_UPNPAV_ROOT_CONTENT_ID "0"
#define CG_UPNPAV_ROOT_CONTENT_PARENTID "-1"

#define CG_UPNPAV_MIMETYPE_JPEG "image/jpeg"
#define CG_UPNPAV_MIMETYPE_PNG "image/png"
#define CG_UPNPAV_MIMETYPE_MPEG "video/mpeg"
#define CG_UPNPAV_MIMETYPE_MP3 "audio/mpeg"

#define CG_UPNPAV_UPNPCLASS_CONTAINER "object.container"
#define CG_UPNPAV_UPNPCLASS_ITEM "object.item"
#define CG_UPNPAV_UPNPCLASS_PHOTO "object.item.imageItem.photo"
#define CG_UPNPAV_UPNPCLASS_MOVIE "object.item.videoItem.movie"
#define CG_UPNPAV_UPNPCLASS_MUSIC "object.item.audioItem.musicTrack"

#define CG_UPNPAV_DLNAATTR_MAXLEN 128
#define CG_UPNPAV_PROTOCOLINFO_MAXLEN (CG_UPNPAV_DLNAATTR_MAXLEN + 64)
#define CG_UPNPAV_DLNA_PN_JPEG_LRG "JPEG_LRG"
#define CG_UPNPAV_DLNA_PN_MPEG_PS_NTSC "MPEG_PS_NTSC"
#define CG_UPNPAV_DLNA_PN_MP3 "MP3"

/****************************************
* Struct
****************************************/

typedef CgXmlNode CgUpnpAvContent;
typedef CgXmlNodeList CgUpnpAvContentList;

/****************************************
* Content
****************************************/

#define cg_upnpav_contentlist_new() cg_xml_nodelist_new()
#define cg_upnpav_contentlist_delete(conList) cg_xml_nodelist_delete(conList)
#define cg_upnpav_contentlist_clear(conList) cg_xml_nodelist_clear(conList)
#define cg_upnpav_contentlist_size(conList) cg_xml_nodelist_size(conList)
#define cg_upnpav_contentlist_gets(conList) cg_xml_nodelist_gets(conList)
#define cg_upnpav_contentlist_add(conList, res) cg_xml_nodelist_add(conList, res)

#define cg_upnpav_content_new() cg_xml_node_new()
#define cg_upnpav_content_delete(con) cg_xml_node_delete(con)
CgUpnpAvContent *cg_upnpav_content_next(CgUpnpAvContent *con);
#define cg_upnpav_content_remove(con) cg_xml_node_remove(con)

void cg_upnpav_content_copy(CgUpnpAvContent *destContent, CgUpnpAvContent *srcContent);

void cg_upnpav_content_addchildcontent(CgUpnpAvContent *con, CgUpnpAvContent *childCon);
int cg_upnpav_content_getnchildcontents(CgUpnpAvContent *con);
CgUpnpAvContent *cg_upnpav_content_getchildcontents(CgUpnpAvContent *con);
BOOL cg_upnpav_content_haschildcontents(CgUpnpAvContent *con);
#define cg_upnpav_content_clearchildcontents(con) cg_xml_nodelist_clear(cg_xml_node_getchildnodelist(con))

#define cg_upnpav_content_setid(con, val) cg_xml_node_setattribute(con, CG_UPNPAV_CONTENT_ID, val)
#define cg_upnpav_content_getid(con) cg_xml_node_getattributevalue(con, CG_UPNPAV_CONTENT_ID)

#define cg_upnpav_content_setparentid(con, val) cg_xml_node_setattribute(con, CG_UPNPAV_CONTENT_PARENTID, val)
#define cg_upnpav_content_getparentid(con) cg_xml_node_getattributevalue(con, CG_UPNPAV_CONTENT_PARENTID)

#define cg_upnpav_content_settitle(con, val) cg_xml_node_setchildnode(con, CG_UPNPAV_CONTENT_TITLE, val)
#define cg_upnpav_content_gettitle(con) cg_xml_node_getchildnodevalue(con, CG_UPNPAV_CONTENT_TITLE)

#define cg_upnpav_content_setupnpclass(con, val) cg_xml_node_setchildnode(con, CG_UPNPAV_CONTENT_UPNPCLASS, val)
#define cg_upnpav_content_getupnpclass(con) cg_xml_node_getchildnodevalue(con, CG_UPNPAV_CONTENT_UPNPCLASS)

#define cg_upnpav_content_setupnpalbumarturi(con, val) cg_xml_node_setchildnode(con, CG_UPNPAV_CONTENT_UPNPALBUMARTURI, val)
#define cg_upnpav_content_getupnpalbumarturi(con) cg_xml_node_getchildnodevalue(con, CG_UPNPAV_CONTENT_UPNPALBUMARTURI)

void cg_upnpav_content_settype(CgUpnpAvContent *con, int type);
int cg_upnpav_content_gettype(CgUpnpAvContent *con);

#define cg_upnpav_content_isitemnode(node) (cg_upnpav_content_gettype(node) == CG_UPNPAV_CONTENT_ITEM ? TRUE : FALSE)
#define cg_upnpav_content_iscontainernode(node) (cg_upnpav_content_gettype(node) == CG_UPNPAV_CONTENT_CONTAINER ? TRUE : FALSE)
BOOL cg_upnpav_content_iscontentnode(CgXmlNode *node);

#define cg_upnpav_content_setuserdata(con, data) cg_xml_node_setuserdata(con, data)
#define cg_upnpav_content_getuserdata(con) cg_xml_node_getuserdata(con)

typedef BOOL (*CG_UPNPAV_CONTENT_COMPARE_FUNC)(CgUpnpAvContent *con, void *userData);
CgUpnpAvContent *cg_upnpav_content_getbyid(CgUpnpAvContent *con, char *id);
CgUpnpAvContent *cg_upnpav_content_getbytitle(CgUpnpAvContent *con, char *title);

#define cg_upnpav_content_addresource(con, res) cg_xml_node_addchildnode(con, res)
#define cg_upnpav_content_getresources(con) cg_xml_node_getchildnode(con, CG_UPNPAV_RESOURCE_NAME)

#ifdef __cplusplus
}
#endif

#endif
