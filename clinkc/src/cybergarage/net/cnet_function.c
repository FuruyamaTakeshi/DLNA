/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cnet_function.c
*
*	Revision:
*
*	02/09/05
*		- first revision
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/net/cinterface.h>
#include <cybergarage/util/clog.h>

/****************************************
* global variables
****************************************/

static BOOL CG_NET_USE_ONLY_IPV4_ADDR = FALSE;
static BOOL CG_NET_USE_ONLY_IPV6_ADDR = FALSE;

////////////////////////////////////////////////
//	cg_net_isuseaddress
////////////////////////////////////////////////

BOOL cg_net_isuseaddress(char *addr)
{
	cg_log_debug_l4("Entering...\n");

	if (CG_NET_USE_ONLY_IPV6_ADDR == TRUE) {
		if (cg_net_isipv6address(addr) == FALSE)
			return FALSE;
	}
	if (CG_NET_USE_ONLY_IPV4_ADDR == TRUE) {
		if (cg_net_isipv6address(addr) == TRUE)
			return FALSE;
	}
	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

////////////////////////////////////////////////
//	cg_net_getipv6address
////////////////////////////////////////////////

BOOL cg_net_isipv6address(char *addr)
{
	cg_log_debug_l4("Entering...\n");

	if (addr == NULL)
		return FALSE;
	if (0 < cg_strchr(addr, ":", 1))
		return TRUE;
	return FALSE;

	cg_log_debug_l4("Leaving...\n");
}

////////////////////////////////////////////////
//	cg_net_getipv6address
////////////////////////////////////////////////

int cg_net_getipv6scopeid(char *addr)
{
	int addrLen;
	int perIdx;
	char scopeIDBuf[8+1];

	cg_log_debug_l4("Entering...\n");

	if (cg_net_isipv6address(addr) == FALSE)
		return 0;
	addrLen = cg_strlen(addr);
	perIdx = cg_strchr(addr, "%", 1);
	if (perIdx < 0)
		return 0;
	cg_strncpy(scopeIDBuf, (addr+perIdx+1), sizeof(scopeIDBuf)-1);
	scopeIDBuf[sizeof(scopeIDBuf)-1] = '\0';
	
	cg_log_debug_l4("Leaving...\n");
	
	return atoi(scopeIDBuf);

}

/*
////////////////////////////////////////////////
//	GetHostURL
////////////////////////////////////////////////

const char *CyberNet::GetHostURL(const char *host, int port, const char *uri, std::string &buf)
{
	cg_log_debug_l4("Entering...\n");

	std::string hostStr = host;
	if (IsIPv6Address(host) == true) {
		StripIPv6ScopeID(host, hostStr);
		hostStr = "[";
		hostStr += hostStr.c_str();
		hostStr += "]";
	}
#ifndef NO_USE_OSTRINGSTREAM
	std::ostringstream sbuf;
	sbuf << "http://" << hostStr << ":" << port << uri;
	buf = sbuf.str();
#else
	buf = "http://";
	buf += hostStr;
	buf += ":";
	buf += port;
	buf += uri;
#endif

	cg_log_debug_l4("Leaving...\n");

	return buf.c_str();
}

////////////////////////////////////////////////
//	GetHostURL
////////////////////////////////////////////////

static string ifAddress;

void CyberNet::SetHostInterface(const char *ifaddr)
{
	cg_log_debug_l4("Entering...\n");

	ifAddress = ifaddr;

	cg_log_debug_l4("Leaving...\n");
}

const char *CyberNet::GetHostInterface()
{
	cg_log_debug_l4("Entering...\n");

	cg_log_debug_l4("Leaving...\n");

	return ifAddress.c_str();
}

bool CyberNet::HasAssignedHostInterface()
{
	cg_log_debug_l4("Entering...\n");

	cg_log_debug_l4("Leaving...\n");

	return (0 < ifAddress.length()) ? true : false;
}
*/
