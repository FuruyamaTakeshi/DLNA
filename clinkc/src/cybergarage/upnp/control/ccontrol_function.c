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
*	File: ccontrol_function.c
*
*	Revision:
*
*	06/03/05
*		- first revision
*	09/11/05
*		- Thanks for Visa Smolander <visa.smolander@nokia.com>
*		- Fixed cg_upnp_control_soap_response_createfaultresponsenode() to set
*		  the namespace attribute in the response <UPnPError> node.
*
*****************************************************************/

#include <cybergarage/upnp/control/ccontrol.h>

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/util/cstring.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_usn_getudn
****************************************/

char *cg_upnp_usn_getudn(char *usn, char *udnBuf, int udnBufLen)
{
	int idx;
	
	cg_log_debug_l4("Entering...\n");

	if (usn == NULL) {
		udnBuf[0] = '\0';
		return udnBuf;
	}
	
	idx = cg_strstr(usn, "::");
	if (idx < 0) {
		cg_strncpy(udnBuf, usn, udnBufLen);
		udnBuf[udnBufLen-1] = '\0';
		cg_strtrim(udnBuf, " ", 1);
		return udnBuf;
	}
	
	if ( idx < udnBufLen ) {
		cg_strncpy(udnBuf, usn, (idx));
		udnBuf[idx] = '\0';
	}
	else {
		cg_strncpy(udnBuf, usn, udnBufLen);
		udnBuf[udnBufLen-1] = '\0';
	}
	
	cg_strtrim(udnBuf, " ", 1);

	cg_log_debug("UDN: %s\n", udnBuf);
	
	return udnBuf;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_soap_response_initializeenvelopenode
****************************************/

void cg_upnp_control_soap_response_initializeenvelopenode(CgSoapResponse *soapRes)
{
	CgXmlNodeList *rootNodeList;
	CgHttpResponse *httpRes;

	cg_log_debug_l4("Entering...\n");

	rootNodeList = cg_soap_response_getrootnoodelist(soapRes);
	cg_xml_nodelist_add(rootNodeList, cg_soap_createenvelopebodynode());

	httpRes = cg_soap_response_gethttpresponse(soapRes);
	cg_http_request_setcontenttype(httpRes, CG_SOAP_CONTENT_TYPE);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_soap_request_initializeenvelopenode
****************************************/

void cg_upnp_control_soap_request_initializeenvelopenode(CgSoapRequest *soapReq)
{
	CgXmlNodeList *rootNodeList;
	CgHttpRequest *httpReq;

	cg_log_debug_l4("Entering...\n");

	rootNodeList = cg_soap_request_getrootnoodelist(soapReq);
	cg_xml_nodelist_add(rootNodeList, cg_soap_createenvelopebodynode());

	httpReq = cg_soap_request_gethttprequest(soapReq);
	cg_http_request_setcontenttype(httpReq, CG_SOAP_CONTENT_TYPE);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_soap_response_createfaultresponsenode
****************************************/

CgXmlNode *cg_upnp_control_soap_response_createfaultresponsenode(int errCode, char *errDescr)
{
	CgXmlNode *faultNode;
	CgXmlNode *faultCodeNode;
	CgXmlNode *faultStringNode;
	CgXmlNode *detailNode;
	CgXmlNode *upnpErrorNode;
	CgXmlNode *errorCodeNode;
	CgXmlNode *errorDesctiprionNode;
	
	cg_log_debug_l4("Entering...\n");

	/**** <s:Fault> ****/
	faultNode = cg_xml_node_new();
	cg_xml_node_setname(faultNode, CG_SOAP_XMLNS CG_SOAP_DELIM CG_SOAP_FAULT);
	
	/**** <faultcode>s:Client</faultcode> ****/
	faultCodeNode = cg_xml_node_new();
	cg_xml_node_setname(faultCodeNode, CG_SOAP_FAULT_CODE);
	cg_xml_node_setvalue(faultCodeNode, CG_SOAP_XMLNS CG_SOAP_DELIM CG_UPNP_CONTROL_FAULT_CODE);
	cg_xml_node_addchildnode(faultNode, faultCodeNode);
		
	/**** <faultstring>UPnPError</faultstring> ****/
	faultStringNode = cg_xml_node_new();
	cg_xml_node_setname(faultStringNode, CG_SOAP_FAULT_STRING);
	cg_xml_node_setvalue(faultStringNode, CG_UPNP_CONTROL_FAULT_STRING);
	cg_xml_node_addchildnode(faultNode, faultStringNode);

	/**** <detail> ****/
	detailNode = cg_xml_node_new();
	cg_xml_node_setname(detailNode, CG_SOAP_DETAIL);
	cg_xml_node_addchildnode(faultNode, detailNode);

	/**** <UPnPError xmlns="urn:schemas-upnp-org:control-1-0"> ****/
	upnpErrorNode = cg_xml_node_new();
	cg_xml_node_setname(upnpErrorNode, CG_UPNP_CONTROL_FAULT_STRING);
	/**** Thanks for Visa Smolander (09/11/2005) ****/
	cg_xml_node_setattribute(upnpErrorNode, CG_SOAP_ATTRIBUTE_XMLNS, CG_UPNP_CONTROL_XMLNS);
	cg_xml_node_addchildnode(detailNode, upnpErrorNode);

	/**** <errorCode>error code</errorCode> ****/
	errorCodeNode = cg_xml_node_new();
	cg_xml_node_setname(errorCodeNode, CG_UPNP_CONTROL_ERROR_CODE);
	cg_xml_node_setintvalue(errorCodeNode, errCode);
	cg_xml_node_addchildnode(upnpErrorNode, errorCodeNode);

	/**** <errorDescription>error string</errorDescription> ****/
	errorDesctiprionNode = cg_xml_node_new();
	cg_xml_node_setname(errorDesctiprionNode, CG_UPNP_CONTROL_ERROR_DESCRIPTION);
	cg_xml_node_setvalue(errorDesctiprionNode, errDescr);
	cg_xml_node_addchildnode(upnpErrorNode, errorDesctiprionNode);
	
	return faultNode;

	cg_log_debug_l4("Leaving...\n");
}


/****************************************
* cg_upnp_control_soap_response_setfaultresponse
****************************************/

void cg_upnp_control_soap_response_setfaultresponse(CgSoapResponse *soapRes, int errCode, char *errDescr)
{
	CgHttpResponse *httpRes;
	CgXmlNode *bodyNode;
	CgXmlNode *faultNode;
	CgXmlNode *envNode;

	cg_log_debug_l4("Entering...\n");

	httpRes = cg_soap_response_gethttpresponse(soapRes);

	cg_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_INTERNAL_SERVER_ERROR);
	cg_upnp_control_soap_response_initializeenvelopenode(soapRes);

	bodyNode = cg_soap_response_getbodynode(soapRes);
	faultNode = cg_upnp_control_soap_response_createfaultresponsenode(errCode, errDescr);
	cg_xml_node_addchildnode(bodyNode, faultNode);
	
	envNode = cg_soap_response_getenvelopenode(soapRes);
	cg_soap_response_setcontent(soapRes, envNode);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_control_request_sethostfromservice
****************************************/

void cg_upnp_control_request_sethostfromservice(CgSoapRequest *soapReq, CgUpnpService *service)
{
	CgHttpRequest *httpReq;
	CgNetURL *ctrlURL;
	
	cg_log_debug_l4("Entering...\n");

	httpReq = cg_soap_request_gethttprequest(soapReq);

	ctrlURL = cg_upnp_service_getcontrolurl(service);

	cg_log_debug_s("Ctrl URL: %s - %d -%s", 
			cg_net_url_gethost(ctrlURL), 
			cg_net_url_getport(ctrlURL), 
			cg_net_url_getpath(ctrlURL));

	//cg_http_request_seturi(httpReq, cg_xml_node_getchildnodevalue(cg_upnp_service_getservicenode(service), CG_UPNP_SERVICE_CONTROL_URL));
	cg_http_request_seturi(httpReq, cg_net_url_getrequest(ctrlURL));

	/**** Host ****/
	cg_net_url_delete(httpReq->postURL);
	httpReq->postURL = ctrlURL;

	cg_log_debug_s("Post URL: %s - %d -%s", 
			cg_net_url_gethost(httpReq->postURL), 
			cg_net_url_getport(httpReq->postURL), 
			cg_net_url_getpath(httpReq->postURL));

	cg_log_debug_l4("Leaving...\n");
}
