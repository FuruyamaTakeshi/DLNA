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
*	File: ccontrolpoint_http_server.c
*
*	Revision:
*
*	07/07/05
*		- first revision
*	01/15/06 Aapo Makela
*		- Modified to resubscribe, if event is missed
*	12/13/07 Aapo Makela
*		- Added proper locking for the control point
*
******************************************************************/

#include <cybergarage/upnp/ccontrolpoint.h>
#include <cybergarage/upnp/event/cnotify.h>
#include <cybergarage/util/clog.h>

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_CONTROLPOINT)

/**
 * The function that calls all HTTP listener callback functions. Do not call
 * this from applications.
 *
 * @param httpReq The received HTTP request
 */
void cg_upnp_controlpoint_httprequestreceived(CgHttpRequest *httpReq)
{
	CgUpnpControlPoint *ctrlPoint = NULL;
	CgUpnpNotifyRequest *notifyReq = NULL;
	CgUpnpPropertyList *propList = NULL;
	CgUpnpProperty *prop = NULL;
	CgUpnpEventListenerList *eventListeners = NULL;
	char *sid = NULL;
	long seq = 0;
	long timeout = 0;
	CgUpnpDevice *dev = NULL;
	CgUpnpService *service = NULL;
	int notifyListeners = 0;
	
	cg_log_debug_l4("Entering...\n");

	ctrlPoint = (CgUpnpControlPoint *)cg_http_request_getuserdata(httpReq);

	cg_upnp_controlpoint_lock(ctrlPoint);

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	if (cg_http_request_isnotifyrequest(httpReq) == TRUE) {	
		notifyReq = cg_upnp_event_notify_request_new();
		cg_upnp_event_notify_request_sethttprequest(notifyReq, httpReq);

		/* Get service according to SID */
		sid = cg_upnp_event_notify_request_getsid(notifyReq);
		
		for (dev = cg_upnp_controlpoint_getdevices(ctrlPoint); 
		     dev != NULL;
		     dev = cg_upnp_device_next(dev))
		{
			service = cg_upnp_device_getservicebysid(dev, sid);
			if (service != NULL) break;
		}

		if (service != NULL) {
			/* We found a service */
			seq = cg_upnp_event_notify_request_getseq(notifyReq);
			
			/* Check that event key = previous + 1 */
			if (seq != 0 && seq != cg_upnp_service_geteventkey(service) + 1)
			{
				/* The sequence does not match, unsubscribe and subscribe */
				timeout = cg_upnp_service_getsubscriptiontimeout(service);
				cg_upnp_controlpoint_unsubscribe(ctrlPoint, service);
				cg_upnp_controlpoint_subscribe(ctrlPoint, service, timeout);
			} else {
				/* Wrap seq, so that assertion is true next time */
				if (seq == CG_UPNP_EVENT_MAX_SEQ) seq = 0;
				
				/* Set event key */
				cg_upnp_service_seteventkey(service, seq);
				
				notifyListeners = 1;
				propList = cg_upnp_event_notify_request_getpropertylist(notifyReq); 
				for (prop=cg_upnp_propertylist_gets(propList); 
				     prop != NULL; 
				     prop = cg_upnp_property_next(prop)) 
				{
					/* Update the service's state table from the event */
					cg_upnp_controlpoint_updatestatetablefromproperty(service, prop);
				}
			}
		}
		eventListeners = cg_upnp_controlpoint_geteventlisteners(ctrlPoint);
		cg_upnp_controlpoint_unlock(ctrlPoint);

		if (notifyListeners && propList != NULL) 
		{
			/* Notify listeners out of control point lock */
			for (prop=cg_upnp_propertylist_gets(propList); 
			     prop != NULL; 
			     prop = cg_upnp_property_next(prop)) 
			{
				cg_upnp_eventlistenerlist_notify(eventListeners, prop);
			}
		}

		cg_upnp_event_notify_request_delete(notifyReq);
		cg_http_request_postokrequest(httpReq);
		
		return;
	}
#endif
	
	cg_upnp_controlpoint_unlock(ctrlPoint);
	cg_http_request_postbadrequest(httpReq);

	cg_log_debug_l4("Leaving...\n");
}

/**
 * When an event is received, update also the associated service's
 * state table to include the evented state variable.
 *
 * @param service The service, whose state table to update
 * @param prop The evented property from which to update
 */
void cg_upnp_controlpoint_updatestatetablefromproperty(CgUpnpService* service,
						       CgUpnpProperty* prop)
{
	CgUpnpStateVariable* var = NULL;
	
	cg_log_debug_l4("Entering...\n");

	if (service == NULL || prop == NULL)
	{
		return;
	}
	
	var = cg_upnp_service_getstatevariablebyname(service,
					       cg_upnp_property_getname(prop));
	
	if (var)
	{
		cg_upnp_statevariable_setvalue(var,
					       cg_upnp_property_getvalue(prop));
	}

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (End)
****************************************/

#endif
