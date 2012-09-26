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
*	File: cdevice_http_server.c
*
*	Revision:
*
*	03/22/05
*		- first revision
*
*	18-Jan-06 Aapo Makela
*		- Added HEAD-method support
*		- Fixed to handle "Not Found" case for GET-request
*		- Fixed to unescape URIs
******************************************************************/

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/upnp/cupnp.h>
#include <cybergarage/http/chttp.h>
#include <cybergarage/net/curl.h>
#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/upnp/event/cevent.h>
#include <cybergarage/util/clog.h>

/****************************************
* prototype define for static functions
****************************************/

static void cg_upnp_device_getrequestrecieved(CgUpnpDevice *dev, CgHttpRequest *httpReq);
static void cg_upnp_device_postrequestrecieved(CgUpnpDevice *dev, CgHttpRequest *httpReq);
static void cg_upnp_device_soapactionrecieved(CgUpnpDevice *dev, CgSoapRequest *soapReq);

static void cg_upnp_device_controlrequestrecieved(CgUpnpService *service, CgSoapRequest *soapReq);
#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)
static void cg_upnp_device_actioncontrolrequestrecieved(CgUpnpService *service, CgUpnpActionRequest *actionReq);
#endif
#if !defined(CG_UPNP_NOUSE_QUERYCTRL)
static void cg_upnp_device_querycontrolrequestrecieved(CgUpnpService *service, CgUpnpQueryRequest *queryReq);
#endif

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
static void cg_upnp_device_subscriptionrecieved(CgUpnpDevice *dev, CgUpnpSubscriptionRequest *subReq);
static void cg_upnp_device_newsubscriptionrecieved(CgUpnpService *service, CgUpnpSubscriptionRequest *subReq);
static void cg_upnp_device_renewsubscriptionrecieved(CgUpnpService *service, CgUpnpSubscriptionRequest *subReq);
static void cg_upnp_device_unsubscriptionrecieved(CgUpnpService *service, CgUpnpSubscriptionRequest *subReq);
#endif

/****************************************
* cg_upnp_device_httprequestrecieved
****************************************/

void cg_upnp_device_httprequestrecieved(CgHttpRequest *httpReq)
{
	CgUpnpDevice *dev;
	CgString *unescapedUrl;
	char *url;
	
	cg_log_debug_l4("Entering...\n");

	dev = (CgUpnpDevice *)cg_http_request_getuserdata(httpReq);
	
	/* Unescape URI */
	url = cg_http_request_geturi(httpReq);
	if (0 < cg_strlen(url)) {
		unescapedUrl = cg_string_new();
		cg_net_uri_unescapestring(url, 0, unescapedUrl);
		if (0 < cg_string_length(unescapedUrl))
			cg_http_request_seturi(httpReq, cg_string_getvalue(unescapedUrl));
		cg_string_delete(unescapedUrl);
	}
	
	if (cg_http_request_isgetrequest(httpReq) == TRUE ||
	    cg_http_request_isheadrequest(httpReq) == TRUE) {
		cg_upnp_device_getrequestrecieved(dev, httpReq);
		return;
	}

	if (cg_http_request_ispostrequest(httpReq) == TRUE) {
		cg_upnp_device_postrequestrecieved(dev, httpReq);
		return;
	}

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	if (cg_http_request_issubscriberequest(httpReq) == TRUE || cg_http_request_isunsubscriberequest(httpReq) == TRUE) {
		cg_upnp_device_subscriptionrecieved(dev, httpReq);
		return;
	}
#endif

	cg_http_request_postbadrequest(httpReq);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* 
* HTTP GET REQUEST
*
****************************************/

/****************************************
* cg_upnp_device_updateurlbase
****************************************/

void cg_upnp_device_seturlbase(CgUpnpDevice *dev, char *value)
{
	CgXmlNode *rootNode;
	CgXmlNode *node;

	cg_log_debug_l4("Entering...\n");

	if (cg_upnp_device_isrootdevice(dev) == FALSE)
		return;
	
	rootNode = cg_upnp_device_getrootnode(dev);
	if (rootNode == NULL)
		return;

	node = cg_xml_node_getchildnode(rootNode, CG_UPNP_DEVICE_URLBASE_NAME);
	if (node != NULL) {
		cg_xml_node_setvalue(node, value);
		return;
	}

	node = cg_xml_node_new();
	cg_xml_node_setname(node, CG_UPNP_DEVICE_URLBASE_NAME);
	cg_xml_node_setvalue(node, value);

	cg_xml_node_addchildnode(rootNode, node);

	cg_log_debug_l4("Leaving...\n");
}

static void cg_upnp_device_updateurlbase(CgUpnpDevice *dev, char *host)
{
	char urlBase[CG_UPNP_DEVICE_URLBASE_MAXLEN];

	cg_log_debug_l4("Entering...\n");

	cg_net_gethosturl(host, cg_upnp_device_gethttpport(dev), "", urlBase, sizeof(urlBase));
	cg_upnp_device_seturlbase(dev, urlBase);

	cg_log_debug_l4("Leaving...\n");
}

static char *cg_upnp_device_getdescription(CgUpnpDevice *dev, char *ifAddr, CgString *descStr)
{
	CgXmlNode *rootNode;
	
	cg_log_debug_l4("Entering...\n");

	cg_upnp_device_lock(dev);
	
	if (cg_upnp_isnmprmode() == FALSE)
		cg_upnp_device_updateurlbase(dev, ifAddr);
	
	rootNode = cg_upnp_device_getrootnode(dev);
	
	if (rootNode != NULL) {
		cg_string_addvalue(descStr, CG_UPNP_XML_DECLARATION);
		cg_string_addvalue(descStr, "\n");
		cg_xml_node_tostring(rootNode, TRUE, descStr);	
	}
	
	cg_upnp_device_unlock(dev);
	
	return cg_string_getvalue(descStr);

	cg_log_debug_l4("Leaving...\n");
}

static void cg_upnp_device_getrequestrecieved(CgUpnpDevice *dev, CgHttpRequest *httpReq)
{
	CgString *descStr;
	char *url;
	char *ifAddr;
	CgUpnpService *embService;
	CgUpnpDevice *embDev;
	CgHttpResponse *httpRes;
	
	cg_log_debug_l4("Entering...\n");

	url = cg_http_request_geturi(httpReq);
	if (cg_strlen(url) <= 0) {
		cg_http_request_postbadrequest(httpReq);
		return;
	}

	descStr = cg_string_new();	
	ifAddr = cg_http_request_getlocaladdress(httpReq);

	cg_log_debug_s("Requested: |%s|, description: |%s|\n", url, cg_string_getvalue(dev->descriptionURI));
	if (cg_upnp_device_isdescriptionuri(dev, url) == TRUE) {
		cg_upnp_device_getdescription(dev, ifAddr, descStr);
	}
	else if ((embService = cg_upnp_device_getservicebyscpdurl(dev, url)) != NULL) {
		cg_upnp_service_getdescription(embService, descStr);
	}
	else if ((embDev = cg_upnp_device_getdevicebydescriptionuri(dev, url)) != NULL) {
		cg_upnp_device_getdescription(embDev, ifAddr, descStr);
	} else {
		/* Here we should handle Not Found case */
		cg_http_request_poststatuscode(httpReq, CG_HTTP_STATUS_NOT_FOUND);
		cg_string_delete(descStr);
		return;
	}
	
	httpRes = cg_http_response_new();
	cg_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_OK);
	cg_http_response_setcontenttype(httpRes, CG_XML_CONTENT_TYPE);
	cg_http_response_setcontent(httpRes, cg_string_getvalue(descStr));
	cg_http_response_setcontentlength(httpRes, cg_string_length(descStr));
	
	if (cg_http_request_isheadrequest(httpReq) == TRUE)
	{
		/* If the request is head request, then clear message body */
		cg_http_response_setcontent(httpRes, NULL);
	}
	
	cg_http_response_print(httpRes);
	
	cg_http_request_postresponse(httpReq, httpRes);
	cg_http_response_delete(httpRes);	

	cg_string_delete(descStr);	

	cg_log_debug_l4("Leaving...\n");
}

static void cg_upnp_device_postrequestrecieved(CgUpnpDevice *dev, CgHttpRequest *httpReq)
{
	CgSoapRequest *soapReq;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_http_request_issoapaction(httpReq) == TRUE) {
		soapReq = cg_soap_request_new();
		cg_soap_request_sethttprequest(soapReq, httpReq);
		cg_upnp_device_soapactionrecieved(dev, soapReq);
		cg_soap_request_delete(soapReq);
		return;
	}
	cg_http_request_postbadrequest(httpReq);

	cg_log_debug_l4("Leaving...\n");
}


/****************************************
* 
* SOAP REQUEST
*
****************************************/

static void cg_upnp_device_badsoapactionrecieved(CgHttpRequest *httpReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_request_postbadrequest(httpReq);

	cg_log_debug_l4("Leaving...\n");
}

static void cg_upnp_device_invalidcontrolrecieved(CgSoapRequest *soapReq, int code)
{
	CgHttpRequest *httpReq;
	CgSoapResponse *soapRes;
	CgHttpResponse *httpRes;

	cg_log_debug_l4("Entering...\n");

	httpReq = cg_soap_request_gethttprequest(soapReq);
	
	soapRes = cg_soap_response_new();
	cg_upnp_control_soap_response_setfaultresponse(soapRes, code, cg_upnp_status_code2string(code));
	httpRes = cg_soap_response_gethttpresponse(soapRes);
	cg_http_request_postresponse(httpReq, httpRes);
	cg_soap_response_delete(soapRes);

	cg_log_debug_l4("Leaving...\n");
}

static void cg_upnp_device_soapactionrecieved(CgUpnpDevice *dev, CgSoapRequest *soapReq)
{
	CgHttpRequest *httpReq;
	CgUpnpService *ctrlService;
	char *url;
	
	cg_log_debug_l4("Entering...\n");

	httpReq = cg_soap_request_gethttprequest(soapReq);
	
	url = cg_http_request_geturi(httpReq);
	cg_log_debug_s("Control url in device: %s\n", url);
	ctrlService = cg_upnp_device_getservicebycontrolurl(dev, url);
	
	if (ctrlService != NULL) {
		cg_upnp_device_controlrequestrecieved(ctrlService, soapReq);
		return;
	}
	
	cg_upnp_device_badsoapactionrecieved(httpReq);

	cg_log_debug_l4("Leaving...\n");
}

static void cg_upnp_device_controlrequestrecieved(CgUpnpService *service, CgSoapRequest *soapReq)
{
	CgHttpRequest *httpReq;
	CgUpnpActionRequest *actionReq;
	CgUpnpQueryRequest *queryReq;
	
	cg_log_debug_l4("Entering...\n");

	httpReq = cg_soap_request_gethttprequest(soapReq);
	
#if !defined(CG_UPNP_NOUSE_QUERYCTRL)
	if (cg_upnp_control_isqueryrequest(soapReq) == TRUE) {
		queryReq = cg_upnp_control_query_request_new();
		cg_upnp_control_query_request_setsoaprequest(queryReq, soapReq);
		cg_upnp_device_querycontrolrequestrecieved(service, queryReq);
		cg_upnp_control_query_request_delete(queryReq);
		return;
	}
#endif
	
#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)
	if (cg_upnp_control_isactionrequest(soapReq) == TRUE) {
		actionReq = cg_upnp_control_action_request_new();
		cg_upnp_control_action_request_setsoaprequest(actionReq, soapReq);
		cg_upnp_device_actioncontrolrequestrecieved(service, actionReq);
		cg_upnp_control_action_request_delete(actionReq);
		return;
	}
#endif
	
	cg_upnp_device_badsoapactionrecieved(httpReq);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* 
* SOAP REQUEST (Action)
*
****************************************/

#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)

#define cg_upnp_device_invalidactioncontrolrecieved(actionReq) cg_upnp_device_invalidcontrolrecieved(cg_upnp_control_action_request_getsoaprequest(actionReq), CG_UPNP_STATUS_INVALID_ACTION)

static void cg_upnp_device_actioncontrolrequestrecieved(CgUpnpService *service, CgUpnpActionRequest *actionReq)
{
	char *actionName;
	CgUpnpAction *action;
	CgUpnpArgumentList *actionArgList;
	CgUpnpArgumentList *actionReqArgList;
	
	cg_log_debug_l4("Entering...\n");

	actionName = cg_upnp_control_action_request_getactionname(actionReq);
	action = cg_upnp_service_getactionbyname(service, actionName);
	if (action == NULL) {
		cg_upnp_device_invalidactioncontrolrecieved(actionReq);
		return;
	}
	
	actionArgList = cg_upnp_action_getargumentlist(action);
	actionReqArgList = cg_upnp_control_action_request_getargumentlist(actionReq);
	cg_upnp_argumentlist_set(actionArgList, actionReqArgList);
	if (cg_upnp_action_performlistner(action, actionReq) == FALSE)
		cg_upnp_device_invalidactioncontrolrecieved(actionReq);

	cg_log_debug_l4("Leaving...\n");
}

#endif

/****************************************
* 
* SOAP REQUEST (Query)
*
****************************************/

#if !defined(CG_UPNP_NOUSE_QUERYCTRL)

#define cg_upnp_device_invalidquerycontrolrecieved(queryReq) cg_upnp_device_invalidcontrolrecieved(cg_upnp_control_query_request_getsoaprequest(queryReq), CG_UPNP_STATUS_INVALID_VAR)

static void cg_upnp_device_querycontrolrequestrecieved(CgUpnpService *service, CgUpnpQueryRequest *queryReq)
{
	char *varName;
	CgUpnpStateVariable *stateVar;
	
	cg_log_debug_l4("Entering...\n");

	varName = cg_upnp_control_query_request_getvarname(queryReq);
	if (cg_upnp_service_hasstatevariablebyname(service, varName) == FALSE) {
		cg_upnp_device_invalidquerycontrolrecieved(queryReq);
		return;
	}

	stateVar = cg_upnp_service_getstatevariablebyname(service, varName);
	if (cg_upnp_statevariable_performlistner(stateVar, queryReq) == FALSE)
		cg_upnp_device_invalidquerycontrolrecieved(queryReq);

	cg_log_debug_l4("Leaving...\n");
}

#endif

/****************************************
* 
* SOAP REQUEST (SubScribe)
*
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

static void cg_upnp_device_badsubscriptionrecieved(CgUpnpSubscriptionRequest *subReq, int code)
{
	CgUpnpSubscriptionResponse *subRes;
	
	cg_log_debug_l4("Entering...\n");

	subRes = cg_upnp_event_subscription_response_new();
	cg_upnp_event_subscription_response_setstatuscode(subRes, code);
	cg_upnp_event_subscription_request_postresponse(subReq, subRes);
	cg_upnp_event_subscription_response_delete(subRes);

	cg_log_debug_l4("Leaving...\n");
}

static void cg_upnp_device_subscriptionrecieved(CgUpnpDevice *dev, CgUpnpSubscriptionRequest *subReq)
{
	char *uri;
	CgUpnpService *service;
	
	cg_log_debug_l4("Entering...\n");

	uri = cg_http_request_geturi(subReq);
	if (cg_strlen(uri) <= 0) {
		cg_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		return;
	}
	service = cg_upnp_device_getservicebyeventsuburl(dev, uri);
	if (service == NULL) {
		cg_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		return;
	}
		
	if (cg_upnp_event_subscription_request_hascallback(subReq) == FALSE && cg_upnp_event_subscription_request_hassid(subReq) == FALSE) {
		cg_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		return;
	}

        if (cg_upnp_event_subscription_request_hascallback(subReq) &&
            cg_strlen(cg_upnp_event_subscription_request_getcallback(subReq)) <= 0) {
		cg_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		return;
	}

        if (cg_upnp_event_subscription_request_hassid(subReq) && 
            (cg_upnp_event_subscription_request_hascallback(subReq) ||
             cg_upnp_event_subscription_request_hasnt(subReq))) {
                cg_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_BAD_REQUEST);
		return;
        }

	if (cg_upnp_event_subscription_request_hasnt(subReq) &&
            (cg_strcmp(cg_upnp_event_subscription_request_getnt(subReq), CG_UPNP_NT_EVENT) != 0)) {
                cg_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		return;
        }

	/**** UNSUBSCRIBE ****/
	if (cg_upnp_event_subscription_isunsubscriberequest(subReq) == TRUE) {
		cg_upnp_device_unsubscriptionrecieved(service, subReq);
		return;
	}

	/**** SUBSCRIBE (NEW) ****/
	if (cg_upnp_event_subscription_request_hascallback(subReq) == TRUE) {
		cg_upnp_device_newsubscriptionrecieved(service, subReq);
		return;
	}
		
	/**** SUBSCRIBE (RENEW) ****/
	if (cg_upnp_event_subscription_request_hassid(subReq) == TRUE) {
		cg_upnp_device_renewsubscriptionrecieved(service, subReq);
		return;
	}

	cg_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);

	cg_log_debug_l4("Leaving...\n");
}

static void cg_upnp_device_newsubscriptionrecieved(CgUpnpService *service, CgUpnpSubscriptionRequest *subReq)
{
	char *callback;
	char *aux;
	long timeout;
	char sid[CG_UPNP_SUBSCRIPTION_SID_SIZE];
	CgUpnpSubscriber *sub;
	CgUpnpSubscriptionResponse *subRes;
	
	cg_log_debug_l4("Entering...\n");

	aux = cg_strdup(cg_upnp_event_subscription_request_getcallback(subReq));
	if (aux == NULL)
		return;
	callback = cg_strltrim(aux, CG_UPNP_SUBSCRIPTION_CALLBACK_START_WITH, 1);
	cg_strrtrim(callback, CG_UPNP_SUBSCRIPTION_CALLBACK_END_WITH, 1);
	
	timeout = cg_upnp_event_subscription_request_gettimeout(subReq);
	/* Limit timeout to the given maximum */
	if (CG_UPNP_SUBSCRIPTION_MAX_TIMEOUT > 0)
	{
		if (timeout > CG_UPNP_SUBSCRIPTION_MAX_TIMEOUT ||
		    timeout == CG_UPNP_SUBSCRIPTION_INFINITE_VALUE)
		{
			timeout = CG_UPNP_SUBSCRIPTION_MAX_TIMEOUT;
		}
	}
	
	cg_upnp_event_subscription_createsid(sid, sizeof(sid));

	sub = cg_upnp_subscriber_new();
	cg_upnp_subscriber_setdeliveryurl(sub, callback);
	free(aux);
	cg_upnp_subscriber_settimeout(sub, timeout);
	cg_upnp_subscriber_setsid(sub, sid);
	cg_upnp_service_addsubscriber(service, sub);

	subRes = cg_upnp_event_subscription_response_new();
	cg_upnp_event_subscription_subscriberesponse_setresponse(subRes, CG_HTTP_STATUS_OK);
	cg_upnp_event_subscription_response_setsid(subRes, sid);
	cg_upnp_event_subscription_response_settimeout(subRes, timeout);
	cg_upnp_event_subscription_request_postresponse(subReq, subRes);
	cg_upnp_event_subscription_response_delete(subRes);

	cg_upnp_service_createnotifyallthread(service, CG_UPNP_SERVICE_NOTIFY_WAITTIME);

	cg_log_debug_l4("Leaving...\n");
}

static void cg_upnp_device_renewsubscriptionrecieved(CgUpnpService *service, CgUpnpSubscriptionRequest *subReq)
{
	CgUpnpSubscriber *sub;
	char *sid;
	long timeout;
	CgUpnpSubscriptionResponse *subRes;
	
	cg_log_debug_l4("Entering...\n");

	sid = cg_upnp_event_subscription_request_getsid(subReq);
	sub = cg_upnp_service_getsubscriberbysid(service, sid);

	if (sub == NULL) {
		cg_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		return;
	}
	
	timeout = cg_upnp_event_subscription_request_gettimeout(subReq);
	/* Limit timeout to the given maximum */
	if (CG_UPNP_SUBSCRIPTION_MAX_TIMEOUT > 0)
	{
		if (timeout > CG_UPNP_SUBSCRIPTION_MAX_TIMEOUT ||
		    timeout == CG_UPNP_SUBSCRIPTION_INFINITE_VALUE)
		{
			timeout = CG_UPNP_SUBSCRIPTION_MAX_TIMEOUT;
		}
	}
	
	cg_upnp_subscriber_settimeout(sub, timeout);
	cg_upnp_subscriber_renew(sub);

	subRes = cg_upnp_event_subscription_response_new();
	cg_upnp_event_subscription_subscriberesponse_setresponse(subRes, CG_HTTP_STATUS_OK);
	cg_upnp_event_subscription_response_setsid(subRes, sid);
	cg_upnp_event_subscription_response_settimeout(subRes, timeout);
	cg_upnp_event_subscription_request_postresponse(subReq, subRes);
	cg_upnp_event_subscription_response_delete(subRes);

	cg_log_debug_l4("Leaving...\n");
}		

static void cg_upnp_device_unsubscriptionrecieved(CgUpnpService *service, CgUpnpSubscriptionRequest *subReq)
{
	CgUpnpSubscriber *sub;
	char *sid;
	CgUpnpSubscriptionResponse *subRes;
	
	cg_log_debug_l4("Entering...\n");

	sid = cg_upnp_event_subscription_request_getsid(subReq);
	cg_upnp_service_lock(service);
	sub = cg_upnp_service_getsubscriberbysid(service, sid);

	if (sub == NULL) {
		cg_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		cg_upnp_service_unlock(service);
		return;
	}
	
	cg_upnp_service_removesubscriber(service, sub);
	cg_upnp_service_unlock(service);
	
	subRes = cg_upnp_event_subscription_response_new();
	cg_upnp_event_subscription_subscriberesponse_setresponse(subRes, CG_HTTP_STATUS_OK);
	cg_upnp_event_subscription_request_postresponse(subReq, subRes);
	cg_upnp_event_subscription_response_delete(subRes);

	cg_log_debug_l4("Leaving...\n");
}		

#endif
