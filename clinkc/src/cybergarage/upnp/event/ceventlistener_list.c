/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Nokia Corporation 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cthread_list.c
*
*	Revision:
*
*	11/10/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/event/cevent.h>
#include <cybergarage/util/clog.h>

#include <string.h>

/**
 * Create a new event listener list
 *
 */
CgUpnpEventListenerList *cg_upnp_eventlistenerlist_new()
{
	CgUpnpEventListenerList *eventListenerList;

	cg_log_debug_l4("Entering...\n");

	eventListenerList = (CgUpnpEventListenerList *)malloc(sizeof(CgUpnpEventListenerList));

	if ( NULL != eventListenerList )
		cg_list_header_init((CgList *)eventListenerList);

	return eventListenerList;

	cg_log_debug_l4("Leaving...\n");
}

/**
 * Delete a event listener list.
 *
 * @param eventListenerList The event listener list to delete
 *
 */
void cg_upnp_eventlistenerlist_delete(CgUpnpEventListenerList *eventListenerList)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_eventlistenerlist_clear(eventListenerList);
	free(eventListenerList);

	cg_log_debug_l4("Leaving...\n");
}

/**
 * Remove a listener from the event listener list
 *
 * @param eventListenerList The event listener list
 * @param listener The listener to remove
 *
 */
void cg_upnp_eventlistenerlist_remove(CgUpnpEventListenerList* eventListenerList, CG_UPNP_EVENT_LISTENER listener)
{
	CgUpnpEventListenerList *list_node = NULL;
	
	cg_log_debug_l4("Entering...\n");

	if (listener == NULL) return;
	
	for (list_node = cg_upnp_eventlistenerlist_gets(eventListenerList);
	     list_node != NULL;
	     list_node = cg_upnp_eventlistenerlist_next(list_node))
	{
		if (list_node->listener == listener)
		{
			cg_list_remove((CgList*)list_node);
			free(list_node);
			break;
		}
	}
	

	cg_log_debug_l4("Leaving...\n");
}

/**
 * Add a listener to the event listener list
 *
 * @param eventListenerList The event listener list
 * @param listener The listener to add
 *
 */
void cg_upnp_eventlistenerlist_add(CgUpnpEventListenerList* eventListenerList, CG_UPNP_EVENT_LISTENER listener)
{
	CgUpnpEventListenerList *list_node;
	
	cg_log_debug_l4("Entering...\n");

	if (listener == NULL) return;
	
	list_node = (CgUpnpEventListenerList*)malloc(sizeof(CgUpnpEventListenerList));

	if ( NULL != list_node )
	{	
		memset(list_node, 0, sizeof(CgUpnpEventListenerList));
		list_node->listener = listener;
		cg_list_node_init((CgList *)list_node);
		cg_list_add((CgList*)eventListenerList, (CgList*)list_node);
	}
	else
		cg_log_debug_s("Memory allocation failure!\n");

	cg_log_debug_l4("Leaving...\n");
}

/**
 * Call all event listeners in the list with the given evented data.
 *
 * @param eventListenerList The list to iterate thru
 * @param property The property that has been evented
 */
void cg_upnp_eventlistenerlist_notify(CgUpnpEventListenerList* eventListenerList, CgUpnpProperty *property)
{
	CgUpnpEventListenerList *list_node = NULL;
	
	cg_log_debug_l4("Entering...\n");

	for (list_node = cg_upnp_eventlistenerlist_gets(eventListenerList);
	     list_node != NULL;
	     list_node = cg_upnp_eventlistenerlist_next(list_node))
	{
		if (list_node->listener != NULL)
		{
			list_node->listener(property);
		}
	}

	cg_log_debug_l4("Leaving...\n");
}
