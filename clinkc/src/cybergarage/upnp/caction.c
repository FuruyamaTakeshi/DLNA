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
*	File: caction.c
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
* prototype define for static functions
****************************************/

static void cg_upnp_action_initchildnodes(CgUpnpAction *action);
static void cg_upnp_action_initargumentlist(CgUpnpAction *action);

/****************************************
* cg_upnp_action_new
****************************************/

CgUpnpAction *cg_upnp_action_new()
{
	CgUpnpAction *action;

	cg_log_debug_l4("Entering...\n");

	action = (CgUpnpAction *)malloc(sizeof(CgUpnpAction));

	if ( NULL != action )
	{
		cg_list_node_init((CgList *)action);
		
		action->parentService = NULL;
		action->actionNode = NULL;

		action->argumentList = cg_upnp_argumentlist_new();
		
		action->upnpStatus = cg_upnp_status_new();
		action->listener = NULL;
		cg_upnp_action_setuserdata(action, NULL);
	}

	cg_log_debug_l4("Leaving...\n");

	return action;
}

/****************************************
* cg_upnp_action_delete
****************************************/

void cg_upnp_action_delete(CgUpnpAction *action)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_argumentlist_delete(action->argumentList);
	
	cg_upnp_status_delete(action->upnpStatus);
	
	cg_list_remove((CgList *)action);
	free(action);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_action_setactionnode
****************************************/

void cg_upnp_action_setactionnode(CgUpnpAction *action, CgXmlNode *node)
{
	cg_log_debug_l4("Entering...\n");

	action->actionNode = node;
	cg_upnp_action_initchildnodes(action);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
*
* Child Node
*
****************************************/

/****************************************
* cg_upnp_action_initchildnodes
****************************************/

static void cg_upnp_action_initchildnodes(CgUpnpAction *action)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_action_initargumentlist(action);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
*
* Embedded Action
*
****************************************/

/****************************************
* cg_upnp_action_initargumentlist
****************************************/

static void cg_upnp_action_initargumentlist(CgUpnpAction *action)
{
	CgXmlNode *actionNode;
	CgXmlNode *argumentListNode;
	CgXmlNode *childNode;
	CgUpnpArgument *arg;
	
	cg_log_debug_l4("Entering...\n");

	cg_upnp_argumentlist_clear(action->argumentList);

	actionNode = cg_upnp_action_getactionnode(action);
	argumentListNode = cg_xml_node_getchildnode(actionNode, CG_UPNP_ARGUMENTLIST_ELEM_NAME);
	
	if (argumentListNode == NULL)
		return;

	for (childNode = cg_xml_node_getchildnodes(argumentListNode); childNode != NULL; childNode = cg_xml_node_next(childNode)) {
	
		if (cg_upnp_argument_isargumentnode(childNode) == FALSE)
			continue;
			
		arg = cg_upnp_argument_new();
		cg_upnp_argument_setargumentnode(arg, childNode);
		cg_upnp_argumentlist_add(action->argumentList, arg);
	} 

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_action_getargumentbyname
****************************************/

CgUpnpArgument *cg_upnp_action_getargumentbyname(CgUpnpAction *action, char *name)
{
	CgUpnpArgumentList *argList;
	CgUpnpArgument *arg;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(name) <= 0)
		return NULL;
			
	argList = cg_upnp_action_getargumentlist(action);
	for (arg=cg_upnp_argumentlist_gets(argList); arg != NULL; arg = cg_upnp_argument_next(arg)) {
		if (cg_upnp_argument_isname(arg, name) == TRUE)
			return arg;
	}
	
	return NULL;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_action_getargumentvaluebyname
****************************************/

char *cg_upnp_action_getargumentvaluebyname(CgUpnpAction *action, char *name)
{
	CgUpnpArgument *arg;

	arg = cg_upnp_action_getargumentbyname(action, name);
	if (!arg)
		return NULL;
	return cg_upnp_argument_getvalue(arg);
}

/****************************************
* cg_upnp_action_setargumentvaluebyname
****************************************/

BOOL cg_upnp_action_setargumentvaluebyname(CgUpnpAction *action, char *name, char *value)
{
	CgUpnpArgument *arg;

	arg = cg_upnp_action_getargumentbyname(action, name);
	if (!arg)
		return FALSE;
	cg_upnp_argument_setvalue(arg, value);
	return TRUE;
}
