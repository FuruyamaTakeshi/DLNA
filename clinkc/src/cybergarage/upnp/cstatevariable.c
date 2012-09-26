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
*	File: cstatevariable.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cstatevariable.h>
#include <cybergarage/upnp/cservice.h>
#include <cybergarage/util/clog.h>

#include <string.h>

/****************************************
* cg_upnp_statevariable_new
****************************************/

CgUpnpStateVariable *cg_upnp_statevariable_new()
{
	CgUpnpStateVariable *statVar;

	cg_log_debug_l4("Entering...\n");

	statVar = (CgUpnpStateVariable *)malloc(sizeof(CgUpnpStateVariable));

	if ( NULL != statVar )
	{
		cg_list_node_init((CgList *)statVar);
		
		statVar->parentService = NULL;
		statVar->stateVarNode = NULL;
		
		statVar->value = cg_string_new();
		statVar->upnpStatus = cg_upnp_status_new();
		statVar->allowedValueList = NULL;
		
		
		cg_upnp_statevariable_setlistener(statVar, NULL);
		cg_upnp_statevariable_setuserdata(statVar, NULL);
	}
	
	cg_log_debug_l4("Leaving...\n");

	return statVar;
}

/****************************************
* cg_upnp_statevariable_delete
****************************************/

void cg_upnp_statevariable_delete(CgUpnpStateVariable *statVar)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)statVar);
	
	cg_string_delete(statVar->value);
	cg_upnp_status_delete(statVar->upnpStatus);
	if (statVar->allowedValueList)
	{
		cg_list_remove((CgList *)statVar->allowedValueList);
	}
	
	free(statVar);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_statevariable_setstatevariablenode
****************************************/

void cg_upnp_statevariable_setstatevariablenode(CgUpnpStateVariable *statVar, CgXmlNode *node)
{
	cg_log_debug_l4("Entering...\n");

	statVar->stateVarNode = node;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_statevariable_setvaluewithnotify
****************************************/

static void cg_upnp_statevariable_setvaluewithnotify(CgUpnpStateVariable *statVar, char *data, BOOL doNotify)
{
	CgUpnpService *service;

	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(statVar->value, data);

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	/**** notify event ****/
	if (doNotify) {
		if (cg_upnp_statevariable_issendevents(statVar) == FALSE)
			return;

		service = cg_upnp_statevariable_getservice(statVar);
		if (service == NULL)
			return;

		cg_upnp_service_notify(service, statVar);
	}
#endif

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_statevariable_setvalue
****************************************/

void cg_upnp_statevariable_setvalue(CgUpnpStateVariable *statVar, char *data)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_statevariable_setvaluewithnotify(statVar, data, TRUE);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_statevariable_setvaluewithoutnotify
****************************************/

void cg_upnp_statevariable_setvaluewithoutnotify(CgUpnpStateVariable *statVar, char *data)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_statevariable_setvaluewithnotify(statVar, data, FALSE);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
 * cg_upnp_allowedvaluelist_new
 ****************************************/

CgUpnpAllowedValuesList* cg_upnp_allowedvaluelist_new()
{
	CgUpnpAllowedValuesList* allowedValueList = (CgUpnpAllowedValuesList *)malloc(sizeof(CgUpnpAllowedValuesList));
	
	
	if ( NULL != allowedValueList )
	{
		cg_list_header_init((CgList *)allowedValueList);
		allowedValueList->value = cg_string_new();
		return allowedValueList;
	}
	
	return NULL;	
}


/****************************************
 * cg_upnp_statevariable_getallowedvaluelist
 ****************************************/
CgUpnpAllowedValuesList*  cg_upnp_statevariable_getallowedvaluelist(CgUpnpStateVariable* statVar)
{
	if (!statVar->allowedValueList)
	{
		CgXmlNode* allowedValuesNode = cg_xml_node_getchildnodebyname(cg_upnp_statevariable_getstatevariablenode(statVar), CG_UPNP_STATEVARIABLE_ALLOWEDVALUELIST);
		
		if (allowedValuesNode) {
			CgXmlNode* cnode;
			for (cnode = cg_xml_node_getchildnodes(allowedValuesNode); cnode; cnode=cg_xml_node_next(cnode)) {
				if (!statVar->allowedValueList)
				{
					statVar->allowedValueList = (CgUpnpAllowedValuesList *)malloc(sizeof(CgUpnpAllowedValuesList));
					cg_list_header_init((CgList *)statVar->allowedValueList);
					statVar->allowedValueList->value = cg_string_new();
					cg_string_setvalue(statVar->allowedValueList->value, cg_xml_node_getvalue(cnode));
				}
				else {
					CgUpnpAllowedValue* allowed_value;
					allowed_value = (CgUpnpAllowedValue *)malloc(sizeof(CgUpnpAllowedValue));
					cg_list_node_init((CgList*)allowed_value);
					allowed_value->value = cg_string_new();
					cg_string_setvalue(allowed_value->value, cg_xml_node_getvalue(cnode));
					cg_list_insert((CgList *)statVar->allowedValueList, (CgList *)allowed_value);
				}
			}	
		}
	}
	return statVar->allowedValueList;
}

/****************************************
 * cg_upnp_statevariable_is_allowed_value
 ****************************************/
int cg_upnp_statevariable_is_allowed_value(CgUpnpStateVariable* statVar, const char* value)
{
	if (!statVar->allowedValueList)
	{
		cg_upnp_statevariable_getallowedvaluelist(statVar);
	}
	
	if (statVar->allowedValueList)
	{		
		CgUpnpAllowedValue *allowedValue;
		for (allowedValue = (CgUpnpAllowedValue*)cg_list_next((CgList*)statVar->allowedValueList); allowedValue != NULL; allowedValue = (CgUpnpAllowedValue*)cg_list_next((CgList*)allowedValue))
		{
			if (strcasecmp(value, cg_string_getvalue(allowedValue->value)) == 0)
			{
				return 1;
			}
		}
	}
	return 0;
}
