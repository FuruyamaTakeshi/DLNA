/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: csubscriber.c
*
*	Revision:
*
*	02/01/05
*		- first revision
*	12-Jan-06 Heikki Junnila
*		- Added API comments
*	01/12/06 Theo Beisch
*		- modified time() to come from util/ctime
*		  for WINCE OS support		
*	04/04/06 Theo Beisch
*		- subscriber renew does not reset notifyCount
*		- subscriber clear does reset notifyCount
*		- expiry check is by [s] (removed *1000 factor)
*	03/13/08
*		- Changed cg_upnp_subscriber_notifyall() using void parameter instead of CgService not to conflict the prototype defines.
*
******************************************************************/

#include <cybergarage/upnp/event/cevent.h>
#include <cybergarage/upnp/event/cnotify.h>
#include <cybergarage/upnp/cupnp_limit.h>
#include <cybergarage/util/clog.h>
#include <cybergarage/util/ctime.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/**
 * Create a new event subscriber
 */
CgUpnpSubscriber *cg_upnp_subscriber_new()
{
	CgUpnpSubscriber *sub;

	cg_log_debug_l4("Entering...\n");

	sub = (CgUpnpSubscriber *)malloc(sizeof(CgUpnpSubscriber));

	if ( NULL != sub )
	{
		cg_list_node_init((CgList *)sub);
		
		sub->sid = cg_string_new();
		sub->ifAddr = cg_string_new();
		sub->deliveryURL = cg_net_url_new();
		
		cg_upnp_subscriber_settimeout(sub, 0);
		cg_upnp_subscriber_renew(sub);
		cg_upnp_subscriber_setnotifycount(sub, 0);
	}
	
	cg_log_debug_l4("Leaving...\n");

	return sub;
}

/**
 * Destroy an event subscriber
 *
 * @param sub The event subscriber
 */
void cg_upnp_subscriber_delete(CgUpnpSubscriber *sub)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_subscriber_clear(sub);
	cg_list_remove((CgList *)sub);

	cg_string_delete(sub->sid);
	cg_string_delete(sub->ifAddr);
	cg_net_url_delete(sub->deliveryURL);
	
	free(sub);

	cg_log_debug_l4("Leaving...\n");
}

/**
 * Clear the contents of an event subscriber
 *
 * @todo Lacks implementation (is it necessary?)
 *
 * @param sub The event subscriber
 */
void cg_upnp_subscriber_clear(CgUpnpSubscriber *sub)
{
	cg_log_debug_l4("Entering...\n");

	cg_log_debug_l4("Leaving...\n");
}

/**
 * Renew a subscription. Essentially sets subscription time (duration) 
 * to zero and resets notify count (== event key).
 *
 * @param sub The event subscriber
 */
void cg_upnp_subscriber_renew(CgUpnpSubscriber *sub)
{
	cg_log_debug_l4("Entering...\n");

	//Theo Beisch use clinkc time
	cg_upnp_subscriber_setsubscriptiontime(sub, cg_getcurrentsystemtime());

	cg_log_debug_l4("Leaving...\n");
}

/**
 * Increment the event notify count by one
 *
 * @param sub The event subscriber
 * @return The new notify count
 */
long cg_upnp_subscriber_incrementnotifycount(CgUpnpSubscriber *sub)
{
	cg_log_debug_l4("Entering...\n");

	if (CG_UPNP_NOTIFY_COUNT_MAX <= sub->notifyCount)
		sub->notifyCount = 0;
	sub->notifyCount++;

	cg_log_debug_l4("Leaving...\n");

	return sub->notifyCount;
}

/**
 * Check, whether a subscriber's event subscription has been expired
 *
 * @param sub The subscriber
 * @return TRUE if the subscription has been expired; otherwise FALSE
 */
BOOL cg_upnp_subscriber_isexpired(CgUpnpSubscriber *sub)
{
	CgSysTime currTime;
	CgSysTime timeout;
	CgSysTime expiredTime;
	
	timeout = cg_upnp_subscriber_gettimeout(sub);
	if(timeout == CG_UPNP_SUBSCRIPTION_INFINITE_VALUE) 
		return FALSE; 
			
	//Theo Beisch - use clinkc function 
	currTime = cg_getcurrentsystemtime(); //returns time in s 
	expiredTime = cg_upnp_subscriber_getsubscriptiontime(sub) + (timeout); //tb: removed( *1000);
	if (expiredTime < currTime)
		return TRUE;
			
	return FALSE;

	cg_log_debug_l4("Leaving...\n");
}

/**
 * Post a notification to an event subscriber. This is called in a device.
 *
 * @param sub The event subscriber
 * @param statVar The evented state variable
 * @return TRUE if succesful; otherwise FALSE
 */
static BOOL cg_upnp_subscriber_notifymain(CgUpnpSubscriber *sub, CgUpnpService *service, CgUpnpStateVariable *statVar)
{
	char *host;
	int port;
	CgUpnpNotifyRequest *notifyReq;
	CgUpnpNotifyResponse *notifyRes;
	BOOL notifySuccess;
	
	cg_log_debug_l4("Entering...\n");

	host = cg_upnp_subscriber_getdeliveryhost(sub);
	port = cg_upnp_subscriber_getdeliveryport(sub);

	notifyReq = cg_upnp_event_notify_request_new();
	cg_upnp_event_notify_request_setpropertysetnode(notifyReq, sub, service, statVar);
	notifyRes = cg_upnp_event_notify_request_post(notifyReq, host, port);
	notifySuccess = cg_upnp_event_notify_response_issuccessful(notifyRes);

	cg_http_request_print(notifyReq->httpReq);
	cg_http_response_print(notifyRes->httpRes);
	
	cg_upnp_event_notify_request_delete(notifyReq);

	if (notifySuccess == FALSE)
		return FALSE;
		
	cg_upnp_subscriber_incrementnotifycount(sub);
			
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/**
 * Post a notification to an event subscriber. This is called in a device.
 *
 * @param sub The event subscriber
 * @param statVar The evented state variable
 * @return TRUE if succesful; otherwise FALSE
 */
BOOL cg_upnp_subscriber_notify(CgUpnpSubscriber *sub, CgUpnpStateVariable *statVar)
{
	return cg_upnp_subscriber_notifymain(sub, NULL, statVar);
}

/**
 * Post a notification to an event subscriber. This is called in a device.
 *
 * @param sub The event subscriber
 * @param service The evented service
 * @return TRUE if succesful; otherwise FALSE
 */
BOOL cg_upnp_subscriber_notifyall(CgUpnpSubscriber *sub, /* CgUpnpService */ void *service)
{
	return cg_upnp_subscriber_notifymain(sub, (CgUpnpService *)service, NULL);
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
