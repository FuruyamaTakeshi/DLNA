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
*	File: curl.c
*
*	Revision:
*
*	03/16/05
*		- first revision
*
*
*	10/31/05
*		- cg_net_getmodifierhosturl set the tag begin mark "<" before port
*		  while it must come before "http://"
*
*	19-Jan-06 Aapo Makela
*		- Fixed cg_net_gethosturl to call cg_net_getmodifierhosturl
******************************************************************/

#include <cybergarage/net/curi.h>
#include <cybergarage/net/cinterface.h>
#include <cybergarage/util/clog.h>

#include <stdio.h>

/****************************************
* cg_net_getmodifierhosturl
****************************************/

char *cg_net_getmodifierhosturl(char *host, int port, char *uri, char *buf, int bufSize, char *begin, char *end)
{
	BOOL isIPv6Host;
	
	cg_log_debug_l4("Entering...\n");

	isIPv6Host = cg_net_isipv6address(host);
	
#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize,
#else
	sprintf(buf,
#endif
		"%shttp://%s%s%s:%d%s%s",
		begin,
		((isIPv6Host == TRUE) ? "[" : ""),
		host,
		((isIPv6Host == TRUE) ? "]" : ""),
		port,
		uri,
		end);
	
	return buf;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_gethosturl
****************************************/

char *cg_net_gethosturl(char *host, int port, char *uri, char *buf, int bufSize)
{
	cg_log_debug_l4("Entering...\n");

	return cg_net_getmodifierhosturl(host, port, uri, buf, bufSize, "", "");

	cg_log_debug_l4("Leaving...\n");
}
