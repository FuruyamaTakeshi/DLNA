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
*	File: csubscription_request.c
*
*	Revision:
*
*	07/13/05
*		- first revision
*	08/25/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Change the following functions to set HOTS header according to UPnP Spec HOST.
*		  cg_upnp_event_subscription_request_setnewsubscription()
*		  cg_upnp_event_subscription_request_setrenewsubscription()
*		  cg_upnp_event_subscription_request_setunsubscription()
*		- Change cg_upnp_event_subscription_request_setservice() to add / to preceede relative path statement (required e.g. for intel devices).
*
*	10/31/05
*		- cg_upnp_event_subscription_request_setservice:
*		  changed eventSubURLStr[1] to [0]
*
*	04/24/07 Aapo Makela
*		- Do not set host in subscription request (it is handled automatically when sending request)
******************************************************************/

#include <cybergarage/upnp/event/cevent.h>
#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/util/clog.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* cg_upnp_event_subscription_request_setsid
****************************************/

void cg_upnp_event_subscription_request_setsid(CgUpnpSubscriptionRequest *subReq, char *sid)
{
	CgString *headerSID;
	int uuidIdx;
	
	cg_log_debug_l4("Entering...\n");

	headerSID = cg_string_new();

	uuidIdx = cg_strstr(sid, CG_UPNP_ST_UUID_DEVICE);
	if (uuidIdx < 0)
		cg_string_addvalue(headerSID, CG_UPNP_ST_UUID_DEVICE ":");
	cg_string_addvalue(headerSID, sid);
	
	cg_http_packet_setheadervalue(((CgHttpPacket*)subReq), CG_HTTP_SID, cg_string_getvalue(headerSID));

	cg_string_delete(headerSID);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_event_subscription_request_settimeout
****************************************/

void cg_upnp_event_subscription_request_settimeout(CgUpnpSubscriptionRequest *subReq, long timeout)
{
	CgString *timeoutBuf;

	cg_log_debug_l4("Entering...\n");

	timeoutBuf = cg_string_new();
	cg_http_packet_setheadervalue(((CgHttpPacket*)subReq), CG_HTTP_TIMEOUT, cg_upnp_event_subscription_totimeoutheaderstring(timeout, timeoutBuf));
	cg_string_delete(timeoutBuf);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_event_subscription_request_setservice
****************************************/

static void cg_upnp_event_subscription_request_setservice(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service)
{
	CgNetURL *eventSubURL;

	cg_log_debug_l4("Entering...\n");

	eventSubURL = cg_upnp_service_geteventsuburl(service); 
	cg_http_request_seturi(subReq, cg_net_url_getrequest(eventSubURL));

	cg_net_url_delete(subReq->postURL);

	subReq->postURL = eventSubURL;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_event_subscription_request_setnewsubscription
****************************************/

void cg_upnp_event_subscription_request_setnewsubscription(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service, char *callback, long timeout)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_request_setmethod(subReq, CG_HTTP_SUBSCRIBE);
	cg_upnp_event_subscription_request_setservice(subReq, service);
	cg_upnp_event_subscription_request_setcallback(subReq, callback);
	cg_upnp_event_subscription_request_setnt(subReq, CG_UPNP_NT_EVENT);
	cg_upnp_event_subscription_request_settimeout(subReq, timeout);
	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_event_subscription_request_setrenewsubscription
****************************************/

void cg_upnp_event_subscription_request_setrenewsubscription(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service, char *uuid, long timeout)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_request_setmethod(subReq, CG_HTTP_SUBSCRIBE);
	cg_upnp_event_subscription_request_setservice(subReq, service);
	cg_upnp_event_subscription_request_setsid(subReq, uuid);
	cg_upnp_event_subscription_request_settimeout(subReq, timeout);
	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_event_subscription_request_setunsubscription
****************************************/

void cg_upnp_event_subscription_request_setunsubscription(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_request_setmethod(subReq, CG_HTTP_UNSUBSCRIBE);
	cg_upnp_event_subscription_request_setservice(subReq, service);
	cg_upnp_event_subscription_request_setsid(subReq, cg_upnp_service_getsubscriptionsid(service));
	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
