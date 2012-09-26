/******************************************************************
*
*	CyberSOAP for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: csoap_response.c
*
*	Revision:
*
*	02/13/05
*		- first revision
*
******************************************************************/

#include <cybergarage/soap/csoap.h>
#include <cybergarage/util/clog.h>
#include <cybergarage/util/cstring.h>

/****************************************
* cg_soap_response_new
****************************************/

CgSoapResponse *cg_soap_response_new()
{
	CgSoapResponse *soapRes;

	cg_log_debug_l4("Entering...\n");

	soapRes = (CgSoapResponse *)malloc(sizeof(CgSoapResponse));

	if ( NULL != soapRes )
	{
		soapRes->rootNodeList = cg_xml_nodelist_new();

		soapRes->httpRes = cg_http_response_new();
		soapRes->isHttpResCreated = TRUE;
	
		cg_soap_response_setuserdata(soapRes, NULL);
	}

	return soapRes;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_soap_response_delete
****************************************/

void cg_soap_response_delete(CgSoapResponse *soapRes)
{
	cg_log_debug_l4("Entering...\n");

	cg_soap_response_clear(soapRes);
	cg_xml_nodelist_delete(soapRes->rootNodeList);

	if (soapRes->isHttpResCreated == TRUE)
		cg_http_response_delete(soapRes->httpRes);

	free(soapRes);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_soap_response_clear
****************************************/

void cg_soap_response_clear(CgSoapResponse *soapRes)
{
	cg_log_debug_l4("Entering...\n");

	cg_xml_nodelist_clear(soapRes->rootNodeList);

	if (soapRes->isHttpResCreated == TRUE)
		cg_http_response_delete(soapRes->httpRes);
	soapRes->httpRes = cg_http_response_new();
	soapRes->isHttpResCreated = TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_soap_request_getbodynode
****************************************/

CgXmlNode *cg_soap_response_getbodynode(CgSoapResponse *soapRes)
{
	CgXmlNode *envNode;
	 
	cg_log_debug_l4("Entering...\n");

	envNode = cg_soap_response_getenvelopenode(soapRes);
	if (envNode == NULL)
		return NULL;
	if (cg_xml_node_haschildnodes(envNode) == FALSE)
		return NULL;
	return cg_xml_node_getchildnodes(envNode);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_soap_response_sethttpresponse
****************************************/

void cg_soap_response_sethttpresponse(CgSoapResponse *soapRes, CgHttpResponse *httpRes)
{
	cg_log_debug_l4("Entering...\n");

	if (soapRes->isHttpResCreated == TRUE)
		cg_http_response_delete(soapRes->httpRes);
	soapRes->httpRes = httpRes;
	soapRes->isHttpResCreated = FALSE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_soap_response_setcontent
****************************************/

void cg_soap_response_setcontent(CgSoapResponse *soapReq, CgXmlNode *node)
{
	CgHttpResponse *httpRes;
		
	cg_log_debug_l4("Entering...\n");

	httpRes = cg_soap_response_gethttpresponse(soapReq);

	/**** content type ****/
	cg_http_response_setcontenttype(httpRes, CG_XML_CONTENT_TYPE);
	
	/**** content ****/
	cg_http_response_appendncontent(httpRes, CG_SOAP_VERSION_HEADER,
					cg_strlen(CG_SOAP_VERSION_HEADER));
	cg_http_response_appendncontent(httpRes, CG_XML_CONTENT_LF,
					cg_strlen(CG_XML_CONTENT_LF));
	cg_xml_node_tostring(node, TRUE, httpRes->content);
	
	/**** content length ****/
	cg_http_response_setcontentlength(httpRes, 
					  cg_string_length(httpRes->content));

	cg_log_debug_l4("Leaving...\n");
}
