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
*	File: csubscription_response.c
*
*	Revision:
*
*	06/20/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/event/cevent.h>
#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/util/clog.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* cg_upnp_event_subscription_subscriberesponse_setresponse
****************************************/

void cg_upnp_event_subscription_subscriberesponse_setresponse(CgUpnpSubscriptionResponse *subRes, int code)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_response_setstatuscode(subRes, code);
	cg_http_response_setcontentlength(subRes, 0);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_event_subscription_response_setsid
****************************************/

void cg_upnp_event_subscription_response_setsid(CgUpnpSubscriptionResponse *subRes, char *sid)
{
	CgString *headerSID;
	int uuidIdx;
	
	cg_log_debug_l4("Entering...\n");

	headerSID = cg_string_new();

	uuidIdx = cg_strstr(sid, CG_UPNP_ST_UUID_DEVICE);
	if (uuidIdx < 0)
		cg_string_addvalue(headerSID, CG_UPNP_ST_UUID_DEVICE ":");
	cg_string_addvalue(headerSID, sid);
	
	cg_http_packet_setheadervalue(((CgHttpPacket*)subRes), CG_HTTP_SID, cg_string_getvalue(headerSID));

	cg_string_delete(headerSID);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_event_subscription_response_settimeout
****************************************/

void cg_upnp_event_subscription_response_settimeout(CgUpnpSubscriptionResponse *subRes, long value)
{
	CgString *buf;

	cg_log_debug_l4("Entering...\n");

	buf = cg_string_new();
	cg_http_packet_setheadervalue((CgHttpPacket*)subRes, CG_HTTP_TIMEOUT, cg_upnp_event_subscription_totimeoutheaderstring(value, buf));
	cg_string_delete(buf);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
