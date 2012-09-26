/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cwebservice.h
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNPAV_WEBSERVICE_H_
#define _CG_UPNPAV_WEBSERVICE_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <cybergarage/util/cstring.h>

BOOL cg_http_getrestresponse(char *url, CgString *str);
BOOL cg_http_getredirecturl(char *orgURL, char *redirectURL, int redirectURLSize);

#ifdef  __cplusplus
}
#endif

#endif
