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
*	File: cinterface.cpp
*
*	Revision:
*
*	02/09/05
*		- first revision
*	02/09/05
*		- Added cg_net_interface_setnetmask() and cg_net_interface_getnetmask().
*	09/12/07
*		- Added the following functions to get MAC address.
*		  cg_net_interface_setmacaddress(), cg_net_interface_getmacaddress()
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/net/cinterface.h>
#include <cybergarage/util/clog.h>
#include <string.h>

/****************************************
* cg_net_interface_new
****************************************/

CgNetworkInterface *cg_net_interface_new()
{
	CgNetworkInterface *netIf;

	cg_log_debug_l4("Entering...\n");

	netIf = (CgNetworkInterface *)malloc(sizeof(CgNetworkInterface));

	if ( NULL != netIf )
	{
		cg_list_node_init((CgList *)netIf);
		netIf->name = cg_string_new();
		netIf->ipaddr = cg_string_new();
		netIf->netmask = cg_string_new();
		cg_net_interface_setindex(netIf, 0);
		memset(netIf->macaddr, 0, (size_t)CG_NET_MACADDR_SIZE);
	}
	
	cg_log_debug_l4("Leaving...\n");

	return netIf;
}

/****************************************
* cg_net_interface_delete
****************************************/

void cg_net_interface_delete(CgNetworkInterface *netIf)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)netIf);
	cg_string_delete(netIf->name);
	cg_string_delete(netIf->ipaddr);
	cg_string_delete(netIf->netmask);

	cg_log_debug_l4("Leaving...\n");

	free(netIf);
}

/****************************************
* cg_net_interface_getany
****************************************/

CgNetworkInterface *cg_net_interface_getany()
{
	CgNetworkInterface *netIf;

	cg_log_debug_l4("Entering...\n");

	netIf = cg_net_interface_new();
	/* It would be more approriate to use INADDR_ANY, but this will do */
	cg_net_interface_setname(netIf, "INADDR_ANY");
	cg_net_interface_setaddress(netIf, "0.0.0.0");
	
	cg_log_debug_l4("Leaving...\n");

	return netIf;
}

/****************************************
* cg_net_interface_setname
****************************************/

void cg_net_interface_setname(CgNetworkInterface *netIf, char *name)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(netIf->name, name);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_interface_getname
****************************************/

char *cg_net_interface_getname(CgNetworkInterface *netIf)
{
	cg_log_debug_l4("Entering...\n");

	return cg_string_getvalue(netIf->name);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_interface_setaddress
****************************************/

void cg_net_interface_setaddress(CgNetworkInterface *netIf, char *value)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(netIf->ipaddr, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_interface_getaddress
****************************************/

char *cg_net_interface_getaddress(CgNetworkInterface *netIf)
{
	cg_log_debug_l4("Entering...\n");

	return cg_string_getvalue(netIf->ipaddr);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_interface_setnetmask
****************************************/

void cg_net_interface_setnetmask(CgNetworkInterface *netIf, char *value)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(netIf->netmask, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_interface_getnetmask
****************************************/

char *cg_net_interface_getnetmask(CgNetworkInterface *netIf)
{
	cg_log_debug_l4("Entering...\n");

	return cg_string_getvalue(netIf->netmask);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_interface_cmp
****************************************/

int cg_net_interface_cmp(CgNetworkInterface *netIfA, 
			 CgNetworkInterface *netIfB)
{
	cg_log_debug_l4("Entering...\n");

	if (netIfA == NULL && netIfB == NULL) return 0;
	if (netIfA == NULL && netIfB != NULL) return 1;
	if (netIfA != NULL && netIfB == NULL) return -1;
	
	return cg_strcmp(cg_net_interface_getaddress(netIfA), 
			 cg_net_interface_getaddress(netIfB));

	cg_log_debug_l4("Leaving...\n");
}
