/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license with patent exclusion,
*       see file COPYING.
*
*	File: ccontrolpoint_event.c
*
*	Revision:
*
*	07/13/05
*		- first revision
*	08/25/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Change the following functions to set HOTS header according to UPnP Spec HOST.
*		  cg_upnp_controlpoint_subscribe()
*		  cg_upnp_controlpoint_resubscribe()
*		  cg_upnp_controlpoint_unsubscribe()
*		- Changed cg_upnp_controlpoint_geteventsubcallbackurl() to add < and > around callbackurl per UPnP def.
*	17-Jan-06 Aapo Makela
*		- Added expiration handler thread
*	04/24/07 Aapo Makela
*		- Do not set host in subscription request (it is handled automatically when sending request)
******************************************************************/

#include <cybergarage/upnp/cupnp_limit.h>
#include <cybergarage/upnp/ccontrolpoint.h>
#include <cybergarage/util/clog.h>

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_CONTROLPOINT) && !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

CgUpnpService *cg_upnp_device_getfirstexpiratedservice(CgUpnpControlPoint *ctrlPoint,
						       CgUpnpDevice *dev, 
						       CgSysTime expirationTime)
{
	CgUpnpDevice *childDev = NULL;
	CgUpnpService *srv = NULL;
	CgUpnpService *found_srv = NULL;
	CgSysTime tmpTime;
	CgSysTime currTime;
	long timeout;
	
	cg_log_debug_l4("Entering...\n");

	tmpTime = 0;
	currTime = cg_getcurrentsystemtime();
	
	for (srv = cg_upnp_device_getservices(dev); srv != NULL;
	     srv = cg_upnp_service_next(srv))
	{
		if (cg_upnp_service_issubscribed(srv) == FALSE) continue;
	
		tmpTime = cg_upnp_service_getsubscriptionexpiration(srv);
		
	cg_log_debug_s("Found subscribed service with expiration of  %ld\n",
			tmpTime);
		
		if (tmpTime > 0 && tmpTime <= currTime)
		{
		cg_log_debug_s("Trying to resubscribe!\n");
			/* Subscription has almost expired! resubscribe */
			timeout = cg_upnp_service_getsubscriptiontimeout(srv);
			if (cg_upnp_controlpoint_resubscribe(ctrlPoint, srv, timeout) == FALSE)
			{
			cg_log_debug_s("Resubscription unsuccesful!\n");
				tmpTime = 0;
			} else {
			cg_log_debug_s("Resubscription succesful!\n");
				tmpTime = cg_upnp_service_getsubscriptionexpiration(srv);
			}
			
		} 
		
		if (tmpTime > 0 && tmpTime < expirationTime)
		{
			expirationTime = tmpTime;
			found_srv = srv;
		}
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL;
	     childDev = cg_upnp_device_next(childDev))
	{
		srv = cg_upnp_device_getfirstexpiratedservice(ctrlPoint,
							      childDev,
							      expirationTime);
		
		if (srv != NULL) found_srv = srv;
	}
	
	cg_log_debug_l4("Leaving...\n");

	return found_srv;
}


void cg_upnp_controlpoint_expirationhandler(CgThread *thread)
{
	CgUpnpControlPoint *ctrlPoint;
	CgUpnpDevice *dev, *tmpDev;
	CgUpnpService *srv;
	CgUpnpSSDPPacket *ssdpPkt;
	CgSysTime currTime;
	CgSysTime expirationTime;
	CgSysTime tmpTime;
	long timeout;
	
	cg_log_debug_l4("Entering...\n");

	ctrlPoint = (CgUpnpControlPoint *)cg_thread_getuserdata(thread);
	cg_log_debug_s("CtrlPoint: %p\n", ctrlPoint);
	
	cg_mutex_lock(ctrlPoint->expMutex);
	
	while (cg_thread_isrunnable(thread) == TRUE)
	{
		currTime = cg_getcurrentsystemtime();
		cg_log_debug_s("Current time: %ld\n", currTime);
		expirationTime = 0;
		tmpTime = 0;
		
		/* Get expirations from all services and devices */
		cg_mutex_unlock(ctrlPoint->expMutex);
		cg_upnp_controlpoint_lock(ctrlPoint);
		
		for (dev = cg_upnp_controlpoint_getdevices(ctrlPoint);
		     dev != NULL; dev = cg_upnp_device_next(dev))
		{
			/* Check device expiration */
			cg_log_debug_s("Checking device expiration.\n");
			ssdpPkt = cg_upnp_device_getssdppacket(dev);
			if (ssdpPkt != NULL)
			{
				tmpTime = cg_upnp_ssdp_packet_getexpiration(ssdpPkt);
				cg_log_debug_s("expiration for device %s: %ld\n", 
				cg_upnp_device_getfriendlyname(dev),
				cg_upnp_ssdp_packet_getexpiration(ssdpPkt));
				
				if (tmpTime > 0 && tmpTime <= currTime)
				{
					/* Device has already expired, remove it! */
					tmpDev = dev;
					dev = cg_upnp_device_next(tmpDev);
					
					cg_upnp_controlpoint_unlock(ctrlPoint);
					cg_upnp_controlpoint_removedevicebyssdppacket(
						ctrlPoint,
						cg_upnp_device_getssdppacket(tmpDev));
					cg_upnp_controlpoint_lock(ctrlPoint);
					
					tmpDev = NULL;
					
					/* Break out of loop */
					if (dev == NULL) break;
						
				} else if (tmpTime > 0 && 
					   (tmpTime < expirationTime || 
					    expirationTime == 0))
				{
					/* Mark expirationTime */
					expirationTime = tmpTime;
				}
			}
			
			/* Get service expiration */
			srv = cg_upnp_device_getfirstexpiratedservice(ctrlPoint, dev, expirationTime);
			
			if (srv != NULL) 
			{
				expirationTime = cg_upnp_service_getsubscriptionexpiration(srv);
			}
		}
		
		cg_upnp_controlpoint_unlock(ctrlPoint);
		cg_mutex_lock(ctrlPoint->expMutex);
		
		/* Now we know, how much we should sleep */
		if (expirationTime > currTime)
		{
			currTime = cg_getcurrentsystemtime();
			timeout = (long)(expirationTime - currTime);
			cg_log_debug_s("Sleeping for %ld sec\n", timeout);
		} else {
			timeout = 0;
			cg_log_debug_s("Sleeping until awaken\n");
		}
		
		if (cg_thread_isrunnable(thread) == FALSE) break;
		cg_cond_wait(ctrlPoint->expCond, ctrlPoint->expMutex, timeout);
		cg_log_debug_s("Sleep over!\n");
		
		/* Check the time after the wait */
		currTime = cg_getcurrentsystemtime();
		if (currTime < expirationTime)
		{
			cg_log_debug_s("New device or subscription! Or someone adjusted the clock!\n");
		}
	}
	cg_mutex_unlock(ctrlPoint->expMutex);
	
	cg_log_debug_l4("Leaving...\n");
}


/****************************************
* cg_upnp_controlpoint_resubscribe
****************************************/

BOOL cg_upnp_controlpoint_resubscribe(CgUpnpControlPoint *ctrlPoint, CgUpnpService *service, long timeout)
{
	CgUpnpDevice *rootDev;
	CgUpnpSubscriptionRequest *subReq;
	CgUpnpSubscriptionResponse *subRes;
	BOOL isSuccess;

	cg_log_debug_l4("Entering...\n");

	if (cg_upnp_service_issubscribed(service) == FALSE)
		return FALSE;

	rootDev = cg_upnp_service_getrootdevice(service);
	if (rootDev == NULL)
		return FALSE;

	subReq = cg_upnp_event_subscription_request_new();
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	cg_upnp_event_subscription_request_setrenewsubscription(subReq, service, cg_upnp_service_getsubscriptionsid(service), timeout);
	subRes = cg_upnp_event_subscription_request_post(subReq);
	isSuccess = cg_upnp_event_subscription_response_issuccessful(subRes);
	if (isSuccess == TRUE) {
		cg_upnp_service_setsubscriptionsid(service, cg_upnp_event_subscription_response_getsid(subRes));
		cg_upnp_service_setsubscriptiontimeout(service, cg_upnp_event_subscription_response_gettimeout(subRes));
		cg_upnp_service_setsubscriptiontimestamp(service, cg_getcurrentsystemtime());
	}
	else
		cg_upnp_service_clearsubscriptionsid(service);

	cg_upnp_event_subscription_request_delete(subReq);

	cg_log_debug_l4("Leaving...\n");

	return isSuccess;
}

/****************************************
* cg_upnp_controlpoint_subscribe
****************************************/

BOOL cg_upnp_controlpoint_subscribe(CgUpnpControlPoint *ctrlPoint, CgUpnpService *service, long timeout)
{
	CgUpnpDevice *rootDev;
	char *roodDevIfAddress;
	CgUpnpSubscriptionRequest *subReq;
	CgUpnpSubscriptionResponse *subRes;
	char eventSubURL[CG_UPNP_CONTROLPOINT_EVENTSUBURL_MAX];
	BOOL isSuccess;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_upnp_service_issubscribed(service) == TRUE)
		return cg_upnp_controlpoint_resubscribe(ctrlPoint, service, timeout);

	rootDev = cg_upnp_service_getrootdevice(service);
	if (rootDev == NULL)
		return FALSE;

#ifdef CG_OPTIMIZED_CP_MODE
	if (cg_upnp_service_isparsed(service) == FALSE)
				cg_upnp_controlpoint_parsescservicescpd(service); 
#endif
	
	cg_upnp_service_lock(service);
	roodDevIfAddress = cg_upnp_device_getinterfaceaddressfromssdppacket(rootDev);

	cg_log_debug_s("roodevifaddress = %s\n", roodDevIfAddress);

	subReq = cg_upnp_event_subscription_request_new();
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	cg_upnp_event_subscription_request_setnewsubscription(subReq, service, cg_upnp_controlpoint_geteventsubcallbackurl(ctrlPoint, roodDevIfAddress, eventSubURL, sizeof(eventSubURL)), timeout);
	/* Set the event key to zero before any events are received */
	cg_upnp_service_seteventkey(service, 0);
	subRes = cg_upnp_event_subscription_request_post(subReq);
	
	isSuccess = cg_upnp_event_subscription_response_issuccessful(subRes);
	if (isSuccess == TRUE) {
		cg_upnp_service_setsubscriptionsid(service, cg_upnp_event_subscription_response_getsid(subRes));
		cg_upnp_service_setsubscriptiontimeout(service, cg_upnp_event_subscription_response_gettimeout(subRes));
		cg_upnp_service_setsubscriptiontimestamp(service, cg_getcurrentsystemtime());
		
		/* New subscription, wake up expirationhandler thread */
		cg_mutex_lock(ctrlPoint->expMutex);
		cg_cond_signal(ctrlPoint->expCond);
		cg_mutex_unlock(ctrlPoint->expMutex);
	}
	else
		cg_upnp_service_clearsubscriptionsid(service);

	cg_upnp_event_subscription_request_delete(subReq);
	cg_upnp_service_unlock(service);
	
	cg_log_debug_l4("Leaving...\n");
	
	return isSuccess;
}

/****************************************
* cg_upnp_event_subscription_request_setunsubscription
****************************************/

BOOL cg_upnp_controlpoint_unsubscribe(CgUpnpControlPoint *ctrlPoint, CgUpnpService *service)
{
	CgUpnpDevice *rootDev;
	CgUpnpSubscriptionRequest *subReq;
	CgUpnpSubscriptionResponse *subRes;
	BOOL isSuccess;

	cg_log_debug_l4("Entering...\n");

	rootDev = cg_upnp_service_getrootdevice(service);
	if (rootDev == NULL)
		return FALSE;

	subReq = cg_upnp_event_subscription_request_new();
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	cg_upnp_event_subscription_request_setunsubscription(subReq, service);
	subRes = cg_upnp_event_subscription_request_post(subReq);
	isSuccess = cg_upnp_event_subscription_response_issuccessful(subRes);
	
	cg_upnp_service_clearsubscriptionsid(service);
	cg_upnp_service_setsubscriptiontimeout(service, 0);
	cg_upnp_service_setsubscriptiontimestamp(service, 0);
	
	cg_upnp_event_subscription_request_delete(subReq);

	cg_log_debug_l4("Leaving...\n");

	return isSuccess;
}

/****************************************
* cg_upnp_controlpoint_subscribeall
****************************************/

BOOL cg_upnp_controlpoint_subscribeall(CgUpnpControlPoint *ctrlPoint, CgUpnpDevice *dev, long timeout)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	BOOL isSuccess;

	cg_log_debug_l4("Entering...\n");

	isSuccess = FALSE;

	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_upnp_controlpoint_subscribe(ctrlPoint, service, timeout) == TRUE)
			isSuccess = TRUE;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(dev)) {
		if (cg_upnp_controlpoint_subscribeall(ctrlPoint, childDev, timeout) == TRUE)
			isSuccess = TRUE;
	}

	cg_log_debug_l4("Leaving...\n");

	return isSuccess;
}

/****************************************
* cg_upnp_controlpoint_resubscribeall
****************************************/

BOOL cg_upnp_controlpoint_resubscribeall(CgUpnpControlPoint *ctrlPoint, CgUpnpDevice *dev, long timeout)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	BOOL isSuccess;

	cg_log_debug_l4("Entering...\n");

	isSuccess = FALSE;

	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_upnp_controlpoint_resubscribe(ctrlPoint, service, timeout) == TRUE)
			isSuccess = TRUE;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(dev)) {
		if (cg_upnp_controlpoint_resubscribeall(ctrlPoint, childDev, timeout) == TRUE)
			isSuccess = TRUE;
	}

	cg_log_debug_l4("Leaving...\n");

	return isSuccess;
}

/****************************************
* cg_upnp_controlpoint_unsubscribeall
****************************************/

BOOL cg_upnp_controlpoint_unsubscribeall(CgUpnpControlPoint *ctrlPoint, CgUpnpDevice *dev)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	BOOL isSuccess;

	cg_log_debug_l4("Entering...\n");

	isSuccess = TRUE;

	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_upnp_service_issubscribed(service) == TRUE)
		{
			if (cg_upnp_controlpoint_unsubscribe(ctrlPoint, service) == FALSE)
				isSuccess = FALSE;
		}
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		if (cg_upnp_controlpoint_unsubscribeall(ctrlPoint, childDev) == FALSE)
			isSuccess = FALSE;
	}

	cg_log_debug_l4("Leaving...\n");

	return isSuccess;
}

/*

bool ControlPoint::isSubscribed(Service *service)
{
	cg_log_debug_l4("Entering...\n");

	if (service == NULL)
		return false;
	return service->isSubscribed();

	cg_log_debug_l4("Leaving...\n");
}

void ControlPoint::unsubscribe(Device *device)
{
	cg_log_debug_l4("Entering...\n");

	int n;

	ServiceList *serviceList = device->getServiceList();
	int serviceCnt = serviceList->size();
	for (n=0; n<serviceCnt; n++) {
		Service *service = serviceList->getService(n);
		if (service->hasSID() == true)
			unsubscribe(service);
	}

	DeviceList *childDevList = device->getDeviceList();
	int childDevCnt = childDevList->size();
	for (n=0; n<childDevCnt; n++) {
		Device *cdev = childDevList->getDevice(n);
		unsubscribe(cdev);
	}		

	cg_log_debug_l4("Leaving...\n");
}
*/

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (End)
****************************************/

#endif
