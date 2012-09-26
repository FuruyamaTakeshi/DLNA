/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdms_filesys.h
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifndef _CG_CLINKC_MEDIASERVER_FILESYS_H_
#define _CG_CLINKC_MEDIASERVER_FILESYS_H_

#include <cybergarage/upnp/std/av/cmediaserver.h>

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct _CgUpnpMediaFileSystemContentData {
	char *pubdir;
} CgUpnpMediaFileSystemContentData;

#define  CG_UPNP_MEDIA_FILESYS_RESURL_PATH "content"
#define  CG_UPNP_MEDIA_FILESYS_RESURL_MAXLEN (CG_NET_IPV6_ADDRSTRING_MAXSIZE + CG_MD5_STRING_BUF_SIZE + 64)

CgUpnpMediaServer *cg_upnp_dms_filesys_new();
void cg_upnp_dms_filesys_delete(CgUpnpMediaServer *dms);

#define cg_upnp_dms_filesys_start(dms) cg_upnp_dms_start(dms)
#define cg_upnp_dms_filesys_stop(dms) cg_upnp_dms_stop(dms)

void cg_upnp_dms_filesys_setpublicationdirectory(CgUpnpMediaServer *dms, char *pubdir);
char *cg_upnp_dms_filesys_getpublicationdirectory(CgUpnpMediaServer *dms);

CgUpnpMediaFileSystemContentData *cg_upnp_dms_filesys_content_data_new();
void cg_upnp_dms_filesys_content_data_delete(CgUpnpMediaFileSystemContentData *data);
CgUpnpMediaFileSystemContentData *cg_upnp_dms_filesys_content_getdata(CgUpnpMediaContent *con);
void cg_upnp_dms_filesys_content_setpubicdirectory(CgUpnpMediaContent *con, char *dir);
char *cg_upnp_dms_filesys_content_getpubicdirectory(CgUpnpMediaContent *con);

void cg_upnp_dms_filesys_http_listener(CgHttpRequest *httpReq);

#ifdef  __cplusplus
}
#endif

#endif
