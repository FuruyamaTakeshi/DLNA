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
*	File: cinterface_list.cpp
*
*	Revision:
*
*	02/09/05
*		- first revision
*
******************************************************************/

#include <cybergarage/util/clist.h>
#include <cybergarage/net/cinterface.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_net_interfacelist_new
****************************************/

CgNetworkInterfaceList *cg_net_interfacelist_new()
{
	CgNetworkInterfaceList *netIfList;

	cg_log_debug_l4("Entering...\n");

	netIfList = (CgNetworkInterfaceList *)malloc(sizeof(CgNetworkInterfaceList));

	if ( NULL != netIfList )
	{
		cg_list_header_init((CgList *)netIfList);
		netIfList->name = NULL;
		netIfList->ipaddr = NULL;
	}

	return netIfList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_interfacelist_delete
****************************************/

void cg_net_interfacelist_delete(CgNetworkInterfaceList *netIfList)
{
	cg_log_debug_l4("Entering...\n");

	cg_net_interfacelist_clear(netIfList);
	free(netIfList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_interfacelist_getinterface
****************************************/

CgNetworkInterface *cg_net_interfacelist_get(CgNetworkInterfaceList *netIfList, char *name)
{
	CgNetworkInterface *netIf;
	char *ifName;
		
	cg_log_debug_l4("Entering...\n");

	if (name == NULL)
		return NULL;
		
	for (netIf = cg_net_interfacelist_gets(netIfList); netIf != NULL; netIf = cg_net_interface_next(netIf)) {
		ifName = cg_net_interface_getname(netIf);
		if (ifName == NULL)
			continue;
		if (cg_strcasecmp(ifName, name) == 0)
			return netIf;
	}
	
	return NULL;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_interfacelist_getchanges
****************************************/

void cg_net_interfacelist_getchanges(CgNetworkInterfaceList *netIfListOld,
				     CgNetworkInterfaceList *netIfListNew,
				     CgNetworkInterfaceList *netIfListAdded,
				     CgNetworkInterfaceList *netIfListRemoved)
{
	CgNetworkInterface *netIfOld, *netIfNew, *tmp;
	BOOL found;
	
	cg_log_debug_l4("Entering...\n");

	/* Browse through old interfaces and check, if they are in the new */
	tmp = cg_net_interfacelist_gets(netIfListOld);
	while (tmp != NULL)
	{
		netIfOld = tmp; tmp = cg_net_interface_next(netIfOld);
		
		found = FALSE;
		for (netIfNew = cg_net_interfacelist_gets(netIfListNew); netIfNew != NULL;
		     netIfNew = cg_net_interface_next(netIfNew))
		{
			if (cg_net_interface_cmp(netIfOld, netIfNew) == 0)
			{
				found = TRUE;
				break;
			}
		}
		
		/* Old interface was not found in new ones, so it's removed */
		if (found == FALSE)
		{
			cg_net_interface_remove(netIfOld);
			if (netIfListRemoved != NULL)
				cg_net_interfacelist_add(netIfListRemoved, 
							 netIfOld);
		}
	}
	
	/* Browse through new interfaces and check, if they are in the 
	   remaining old interfaces */
	tmp = cg_net_interfacelist_gets(netIfListNew);
	while (tmp != NULL)
	{
		netIfNew = tmp; tmp = cg_net_interface_next(netIfNew);
		
		found = FALSE;
		for (netIfOld = cg_net_interfacelist_gets(netIfListOld); netIfOld != NULL;
		     netIfOld = cg_net_interface_next(netIfOld))
		{
			if (cg_net_interface_cmp(netIfOld, netIfNew) == 0)
			{
				found = TRUE;
				break;
			}
		}
		
		/* New interface was not found in old ones, so it's added */
		if (found == FALSE)
		{
			cg_net_interface_remove(netIfNew);
			if (netIfListAdded != NULL)
				cg_net_interfacelist_add(netIfListAdded, 
							 netIfNew);
		}
	}

	cg_log_debug_l4("Leaving...\n");
}
