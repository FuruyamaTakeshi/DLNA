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
*	File: cupnp_function.c
*
*	Revision:
*
*	02/21/05
*		- first revision
*	10/30/05
*		- Thanks for Makela Aapo (aapo.makela@nokia.com)
*		- Changed to cg_upnp_createuuid() create new UUIDs as they are specified in 
*		  UPnP DA specification (UUIDs have "uuid:"prefix).
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/typedef.h>

#if defined(HAVE_UUID_UUID_H) || defined(HAVE_LIBUUID) || defined(TARGET_OS_IPHONE)
#include <uuid/uuid.h>
#endif

#include <cybergarage/upnp/cupnp.h>
#include <cybergarage/util/clog.h>

#include <time.h>
#include <stdlib.h>

/****************************************
* Static
****************************************/

static BOOL isUpnpNMPRMode = FALSE;

/****************************************
* cg_upnp_createuuid
****************************************/

char *cg_upnp_createuuid(char *uuidBuf, int uuidBufSize)
{
#if defined(HAVE_LIBUUID) || defined(TARGET_OS_IPHONE)
	uuid_t uuid;
	char uuidStr[CG_UPNP_UUID_MAX_LEN];
#elif defined(WIN32)
	#pragma comment(lib, "Rpcrt4.lib")
	UUID uuid; 
	unsigned char* szUuid = NULL;
#else
	time_t time1;
	time_t time2;
#endif
	
	cg_log_debug_l4("Entering...\n");

#if defined(HAVE_LIBUUID) || defined(TARGET_OS_IPHONE)
    uuid_generate(uuid);
	uuid_unparse_lower(uuid, uuidStr);
	snprintf(uuidBuf,uuidBufSize, "uuid:%s",uuidStr);
#elif defined(WIN32)
	UuidCreate(&uuid);
	UuidToString(&uuid, &szUuid);
	sprintf(uuidBuf, "%s:%s",
		CG_UPNP_UUID_NAME,
		szUuid);
	RpcStringFree(&szUuid);
#else
	/**** Thanks for Makela Aapo (10/30/05) ****/
	time1 = cg_getcurrentsystemtime();
	time2 = (time_t)((double)cg_getcurrentsystemtime(NULL) * ((double)rand() / (double)RAND_MAX));
	snprintf(uuidBuf, uuidBufSize, "%s:%04x-%04x-%04x-%04x",
		CG_UPNP_UUID_NAME,
		(int)(time1 & 0xFFFF),
		(int)(((time1 >> 31) | 0xA000) & 0xFFFF),
		(int)(time2 & 0xFFFF),
		(int)(((time2 >> 31) | 0xE000) & 0xFFFF));
#endif
		
	cg_log_debug_l4("Leaving...\n");

	return uuidBuf;
}

/****************************************
* cg_upnp_getservername
****************************************/

char *cg_upnp_getservername(char *buf, int bufSize)
{
	int nameLen;
	
	cg_log_debug_l4("Entering...\n");

	cg_http_getservername(buf, bufSize);
	nameLen = cg_strlen(buf);
	bufSize -= nameLen;
	if (bufSize <= 0)
		return buf;
#if defined(HAVE_SNPRINTF)
	snprintf((buf+nameLen), bufSize, " %s/%s UPnP/%s DLNADOC/%s", CG_CLINK_NAME, CG_CLINK_VER, CG_UPNP_VER, CG_DLNA_VER);
#else
	sprintf((buf+nameLen), " %s/%s UPnP/%s DLNADOC/%s", CG_CLINK_NAME, CG_CLINK_VER, CG_UPNP_VER, CG_DLNA_VER);
#endif
	
	cg_log_debug_l4("Leaving...\n");

	return buf;
}

/****************************************
* cg_upnp_setnmprmode
****************************************/

void cg_upnp_setnmprmode(BOOL onFlag)
{
	cg_log_debug_l4("Entering...\n");

	isUpnpNMPRMode = onFlag;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_isnmprmode
****************************************/

BOOL cg_upnp_isnmprmode()
{
	cg_log_debug_l4("Entering...\n");

	return isUpnpNMPRMode;

	cg_log_debug_l4("Leaving...\n");
}

