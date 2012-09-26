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
*	File: cquery_request.c
*
*	Revision:
*
*	06/09/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/upnp/cservice.h>
#include <cybergarage/util/clog.h>

/****************************************
* CG_UPNP_NOUSE_QUERY (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_QUERYCTRL)

/****************************************
* cg_upnp_control_query_request_new
****************************************/

CgUpnpQueryRequest *cg_upnp_control_query_request_new()
{
	CgUpnpQueryRequest *queryReq;
	 
	cg_log_debug_l4("Entering...\n");

	queryReq = (CgUpnpQueryRequest *)malloc(sizeof(CgUpnpQueryRequest));

	if ( NULL != queryReq )
	{	
		queryReq->soapReq = cg_soap_request_new();
		if (queryReq->soapReq == NULL) 
		{
			/* Memory allocation failed */
			free(queryReq);
			return NULL;
		}

		queryReq->isSoapReqCreated = TRUE;
		
		queryReq->queryRes = cg_upnp_control_query_response_new();
		if (queryReq->queryRes == NULL)
		{
			/* Memory allocation failed */
			cg_soap_request_delete(queryReq->soapReq);
			free(queryReq);
			return NULL;
		}
	}
		
	return queryReq;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_query_request_delete
****************************************/

void cg_upnp_control_query_request_delete(CgUpnpQueryRequest *queryReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_control_query_request_clear(queryReq);

	if (queryReq->isSoapReqCreated == TRUE)
		cg_soap_request_delete(queryReq->soapReq);

	cg_upnp_control_query_response_delete(queryReq->queryRes);
			
	free(queryReq);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_query_request_clear
****************************************/

void cg_upnp_control_query_request_clear(CgUpnpQueryRequest *queryReq)
{
	cg_log_debug_l4("Entering...\n");

	if (queryReq->isSoapReqCreated == TRUE)
		cg_soap_request_delete(queryReq->soapReq);
	queryReq->soapReq = cg_soap_request_new();
	queryReq->isSoapReqCreated = TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_query_request_setsoaprequest
****************************************/

void cg_upnp_control_query_request_setsoaprequest(CgUpnpQueryRequest *queryReq, CgSoapRequest *soapReq)
{
	cg_log_debug_l4("Entering...\n");

	if (queryReq->isSoapReqCreated == TRUE)
		cg_soap_request_delete(queryReq->soapReq);
	queryReq->soapReq = soapReq;
	queryReq->isSoapReqCreated = FALSE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_query_request_getvarnamenode
****************************************/

CgXmlNode *cg_upnp_control_query_request_getvarnamenode(CgUpnpQueryRequest *queryReq)
{
	CgSoapRequest *soapReq;	
	CgXmlNode *bodyNode;
	CgXmlNode *queryStateVarNode;
	
	cg_log_debug_l4("Entering...\n");

	soapReq = cg_upnp_control_query_request_getsoaprequest(queryReq);
	
	bodyNode = cg_soap_request_getbodynode(soapReq);
	if (bodyNode == NULL)
		return NULL;
	if (cg_xml_node_haschildnodes(bodyNode) == FALSE)
		return NULL;

	queryStateVarNode = cg_xml_node_getchildnodes(bodyNode);		
	if (queryStateVarNode == NULL)
		return NULL;
	if (cg_xml_node_haschildnodes(queryStateVarNode) == FALSE)
		return NULL;
		
	return cg_xml_node_getchildnodes(queryStateVarNode);		

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_query_request_getqueryname
****************************************/

char *cg_upnp_control_query_request_getvarname(CgUpnpQueryRequest *queryReq)
{
	CgXmlNode *node;
	
	cg_log_debug_l4("Entering...\n");

	node = cg_upnp_control_query_request_getvarnamenode(queryReq);
	if (node == NULL)
		return "";
	
	return cg_xml_node_getvalue(node);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_query_request_cratestatevariablenode
****************************************/

CgXmlNode *cg_upnp_control_query_request_cratestatevariablenode(CgUpnpStateVariable *statVar)
{
	CgXmlNode *queryVarNode;
	CgXmlNode *varNode;
	
	cg_log_debug_l4("Entering...\n");

	queryVarNode = cg_xml_node_new();
	cg_xml_node_setname(queryVarNode, CG_UPNP_CONTROL_NS  CG_SOAP_DELIM CG_UPNP_CONTROL_QUERY_STATE_VARIABLE);
	cg_xml_node_setnamespace(queryVarNode, CG_UPNP_CONTROL_NS, CG_UPNP_CONTROL_XMLNS);
	
	varNode = cg_xml_node_new();
	cg_xml_node_setname(varNode, CG_UPNP_CONTROL_NS CG_SOAP_DELIM CG_UPNP_CONTROL_VAR_NAME);
	cg_xml_node_setvalue(varNode, cg_upnp_statevariable_getname(statVar));
	
	cg_xml_node_addchildnode(queryVarNode, varNode);
	
	return queryVarNode;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_query_request_setstatevariable
****************************************/

void cg_upnp_control_query_request_setstatevariable(CgUpnpQueryRequest *queryReq, CgUpnpStateVariable *statVar)
{
	CgUpnpService *service;
	CgSoapRequest *soapReq;
	CgXmlNode *bodyNode;
	CgXmlNode *queryNode;
	
	cg_log_debug_l4("Entering...\n");

	service = cg_upnp_statevariable_getservice(statVar);
	soapReq = cg_upnp_control_query_request_getsoaprequest(queryReq);
	
	cg_soap_request_setsoapaction(soapReq, CG_UPNP_CONTROL_QUERY_SOAPACTION);
	
	cg_upnp_control_request_sethostfromservice(soapReq, service);
	
	cg_upnp_control_soap_request_initializeenvelopenode(soapReq);
	bodyNode = cg_soap_request_getbodynode(soapReq);
	queryNode = cg_upnp_control_query_request_cratestatevariablenode(statVar);
	cg_xml_node_addchildnode(bodyNode, queryNode);

	cg_soap_request_createcontent(soapReq);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_query_request_post
****************************************/

CgUpnpQueryResponse *cg_upnp_control_query_request_post(CgUpnpQueryRequest *queryReq)
{
	CgSoapRequest *soapReq;
	CgSoapResponse *soapRes;
	CgUpnpQueryResponse *queryRes;
	CgHttpRequest *httpReq;
	CgNetURL *postURL;
	
	cg_log_debug_l4("Entering...\n");

	soapReq = cg_upnp_control_query_request_getsoaprequest(queryReq);
	soapRes = cg_soap_request_getsoapresponse(soapReq);
	queryRes = cg_upnp_control_query_request_getqueryresponse(queryReq);
	httpReq = cg_soap_request_gethttprequest(soapReq);
	postURL = cg_http_request_getposturl(httpReq);
	
	cg_upnp_control_query_response_setsoapresponse(queryRes, soapRes);
	
	cg_soap_request_post(
		soapReq, 
		cg_net_url_gethost(postURL),
		cg_net_url_getport(postURL));
	
	return queryRes;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_QUERY (End)
****************************************/

#endif
