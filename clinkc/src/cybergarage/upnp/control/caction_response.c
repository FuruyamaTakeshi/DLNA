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
*	File: caction_response.c
*
*	Revision:
*
*	05/17/05
*		- first revision
*
*	10/31/05
*		- Added cg_upnp_control_action_response_geterror
*
******************************************************************/

#include <cybergarage/upnp/cupnp_limit.h>
#include <cybergarage/upnp/cservice.h>
#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/util/clog.h>

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)

/****************************************
* cg_upnp_control_action_response_new
****************************************/

CgUpnpActionResponse *cg_upnp_control_action_response_new()
{
	CgUpnpActionResponse *actionRes;
	 
	cg_log_debug_l4("Entering...\n");

	actionRes = (CgUpnpActionResponse *)malloc(sizeof(CgUpnpActionResponse));

	if ( NULL != actionRes )
	{	
		actionRes->soapRes = cg_soap_response_new();
		actionRes->isSoapResCreated = TRUE;

		actionRes->argList = cg_upnp_argumentlist_new();
	}
	
	return actionRes;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_action_response_delete
****************************************/

void cg_upnp_control_action_response_delete(CgUpnpActionResponse *actionRes)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_control_action_response_clear(actionRes);
	
	if (actionRes->isSoapResCreated == TRUE)
		cg_soap_response_delete(actionRes->soapRes);
		
	cg_upnp_argumentlist_delete(actionRes->argList);
	
	free(actionRes);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_action_response_clear
****************************************/

void cg_upnp_control_action_response_clear(CgUpnpActionResponse *actionRes)
{
	cg_log_debug_l4("Entering...\n");

	if (actionRes->isSoapResCreated == TRUE)
		cg_soap_response_delete(actionRes->soapRes);
	actionRes->soapRes = cg_soap_response_new();
	actionRes->isSoapResCreated = TRUE;
		
	cg_upnp_argumentlist_clear(actionRes->argList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_action_response_setsoapresponse
****************************************/

void cg_upnp_control_action_response_setsoapresponse(CgUpnpActionResponse *actionRes, CgSoapResponse *soapRes)
{
	cg_log_debug_l4("Entering...\n");

	if (actionRes->isSoapResCreated == TRUE)
		cg_soap_response_delete(actionRes->soapRes);
	actionRes->soapRes = soapRes;
	actionRes->isSoapResCreated = FALSE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_action_response_createresponsenode
****************************************/

static CgXmlNode *cg_upnp_control_action_response_createresponsenode(CgUpnpAction *action)
{
	CgXmlNode *actionNameResNode;
	char nodeName[CG_UPNP_ACTOINNAME_LEN_MAX + sizeof(CG_SOAP_METHODNS) + sizeof(CG_SOAP_DELIM) + sizeof(CG_SOAP_RESPONSE) + 1];
	char attrName[sizeof(CG_SOAP_ATTRIBUTE_XMLNS) + sizeof(CG_SOAP_DELIM) + sizeof(CG_SOAP_METHODNS) + 1];
	char *actionName;
	CgXmlNode *serviceNode;
	CgUpnpService *service;
	CgUpnpArgumentList *argList;
	CgUpnpArgument *arg;
	CgXmlNode *argNode;
	
	cg_log_debug_l4("Entering...\n");

	actionNameResNode = cg_xml_node_new();

	/* action name */
	actionName = cg_upnp_action_getname(action);
	cg_strcpy(nodeName, CG_SOAP_METHODNS);
	cg_strcat(nodeName, CG_SOAP_DELIM);
	cg_strncat(nodeName, actionName, CG_UPNP_ACTOINNAME_LEN_MAX);
	cg_strcat(nodeName, CG_SOAP_RESPONSE);
	cg_xml_node_setname(actionNameResNode, nodeName);

	/* service attribute */
	serviceNode = cg_upnp_service_getservicenode(cg_upnp_action_getservice(action));
	if (serviceNode != NULL) {
		service = cg_upnp_service_new();
		cg_upnp_service_setservicenode(service, serviceNode);
		cg_strcpy(attrName, CG_SOAP_ATTRIBUTE_XMLNS);
		cg_strcat(attrName, CG_SOAP_DELIM);
		cg_strcat(attrName, CG_SOAP_METHODNS);
		cg_xml_node_setattribute(actionNameResNode, attrName, cg_upnp_service_getservicetype(service));
		cg_upnp_service_delete(service);
	}

	/* arguments */
	argList = cg_upnp_action_getargumentlist(action);
	for (arg = cg_upnp_argumentlist_gets(argList); arg != NULL; arg = cg_upnp_argument_next(arg)) {
		if (cg_upnp_argument_isoutdirection(arg) == FALSE)
			continue;
		argNode = cg_xml_node_new();
		cg_xml_node_setname(argNode, cg_upnp_argument_getname(arg));		
		cg_xml_node_setvalue(argNode, cg_upnp_argument_getvalue(arg));
		cg_xml_node_addchildnode(actionNameResNode, argNode);
	}

	return actionNameResNode;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_action_response_setresponse
****************************************/

void cg_upnp_control_action_response_setresponse(CgUpnpActionResponse *actionRes, CgUpnpAction *action)
{
	CgSoapResponse *soapRes;
	CgHttpResponse *httpRes;
	CgXmlNode *bodyNode;
	CgXmlNode *resNode;
	CgXmlNode *envNode;

	cg_log_debug_l4("Entering...\n");

	soapRes = cg_upnp_control_action_response_getsoapresponse(actionRes);
	httpRes = cg_soap_response_gethttpresponse(soapRes);

	cg_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_OK);
	cg_upnp_control_soap_response_initializeenvelopenode(soapRes);

	bodyNode = cg_soap_response_getbodynode(soapRes);
	resNode = cg_upnp_control_action_response_createresponsenode(action);
	cg_xml_node_addchildnode(bodyNode, resNode);
	
	envNode = cg_soap_response_getenvelopenode(soapRes);
	cg_soap_response_setcontent(soapRes, envNode);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_action_response_getactionresponsenode
****************************************/

CgXmlNode *cg_upnp_control_action_response_getactionresponsenode(CgUpnpActionResponse *actionRes)
{
	CgSoapResponse *soapRes;
	CgXmlNode *bodyNode;
	
	cg_log_debug_l4("Entering...\n");

	soapRes = cg_upnp_control_action_response_getsoapresponse(actionRes);
	
	bodyNode = cg_soap_response_getbodynode(soapRes);

	if (bodyNode == NULL)
		return NULL;
	if (cg_xml_node_haschildnodes(bodyNode) == FALSE)
		return NULL;

	return cg_xml_node_getchildnodes(bodyNode);		

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_action_response_getresult
****************************************/

BOOL cg_upnp_control_action_response_getresult(CgUpnpActionResponse *actionRes, CgUpnpAction *action)
{
	CgXmlNode *resNode;
	CgXmlNode *argNode;
	char *argName;
	CgUpnpArgument *arg;
	
	cg_log_debug_l4("Entering...\n");

	resNode = cg_upnp_control_action_response_getactionresponsenode(actionRes);
	if (resNode == NULL)
		return FALSE;
		
	for (argNode = cg_xml_node_getchildnodes(resNode); argNode != NULL; argNode = cg_xml_node_next(argNode)) {
		argName = cg_xml_node_getname(argNode);
		arg = cg_upnp_action_getargumentbyname(action, argName);
		if (arg == NULL)
			continue;
		cg_upnp_argument_setvalue(arg, cg_xml_node_getvalue(argNode));
	}

	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_action_response_geterror
****************************************/
BOOL cg_upnp_control_action_response_geterror(CgUpnpActionResponse *actionRes, CgUpnpAction *action)
{
	CgXmlNode *resNode;
	CgXmlNode *upnpErrorNode;
	CgXmlNode *node;
	char *errDesc = NULL;
	char *errCode = NULL;
	
	cg_log_debug_l4("Entering...\n");

	resNode = cg_upnp_control_action_response_getactionresponsenode(actionRes);
	if (resNode == NULL)
	{
		return FALSE;
	}
		
	/* Response node is FAULT node, there will be no output args,
	   but set action status and description */
	upnpErrorNode = cg_xml_node_getchildnodewithnamespace(resNode, CG_SOAP_DETAIL, NULL, TRUE);

	if (upnpErrorNode == NULL) return FALSE;
		
	upnpErrorNode = cg_xml_node_getchildnodewithnamespace(upnpErrorNode, CG_UPNP_CONTROL_FAULT_STRING, 
							      NULL, TRUE);

	if (upnpErrorNode == NULL) return FALSE;
		
	node = cg_xml_node_getchildnodewithnamespace(upnpErrorNode, CG_UPNP_CONTROL_ERROR_DESCRIPTION, 
						     NULL, TRUE);
	if (node)
		errDesc = cg_xml_node_getvalue(node);

	node = cg_xml_node_getchildnodewithnamespace(upnpErrorNode, CG_UPNP_CONTROL_ERROR_CODE, 
						     NULL, TRUE);
	if (node)
		errCode = cg_xml_node_getvalue(node);

	if (errCode == NULL) return FALSE;

	cg_upnp_action_setstatusdescription(action, errDesc);
	cg_upnp_action_setstatuscode(action, cg_str2int(errCode));
		
	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (End)
****************************************/

#endif

