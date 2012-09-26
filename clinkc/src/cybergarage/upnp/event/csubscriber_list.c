/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006-2007 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: csubscriber_list.c
*
*	Revision:
*
*	06/21/05
*		- first revision
*	04/03/06
*		- added search for SID 
*	10/22/07 Aapo Makela
*		- Fixed search for SID
*	
******************************************************************/

#include <cybergarage/upnp/event/csubscriber.h>
#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/util/clog.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* cg_upnp_subscriberlist_new
****************************************/

CgUpnpSubscriberList *cg_upnp_subscriberlist_new()
{
	CgUpnpSubscriberList *subscriberList;

	cg_log_debug_l4("Entering...\n");

	subscriberList = (CgUpnpSubscriberList *)malloc(sizeof(CgUpnpSubscriberList));

	if ( NULL != subscriberList )
		cg_list_header_init((CgList *)subscriberList);

	return subscriberList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_subscriberlist_delete
****************************************/

void cg_upnp_subscriberlist_delete(CgUpnpSubscriberList *subscriberList)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_subscriberlist_clear(subscriberList);
	free(subscriberList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_subscriberlist_get
****************************************/

CgUpnpSubscriber *cg_upnp_subscriberlist_get(CgUpnpSubscriberList *subscriberList, char *sid)
{
	CgUpnpSubscriber *sub;
	int uuidIdx;

	if (cg_strlen(sid) <= 0)
		return NULL;
	
	uuidIdx = cg_strstr(sid, CG_UPNP_ST_UUID_DEVICE);
	if (0 <= uuidIdx)
		sid += (uuidIdx + cg_strlen(CG_UPNP_ST_UUID_DEVICE) + 1);

	for (sub = cg_upnp_subscriberlist_gets(subscriberList); sub != NULL; sub = cg_upnp_subscriber_next(sub)) {
		if (cg_streq(sid, cg_upnp_subscriber_getsid(sub)) == TRUE)
			return sub;
	}
	
	return NULL;
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
