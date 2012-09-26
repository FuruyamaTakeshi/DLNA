/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006-2007 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: caction_request.c
*
*	Revision:
*
*	05/09/05
*		- first revision
*	10/31/05
*		- Thanks for Smolander Visa <visa.smolander@nokia.com>
*		- Changed cg_upnp_control_action_request_createactionnode() to use the namespace.
*	10/31/05
*		- Fixed not to include output-args in action request
*		- Fixed some namespace stuff during merge
*	12/13/07 Aapo Makela
*		- Fixes to work in out-of-memory situations
*
******************************************************************/

#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/util/clog.h>

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)

/****************************************
* cg_upnp_control_action_request_new
****************************************/

CgUpnpActionRequest *cg_upnp_control_action_request_new()
{
	CgUpnpActionRequest *actionReq;
	 
	cg_log_debug_l4("Entering...\n");

	actionReq = (CgUpnpActionRequest *)malloc(sizeof(CgUpnpActionRequest));
	
	if ( NULL != actionReq )
	{
		actionReq->soapReq = cg_soap_request_new();
		actionReq->isSoapReqCreated = TRUE;
		actionReq->actionRes = cg_upnp_control_action_response_new();
		
		actionReq->argList = cg_upnp_argumentlist_new();
	}
	
	cg_log_debug_l4("Leaving...\n");
	
	return actionReq;
}

/****************************************
* cg_upnp_control_action_request_delete
****************************************/

void cg_upnp_control_action_request_delete(CgUpnpActionRequest *actionReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_control_action_request_clear(actionReq);

	if (actionReq->isSoapReqCreated == TRUE)
		cg_soap_request_delete(actionReq->soapReq);

	cg_upnp_control_action_response_delete(actionReq->actionRes);
			
	cg_upnp_argumentlist_delete(actionReq->argList);
	
	free(actionReq);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_action_request_clear
****************************************/

void cg_upnp_control_action_request_clear(CgUpnpActionRequest *actionReq)
{
	cg_log_debug_l4("Entering...\n");

	if (actionReq->isSoapReqCreated == TRUE)
		cg_soap_request_delete(actionReq->soapReq);
	actionReq->soapReq = cg_soap_request_new();
	actionReq->isSoapReqCreated = TRUE;
	
	cg_upnp_argumentlist_clear(actionReq->argList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_action_request_setsoaprequest
****************************************/

void cg_upnp_control_action_request_setsoaprequest(CgUpnpActionRequest *actionReq, CgSoapRequest *soapReq)
{
	CgXmlNode *actionNode;
	CgXmlNode *argNode;
	CgUpnpArgument *arg;
	
	cg_log_debug_l4("Entering...\n");

	if (actionReq->isSoapReqCreated == TRUE)
		cg_soap_request_delete(actionReq->soapReq);
	actionReq->soapReq = soapReq;
	actionReq->isSoapReqCreated = FALSE;
	
	cg_upnp_argumentlist_clear(actionReq->argList);
	
	actionNode = cg_upnp_control_action_request_getactionnode(actionReq);
	if (actionNode == NULL)
		return;
	
	for (argNode = cg_xml_node_getchildnodes(actionNode); argNode != NULL; argNode = cg_xml_node_next(argNode)) {
		arg = cg_upnp_argument_new();
		cg_upnp_argument_setargumentnode(arg, argNode);
		cg_upnp_argument_setname(arg, cg_xml_node_getname( argNode ) );
		cg_upnp_argument_setvalue(arg, cg_xml_node_getvalue( argNode ) );
		cg_upnp_argumentlist_add(actionReq->argList, arg);
	}

	cg_soap_request_createcontent(soapReq);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_action_request_getactionnode
****************************************/

CgXmlNode *cg_upnp_control_action_request_getactionnode(CgUpnpActionRequest *actionReq)
{
	CgSoapRequest *soapReq;	
	CgXmlNode *bodyNode;
	
	cg_log_debug_l4("Entering...\n");

	soapReq = cg_upnp_control_action_request_getsoaprequest(actionReq);
	
	bodyNode = cg_soap_request_getbodynode(soapReq);
	if (bodyNode == NULL)
		return NULL;
	
	if (cg_xml_node_haschildnodes(bodyNode) == FALSE)
		return NULL;

	return cg_xml_node_getchildnodes(bodyNode);		

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_action_request_getactionname
****************************************/

char *cg_upnp_control_action_request_getactionname(CgUpnpActionRequest *actionReq)
{
	CgXmlNode *node;
	char *name;
	int urnDelimIdx;
	
	cg_log_debug_l4("Entering...\n");

	node = cg_upnp_control_action_request_getactionnode(actionReq);
	if (node == NULL)
		return "";
	
	name = cg_xml_node_getname(node);
	if (name == NULL)
		return "";	
		
	urnDelimIdx = cg_strstr(name, CG_HTTP_SOAP_URN_DELIM);
	if (urnDelimIdx < 0)
		return "";
		
	cg_log_debug_l4("Leaving...\n");
	
	return (name + urnDelimIdx + 1);
}

/****************************************
* cg_upnp_control_action_request_createactionnode
****************************************/

CgXmlNode *cg_upnp_control_action_request_createactionnode(CgUpnpAction *action)
{
	CgUpnpService *service;
	CgXmlNode *actionNode;
	CgUpnpArgument *arg;
	CgXmlNode *argNode;
	CgString *nameWithNamespace;
		
	cg_log_debug_l4("Entering...\n");

	service = cg_upnp_action_getservice(action);
	
	actionNode = cg_xml_node_new();
	/**** Thanks for Visa Smolander (10/31/2005) ****/
	nameWithNamespace = cg_string_new();
	cg_string_addvalue( nameWithNamespace, CG_UPNP_CONTROL_NS ":" );
	cg_string_addvalue( nameWithNamespace, cg_upnp_action_getname(action) );
	cg_xml_node_setname(actionNode, cg_string_getvalue( nameWithNamespace ) );
	cg_string_delete( nameWithNamespace );
	cg_xml_node_setnamespace(actionNode, CG_UPNP_CONTROL_NS, cg_upnp_service_getservicetype(service));
	
	for (arg = cg_upnp_action_getarguments(action); arg; arg = cg_upnp_argument_next(arg)) {
		if (cg_upnp_argument_isindirection(arg) == FALSE)
			continue;
		argNode = cg_xml_node_new();
		cg_xml_node_setname(argNode, cg_upnp_argument_getname(arg));			
		cg_xml_node_setvalue(argNode, cg_upnp_argument_getvalue(arg));			
		cg_xml_node_addchildnode(actionNode, argNode);
	}
	
	cg_log_debug_l4("Leaving...\n");
	
	return actionNode;
}

/****************************************
* cg_upnp_control_action_setrequest
****************************************/
	
void cg_upnp_control_action_request_setaction(CgUpnpActionRequest *actionReq, CgUpnpAction *action)
{
	CgUpnpService *service;
	CgSoapRequest *soapReq;
	CgString *soapAction;	
	CgXmlNode *bodyNode;
	CgXmlNode *contentNode;
	
	cg_log_debug_l4("Entering...\n");

	service = cg_upnp_action_getservice(action);
	soapReq = cg_upnp_control_action_request_getsoaprequest(actionReq);
	
	soapAction = cg_string_new();
	cg_string_addvalue(soapAction, "\"");
	cg_string_addvalue(soapAction, cg_upnp_service_getservicetype(service));
	cg_string_addvalue(soapAction, "#");
	cg_string_addvalue(soapAction, cg_upnp_action_getname(action));
	cg_string_addvalue(soapAction, "\"");
	cg_soap_request_setsoapaction(soapReq, cg_string_getvalue(soapAction));
	cg_string_delete(soapAction);
		
	cg_upnp_control_request_sethostfromservice(soapReq, service);
	
	cg_upnp_control_soap_request_initializeenvelopenode(soapReq);
	bodyNode = cg_soap_request_getbodynode(soapReq);
	contentNode = cg_upnp_control_action_request_createactionnode(action);
	cg_xml_node_addchildnode(bodyNode, contentNode);

	cg_soap_request_createcontent(soapReq);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_action_request_post
****************************************/

CgUpnpActionResponse *cg_upnp_control_action_request_post(CgUpnpActionRequest *actionReq)
{
	CgSoapRequest *soapReq;
	CgSoapResponse *soapRes;
	CgUpnpActionResponse *actionRes;
	CgHttpRequest *httpReq;
	CgNetURL *postURL;
	
	cg_log_debug_l4("Entering...\n");

	soapReq = cg_upnp_control_action_request_getsoaprequest(actionReq);
	soapRes = cg_soap_request_getsoapresponse(soapReq);
	actionRes = cg_upnp_control_action_request_getactionresponse(actionReq);
	httpReq = cg_soap_request_gethttprequest(soapReq);
	postURL = cg_http_request_getposturl(httpReq);
	
	cg_upnp_control_action_response_setsoapresponse(actionRes, soapRes);
	
	cg_soap_request_post(
		soapReq, 
		cg_net_url_gethost(postURL),
		cg_net_url_getport(postURL));
	
	cg_log_debug_l4("Leaving...\n");
	
	return actionRes;
}

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (End)
****************************************/

#endif

