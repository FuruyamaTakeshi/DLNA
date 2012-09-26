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
*	File: caction_list.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/caction.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_action_new
****************************************/

CgUpnpActionList *cg_upnp_actionlist_new()
{
	CgUpnpAction *actionList;

	cg_log_debug_l4("Entering...\n");

	actionList = (CgUpnpAction *)malloc(sizeof(CgUpnpAction));

	if ( NULL != actionList )
		cg_list_header_init((CgList *)actionList);

	return actionList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_action_delete
****************************************/

void cg_upnp_actionlist_delete(CgUpnpActionList *actionList)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)actionList);
	free(actionList);

	cg_log_debug_l4("Leaving...\n");
}
