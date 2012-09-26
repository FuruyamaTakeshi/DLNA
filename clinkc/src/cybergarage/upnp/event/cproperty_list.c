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
*	File: cproperty_list.c
*
*	Revision:
*
*	07/08/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/event/cproperty.h>
#include <cybergarage/util/clog.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* cg_upnp_propertylist_new
****************************************/

CgUpnpPropertyList *cg_upnp_propertylist_new()
{
	CgUpnpPropertyList *propertyList;

	cg_log_debug_l4("Entering...\n");

	propertyList = (CgUpnpPropertyList *)malloc(sizeof(CgUpnpPropertyList));

	if ( NULL != propertyList )
		cg_list_header_init((CgList *)propertyList);

	return propertyList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_propertylist_delete
****************************************/

void cg_upnp_propertylist_delete(CgUpnpPropertyList *propertyList)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_propertylist_clear(propertyList);
	free(propertyList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
