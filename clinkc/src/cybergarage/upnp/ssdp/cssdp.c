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
*	File: cssdp.c
*
*	Revision:
*
*	02/13/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/ssdp/cssdp.h>
#include <cybergarage/util/cstring.h>
#include <cybergarage/net/cinterface.h>
#include <cybergarage/util/clog.h>

static char *ssdpIPv6Address = CG_UPNP_SSDP_IPV6_LINK_LOCAL_ADDRESS;
static int ssdpAnnounceCount = CG_UPNP_SSDP_DEFAULT_ANNOUNCE_COUNT;

/****************************************
* cg_upnp_ssdp_setipv6address
****************************************/

void cg_upnp_ssdp_setipv6address(char *addr)
{
	cg_log_debug_l4("Entering...\n");

	ssdpIPv6Address = CG_UPNP_SSDP_IPV6_LINK_LOCAL_ADDRESS;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_getipv6address
****************************************/

char *cg_upnp_ssdp_getipv6address()
{
	cg_log_debug_l4("Entering...\n");

	return ssdpIPv6Address;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_setannouncecount
****************************************/

void cg_upnp_ssdp_setannouncecount(int count)
{
	cg_log_debug_l4("Entering...\n");

	ssdpAnnounceCount = count;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_getannouncecount
****************************************/

int cg_upnp_ssdp_getannouncecount()
{
	cg_log_debug_l4("Entering...\n");

	return ssdpAnnounceCount;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_getleasetime
****************************************/

int cg_upnp_ssdp_getleasetime(char *cacheCnt)
{
	int eqIdx;

	cg_log_debug_l4("Entering...\n");

	if (cacheCnt == NULL)
		return 0;
	eqIdx = cg_strchr(cacheCnt, "=", 1);
	if (eqIdx < 0)
		return 0;
	return atoi((cacheCnt + eqIdx + 1));

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_gethostaddress
****************************************/

char *cg_upnp_ssdp_gethostaddress(char *ifAddr)
{
	char *ssdpAddr = CG_UPNP_SSDP_ADDRESS;

	cg_log_debug_l4("Entering...\n");

	if (cg_net_isipv6address(ifAddr) == TRUE)
		ssdpAddr = cg_upnp_ssdp_getipv6address();
	return ssdpAddr;

	cg_log_debug_l4("Leaving...\n");
}
