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
*	File: csoap_request.c
*
*	Revision:
*
*	02/13/05
*		- first revision
*
*	18-Jan-06 Aapo Makela
*		- Fixed to send also M-POST, if required (UPnP DA)
******************************************************************/

#include <cybergarage/soap/csoap.h>
#include <cybergarage/util/clog.h>
#include <cybergarage/util/cstring.h>

/****************************************
* cg_soap_request_new
****************************************/

CgSoapRequest *cg_soap_request_new()
{
	CgSoapRequest *soapReq;

	cg_log_debug_l4("Entering...\n");

	soapReq = (CgSoapRequest *)malloc(sizeof(CgSoapRequest));

	if ( NULL != soapReq )
	{
		soapReq->rootNodeList = cg_xml_nodelist_new();
		soapReq->soapRes = cg_soap_response_new();

		soapReq->httpReq = cg_http_request_new();
		soapReq->isHttpReqCreated = TRUE;
		cg_http_request_setcontenttype(soapReq->httpReq, CG_SOAP_CONTENT_TYPE);
		cg_http_request_setmethod(soapReq->httpReq, CG_HTTP_POST);
	
		cg_soap_request_setuserdata(soapReq, NULL);
	}

	cg_log_debug_l4("Leaving...\n");
	
	return soapReq;
}

/****************************************
* cg_soap_request_delete
****************************************/

void cg_soap_request_delete(CgSoapRequest *soapReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_soap_request_clear(soapReq);
	cg_xml_nodelist_delete(soapReq->rootNodeList);	

	if (soapReq->isHttpReqCreated == TRUE)
		cg_http_request_delete(soapReq->httpReq);

	cg_soap_response_delete(soapReq->soapRes);

	free(soapReq);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_soap_request_clear
****************************************/

void cg_soap_request_clear(CgSoapRequest *soapReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_xml_nodelist_clear(soapReq->rootNodeList);

	if (soapReq->isHttpReqCreated == TRUE)
		cg_http_request_delete(soapReq->httpReq);

	soapReq->httpReq = cg_http_request_new();
	soapReq->isHttpReqCreated = TRUE;
	cg_http_request_setcontenttype(soapReq->httpReq, CG_SOAP_CONTENT_TYPE);
	cg_http_request_setmethod(soapReq->httpReq, CG_HTTP_POST);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_soap_request_getbodynode
****************************************/

CgXmlNode *cg_soap_request_getbodynode(CgSoapRequest *soapReq)
{
	CgXmlNode *envNode;
	CgXmlNode *bodyNode = NULL;
        CgXmlAttribute *attr;
        char *name;
        CgStringTokenizer *tok;
        char *nsPrefix;
        int bodyLen;
        char *body;

	cg_log_debug_l4("Entering...\n");

	envNode = cg_soap_request_getenvelopenode(soapReq);
	if (envNode == NULL)
		return NULL;
	if (cg_xml_node_haschildnodes(envNode) == FALSE)
		return NULL;

        /* We cannot assume the namespace prefix for Body is 's'. 
           According to spec, it could be anything... */
        for (attr = cg_xml_node_getattributes(envNode); attr != NULL; 
             attr = cg_xml_attribute_next(attr)) {
                /* First, find the namespace declaration attribute. */
                /* Note: We must take a copy of the attr name. 
                   Tokenizer doesn't do it (by default) */
                name = cg_strdup( cg_xml_attribute_getname(attr) );
                tok = cg_string_tokenizer_new(name, ":");

                nsPrefix = cg_string_tokenizer_nexttoken(tok);
                if ( -1 != cg_strstr(nsPrefix, "xmlns")) {
                        /* This attribute is a namespace declaration. Check is 
                           it the one defined for SOAP. */
                        if (cg_strcmp(cg_xml_attribute_getvalue(attr), CG_SOAP_XMLNS_URL) == 0) {
                                /* This namespace declaration is correct. 
                                   Use it to find the body node... */
                                if (cg_string_tokenizer_hasmoretoken(tok)) {
                                        /* There is a prefix */
                                        nsPrefix = cg_string_tokenizer_nexttoken(tok);
                                        bodyLen = cg_strlen(nsPrefix) + 
                                                cg_strlen(CG_SOAP_DELIM) + 
                                                cg_strlen(CG_SOAP_BODY) + 1; /* +1 for trailing '\0'*/
                                        body = (char*)malloc(bodyLen);

					if ( NULL == body )
					{
						cg_log_debug_s("Memory allocation failure!\n");
						return NULL;
					}
#if defined(HAVE_SNPRINTF)
                                        snprintf(body, bodyLen, "%s%s%s", nsPrefix, 
                                                 CG_SOAP_DELIM, CG_SOAP_BODY);
#else
                                        sprintf(body, "%s%s%s", nsPrefix, CG_SOAP_DELIM, CG_SOAP_BODY);
#endif
                                        bodyNode = cg_xml_node_getchildnode(envNode, body);
                                        free(body);
                                }
                                else {
                                        /* No prefix */
                                        bodyNode = cg_xml_node_getchildnode(envNode, CG_SOAP_BODY);
                                }
                                /* Free memory before leaving the loop */
                                cg_string_tokenizer_delete(tok);
                                free(name);
                                break;
                        }
                }
                cg_string_tokenizer_delete(tok);
                free(name);
        }

	cg_log_debug_l4("Leaving...\n");
	
	return bodyNode;
}

/****************************************
* cg_soap_request_sethttprequest
****************************************/

BOOL cg_soap_request_sethttprequest(CgSoapRequest *soapReq, CgHttpRequest *httpReq)
{
	char *content;
	int contentLen;

	cg_log_debug_l4("Entering...\n");

	if (soapReq->isHttpReqCreated == TRUE)
		cg_http_request_delete(soapReq->httpReq);
	soapReq->httpReq = httpReq;
	soapReq->isHttpReqCreated = FALSE;

	content = cg_http_request_getcontent(httpReq);
	contentLen = cg_http_request_getcontentlength(httpReq);

	if (content == NULL || contentLen <=0)
		return FALSE;

	cg_log_debug_l4("Leaving...\n");
	
	return cg_soap_request_parsemessage(soapReq, content, contentLen);
}

/****************************************
* cg_soap_request_parsemessage
****************************************/

BOOL cg_soap_request_parsemessage(CgSoapRequest *soapReq, char *msg, int msgLen)
{
	CgXmlParser *xmlParser;
	BOOL parseRet;

	cg_log_debug_l4("Entering...\n");

	if (msgLen<= 0)
		return FALSE;

	xmlParser = cg_xml_parser_new();
	parseRet = cg_xml_parse(xmlParser, soapReq->rootNodeList, msg, msgLen);
	cg_xml_parser_delete(xmlParser);

	cg_log_debug_l4("Leaving...\n");
	
	return parseRet;
}

/****************************************
* cg_soap_request_post
****************************************/

CgSoapResponse *cg_soap_request_post(CgSoapRequest *soapReq, char *ipaddr, int port)
{
	CgHttpResponse *httpRes;
	char *content;
	int contentLen;
	CgXmlParser *xmlParser;
	CgHttpHeader *header = NULL;
	
	cg_log_debug_l4("Entering...\n");

	httpRes = cg_http_request_post(soapReq->httpReq, ipaddr, port);
	
	/* Check for HTTP response 405 Method Not Allowed */
	if (cg_http_response_getstatuscode(httpRes) == CG_HTTP_STATUS_METHOD_NOT_ALLOWED)
	{
		/* Status code implies that we need to use M-POST */
		cg_http_request_setmethod(soapReq->httpReq, CG_HTTP_MPOST);
		
		/* Add MAN header */
		header = cg_http_header_new();
		cg_http_header_setname(header, CG_HTTP_MAN);
		cg_http_header_setvalue(header, CG_HTTP_SOAP_MAN_VALUE);
		cg_http_packet_addheader((CgHttpPacket*)soapReq->httpReq, header);
		
		/* Change soapaction header name to include namespace */
		header = cg_http_packet_getheader((CgHttpPacket*)soapReq->httpReq, 
						  CG_HTTP_SOAP_ACTION);
		if (header != NULL)
		{
			cg_http_header_setname(header, CG_HTTP_SOAP_ACTION_WITH_NS);
		}
		
		/* New attempt */
		httpRes = cg_http_request_post(soapReq->httpReq, ipaddr, port);
	}
	
	cg_soap_response_sethttpresponse(soapReq->soapRes,httpRes);

	content = cg_http_response_getcontent(httpRes);
	contentLen = cg_http_response_getcontentlength(httpRes);
	if (content == NULL || contentLen <= 0)
		return soapReq->soapRes;

	xmlParser = cg_xml_parser_new();
	cg_xml_parse(xmlParser, soapReq->soapRes->rootNodeList, content, contentLen);
	cg_xml_parser_delete(xmlParser);

	cg_log_debug_l4("Leaving...\n");
	
	return soapReq->soapRes;
}

/****************************************
* cg_soap_request_setcontent
****************************************/

void cg_soap_request_setcontent(CgSoapRequest *soapReq, CgXmlNode *node)
{
	CgHttpRequest *httpReq;
		
	cg_log_debug_l4("Entering...\n");

	httpReq = cg_soap_request_gethttprequest(soapReq);
	
	/**** content type ****/
	cg_http_request_setcontenttype(httpReq, CG_XML_CONTENT_TYPE);
	
	/**** content ****/
	cg_http_request_appendncontent(httpReq, CG_SOAP_VERSION_HEADER,
					cg_strlen(CG_SOAP_VERSION_HEADER));
	cg_http_request_appendncontent(httpReq, CG_XML_CONTENT_LF,
					cg_strlen(CG_XML_CONTENT_LF));
	cg_xml_node_tostring(node, TRUE, httpReq->content);
		
	/**** content length ****/
	cg_http_request_setcontentlength(httpReq,
					 cg_string_length(httpReq->content));

	cg_log_debug_l4("Leaving...\n");
}
