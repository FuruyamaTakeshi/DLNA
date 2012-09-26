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
*	File: cproperty.c
*
*	Revision:
*
*	02/01/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/event/cevent.h>
#include <cybergarage/upnp/event/cnotify.h>
#include <cybergarage/upnp/event/cproperty.h>
#include <cybergarage/util/clog.h>

#include <time.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* cg_upnp_property_new
****************************************/

CgUpnpProperty *cg_upnp_property_new()
{
	CgUpnpProperty *prop;

	cg_log_debug_l4("Entering...\n");

	prop = (CgUpnpProperty *)malloc(sizeof(CgUpnpProperty));

	if ( NULL != prop )
	{
		cg_list_node_init((CgList *)prop);
		
		prop->name = cg_string_new();
		prop->value = cg_string_new();
		prop->sid = cg_string_new();
		
		cg_upnp_property_setseq(prop, 0);
	}
	
	return prop;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_property_delete
****************************************/

void cg_upnp_property_delete(CgUpnpProperty *prop)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_property_clear(prop);
	cg_list_remove((CgList *)prop);

	cg_string_delete(prop->name);
	cg_string_delete(prop->value);
	cg_string_delete(prop->sid);
	
	free(prop);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_property_clear
****************************************/

void cg_upnp_property_clear(CgUpnpProperty *prop)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(prop->name, NULL);
	cg_string_setvalue(prop->value, NULL);
	cg_string_setvalue(prop->sid, NULL);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
