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
*	File: cservice_notify.c
*
*	Revision:
*
*	06/21/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cservice.h>
#include <cybergarage/util/clog.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* cg_upnp_service_notifymain
****************************************/

static BOOL cg_upnp_service_notifymain(CgUpnpService *service, CgUpnpStateVariable *statVar)
{
	CgUpnpSubscriber *sub;
	CgUpnpSubscriber **subArray;
	int subArrayCnt;
	int n;
		
	cg_log_debug_l4("Entering...\n");

	cg_upnp_service_lock(service);

	/**** Remove expired subscribers ****/
	subArrayCnt = cg_upnp_service_getnsubscribers(service);
	subArray = (CgUpnpSubscriber **)malloc(sizeof(CgUpnpSubscriber *) * subArrayCnt);

	if ( NULL == subArray )
	{
		cg_log_debug_s("Memory allocation problem!\n");
		cg_upnp_service_unlock(service);
		return FALSE;
	}

	sub = cg_upnp_service_getsubscribers(service);
	for (n=0; n<subArrayCnt; n++) {
		subArray[n] = sub;
		sub = cg_upnp_subscriber_next(sub);
	}
	for (n=0; n<subArrayCnt; n++) {
		sub = subArray[n];
		if (sub == NULL)
			continue;
		if (cg_upnp_subscriber_isexpired(sub) == TRUE)
			cg_upnp_service_removesubscriber(service, sub);
	}
	free(subArray);
		
	/**** Notify to subscribers ****/
	subArrayCnt = cg_upnp_service_getnsubscribers(service);
	subArray = (CgUpnpSubscriber **)malloc(sizeof(CgUpnpSubscriber *) * subArrayCnt);

	if ( NULL == subArray ) {
		cg_log_debug_s("Memory allocation problem!\n");
		cg_upnp_service_unlock(service);
		return FALSE;
	}

	sub = cg_upnp_service_getsubscribers(service);
	for (n=0; n<subArrayCnt; n++) {
		subArray[n] = sub;
		sub = cg_upnp_subscriber_next(sub);
	}
	for (n=0; n<subArrayCnt; n++) {
		sub = subArray[n];
		if (sub == NULL)
			continue;
		if (statVar) {
			if (cg_upnp_subscriber_notify(sub, statVar) == FALSE) {
				/**** remove invalid the subscriber but don't remove in NMPR specification ****/
				cg_upnp_service_removesubscriber(service, sub);
			}
		}
		else {
			if (cg_upnp_subscriber_notifyall(sub, service) == FALSE) {
				/**** remove invalid the subscriber but don't remove in NMPR specification ****/
				cg_upnp_service_removesubscriber(service, sub);
			}
		}
	}
	free(subArray);
	
	cg_upnp_service_unlock(service);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_upnp_service_notify
****************************************/

BOOL cg_upnp_service_notify(CgUpnpService *service, CgUpnpStateVariable *statVar)
{
	return cg_upnp_service_notifymain(service, statVar);
}

/****************************************
* cg_upnp_service_notifyall
****************************************/

BOOL cg_upnp_service_notifyallbracket(CgUpnpService *service)
{
	return cg_upnp_service_notifymain(service, NULL);
}

/****************************************
* cg_upnp_service_notifyall
****************************************/

BOOL cg_upnp_service_notifyall(CgUpnpService *service, BOOL doBracket)
{
	CgUpnpStateVariable *statVar;
	
	cg_log_debug_l4("Entering...\n");

	if (doBracket) {
		cg_upnp_service_notifyallbracket(service);
	}
	else {
		for (statVar = cg_upnp_service_getstatevariables(service); statVar != NULL; statVar = cg_upnp_statevariable_next(statVar)) {
			if (cg_upnp_statevariable_issendevents(statVar) == TRUE)
				cg_upnp_service_notify(service, statVar);
		}
	}

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
 * cg_upnp_service_notifyall
 ****************************************/

static void cg_upnp_service_notifyall_thread(CgThread *thread)
{
	CgUpnpService *service;
	
	service = (CgUpnpService *)cg_thread_getuserdata(thread);
	cg_upnp_service_notifyall(service, TRUE);
	cg_thread_delete(thread);	
}

void cg_upnp_service_createnotifyallthread(CgUpnpService *service, CgSysTime waitTime)
{
	CgThread *thread;
	
	thread = cg_thread_new();
	cg_thread_setaction(thread, cg_upnp_service_notifyall_thread);
	cg_thread_setuserdata(thread, service);
	
	cg_wait(waitTime);
	cg_thread_start(thread);
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
