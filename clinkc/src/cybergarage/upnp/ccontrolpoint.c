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
*	File: ccontrolpoint.c
*
*	Revision:
*
*	05/26/05
*		- first revision
*	08/25/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Changed cg_upnp_controlpoint_geteventsubcallbackurl() to add < and > around callbackurl per UPnP def.
*	09/06/05
*		- Thanks for Smolander Visa <visa.smolander@nokia.com>
*		- Added cg_upnp_controlpoint_setuserdata() and cg_upnp_controlpoint_getuserdatga().
*	10/31/05
*		- Fixed severe bug in getting child devices
*	12/14/05
*		- Fixed to be more robust on opening servers
*	12/14/05
*		- Fixed cg_upnp_controlpoint_removedevicebyssdppacket() to use
*		  cg_upnp_device_delete() instead of cg_upnp_device_remove().
*
*	11-Jan-06 Heikki Junnila
*		- Removed cg_upnp_device_isname and _getbyname because
*		  according to UPnP specs UDN, type or friendlyname is
*		  not the same as the device's name.
*   
*	16-Jan-06 Aapo Makela
*		- Fixed to send many M-SEARCHes (and not too often)
*	25-Jan-05 Aapo Makela
*		- Modified to handle local IP change
*		- Modified to remove devices when CP is stopped.
******************************************************************/

#include <cybergarage/upnp/ccontrolpoint.h>
#include <cybergarage/upnp/cupnp_limit.h>
#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/util/clog.h>
#include <cybergarage/net/curi.h>

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_CONTROLPOINT)

/****************************************
 * static function defines
 ****************************************/

#if defined(CG_UPNP_USE_STDDCP)
char *cg_upnp_service_getstddcp(CgUpnpService *service);
BOOL cg_upnp_service_hasstddcp(CgUpnpService *service);
#endif

/****************************************
* static function defines
****************************************/

static void cg_upnp_controlpoint_ssdplistner(CgUpnpSSDPPacket *ssdpPkt);
static void cg_upnp_controlpoint_ssdpresponselistner(CgUpnpSSDPPacket *ssdpPkt);

/**
 * Get the event subscription callback URI
 *
 * @param ctrlPoint The control point in question
 * @param ifaddr Interface address
 * @param buf Buffer used to store callback url
 * @param bufLen Buffer length
 *
 * @return Event callback url (pointer to buf)
 */

char *cg_upnp_controlpoint_geteventsubcallbackurl(CgUpnpControlPoint *ctrlPoint, char *ifaddr, char *buf, int bufLen)
{
	cg_log_debug_l4("Entering...\n");

	/**** Thanks for Theo Beisch (2005/08/25) ****/
	return cg_net_getmodifierhosturl(ifaddr, cg_upnp_controlpoint_geteventport(ctrlPoint), cg_upnp_controlpoint_geteventsuburi(ctrlPoint), buf, bufLen, "<", ">");

	cg_log_debug_l4("Leaving...\n");
}

/****************************************************************************
 * Control Point top-level control
 ****************************************************************************/

/**
 * Create a new control point. Does not start any threads.
 *
 * @return A newly-created CgUpnpControlPoint
 */
CgUpnpControlPoint *cg_upnp_controlpoint_new()
{
	CgUpnpControlPoint *ctrlPoint;

	cg_log_debug_l4("Entering...\n");

	ctrlPoint = (CgUpnpControlPoint *)malloc(sizeof(CgUpnpControlPoint));

	if ( NULL != ctrlPoint )
	{
#ifdef CG_HTTP_USE_PERSISTENT_CONNECTIONS	
		cg_http_persistentconnection_init();
#endif
		ctrlPoint->mutex = cg_mutex_new();
		ctrlPoint->deviceRootNodeList = cg_xml_nodelist_new();
		ctrlPoint->deviceList = cg_upnp_devicelist_new();
		ctrlPoint->ssdpServerList = cg_upnp_ssdp_serverlist_new();
		ctrlPoint->ssdpResServerList = cg_upnp_ssdpresponse_serverlist_new();
		ctrlPoint->httpServerList = cg_http_serverlist_new();
		ctrlPoint->httpEventURI = cg_string_new();
		ctrlPoint->eventListeners = cg_upnp_eventlistenerlist_new();

		/* Expiration handling */
		ctrlPoint->expThread = cg_thread_new();
		cg_thread_setaction(ctrlPoint->expThread, cg_upnp_controlpoint_expirationhandler);
		cg_thread_setuserdata(ctrlPoint->expThread, ctrlPoint);
		ctrlPoint->expMutex = cg_mutex_new();
		ctrlPoint->expCond = cg_cond_new();
		
		ctrlPoint->ifCache = cg_net_interfacelist_new();
		
		cg_upnp_controlpoint_setssdplistener(ctrlPoint, NULL);
		cg_upnp_controlpoint_setssdpresponselistener(ctrlPoint, NULL);
		cg_upnp_controlpoint_sethttplistener(ctrlPoint, NULL);
		cg_upnp_controlpoint_setdevicelistener(ctrlPoint, NULL);
		
		cg_upnp_controlpoint_setssdpresponseport(ctrlPoint, CG_UPNP_CONTROLPOINT_SSDP_RESPONSE_DEFAULT_PORT);
		cg_upnp_controlpoint_setssdpsearchmx(ctrlPoint, CG_UPNP_CONTROLPOINT_SSDP_DEFAULT_SEARCH_MX);
		cg_upnp_controlpoint_seteventport(ctrlPoint, CG_UPNP_CONTROLPOINT_HTTP_EVENT_DEFAULT_PORT);
		cg_upnp_controlpoint_seteventsuburi(ctrlPoint, CG_UPNP_CONTROLPOINT_HTTP_EVENTSUB_URI);

		cg_upnp_controlpoint_setuserdata(ctrlPoint, NULL);
	}

	cg_log_debug_l4("Leaving...\n");

	return ctrlPoint;
}

/**
 * Destroy the given control point
 *
 * @param ctrlPoint The control point struct to destroy
 */
void cg_upnp_controlpoint_delete(CgUpnpControlPoint *ctrlPoint)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_controlpoint_stop(ctrlPoint);
	
	/* Delete cached interfaces */
	cg_net_interfacelist_delete(ctrlPoint->ifCache);
	
	/* Delete expiration handlers */
	cg_thread_delete(ctrlPoint->expThread);
	cg_mutex_delete(ctrlPoint->expMutex);
	cg_cond_delete(ctrlPoint->expCond);
	
	/* Delete others */
	cg_mutex_delete(ctrlPoint->mutex);
	cg_xml_nodelist_delete(ctrlPoint->deviceRootNodeList);
	cg_upnp_devicelist_delete(ctrlPoint->deviceList);
	cg_upnp_ssdp_serverlist_delete(ctrlPoint->ssdpServerList);
	cg_upnp_ssdpresponse_serverlist_delete(ctrlPoint->ssdpResServerList);
	cg_http_serverlist_delete(ctrlPoint->httpServerList);
	cg_string_delete(ctrlPoint->httpEventURI);
	cg_upnp_eventlistenerlist_delete(ctrlPoint->eventListeners);	

#ifdef CG_HTTP_USE_PERSISTENT_CONNECTIONS	
	cg_http_persistentconnection_clear();
#endif
	free(ctrlPoint);

	cg_log_debug_l4("Leaving...\n");
}

/**
 * Activate the control point. Starts listening for SSDP messages etc.
 * You must call this function before you can actually use a control point.
 *
 * @param ctrlPoint The control point to start
 *
 * @return TRUE if successful; otherwise FALSE
 *
 */
BOOL cg_upnp_controlpoint_start(CgUpnpControlPoint *ctrlPoint)
{
	CgHttpServerList *httpServerList;
	CG_HTTP_LISTENER httpListener;
	int httpEventPort;
	CgUpnpSSDPServerList *ssdpServerList;
	CgUpnpSSDPResponseServerList *ssdpResServerList;
	int ssdpResPort;
	int ssdpMaxResPort;
	
	cg_log_debug_l4("Entering...\n");

	cg_upnp_controlpoint_stop(ctrlPoint);

	/* Expiration handling */
	cg_thread_start(ctrlPoint->expThread);
	
	/**** Cache current interfaces ****/
	cg_net_gethostinterfaces(ctrlPoint->ifCache);
	
	/**** HTTP Server ****/
	httpEventPort = cg_upnp_controlpoint_geteventport(ctrlPoint);
	httpServerList = cg_upnp_controlpoint_gethttpserverlist(ctrlPoint);
	/* Opening HTTP server may fail, so try many ports */
	while(cg_http_serverlist_open(httpServerList, httpEventPort) == FALSE) {
		cg_upnp_controlpoint_seteventport(ctrlPoint, httpEventPort + 1);
		httpEventPort = cg_upnp_controlpoint_geteventport(ctrlPoint);
	}
	cg_http_serverlist_setuserdata(httpServerList, ctrlPoint);
	httpListener = cg_upnp_controlpoint_gethttplistener(ctrlPoint);
	if (httpListener == NULL)
		httpListener = cg_upnp_controlpoint_httprequestreceived;
	cg_http_serverlist_setlistener(httpServerList, httpListener);
	cg_http_serverlist_start(httpServerList);

	/**** SSDP Server ****/
	ssdpServerList = cg_upnp_controlpoint_getssdpserverlist(ctrlPoint);
	if (cg_upnp_ssdp_serverlist_open(ssdpServerList) == FALSE)
		return FALSE;
	cg_upnp_ssdp_serverlist_setlistener(ssdpServerList, cg_upnp_controlpoint_ssdplistner);
	cg_upnp_ssdp_serverlist_setuserdata(ssdpServerList, ctrlPoint);
	if (cg_upnp_ssdp_serverlist_start(ssdpServerList) == FALSE)
		return FALSE;

	/**** SSDP Response Server ****/
	ssdpResPort = cg_upnp_controlpoint_getssdpresponseport(ctrlPoint);
	/* below is the max SSDP Response port number to assign (rosfran.borges) */
	ssdpMaxResPort = ssdpResPort + CG_UPNP_CONTROLPOINT_SSDP_RESPONSE_PORT_MAX_TRIES_INDEX;
	ssdpResServerList = cg_upnp_controlpoint_getssdpresponseserverlist(ctrlPoint);
	/* Opening SSDP response server may fail, so try many ports */
	while(cg_upnp_ssdpresponse_serverlist_open(ssdpResServerList, ssdpResPort) == FALSE &&
		(ssdpResPort < ssdpMaxResPort) ) {
		cg_upnp_controlpoint_setssdpresponseport(ctrlPoint, ssdpResPort + 1);
		ssdpResPort = cg_upnp_controlpoint_getssdpresponseport(ctrlPoint);
	}

	cg_upnp_ssdpresponse_serverlist_setlistener(ssdpResServerList, cg_upnp_controlpoint_ssdpresponselistner);
	cg_upnp_ssdpresponse_serverlist_setuserdata(ssdpResServerList, ctrlPoint);
	if (cg_upnp_ssdpresponse_serverlist_start(ssdpResServerList) == FALSE)
		return FALSE;

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/**
 * Stop the control point. Stops sending/receiveing/responding to any messages.
 *
 * @param ctrlPoint The control point to stop
 *
 * @return TRUE if successful; otherwise FALSE
 *
 */
BOOL cg_upnp_controlpoint_stop(CgUpnpControlPoint *ctrlPoint)
{
	CgUpnpDevice *dev = NULL;
	CgUpnpSSDPServerList *ssdpServerList;
	CgUpnpSSDPResponseServerList *ssdpResServerList;
	CgHttpServerList *httpServerList;
	char *udn = NULL;
	CG_UPNP_DEVICE_LISTENER listener = cg_upnp_controlpoint_getdevicelistener(ctrlPoint);
	
	cg_log_debug_l4("Entering...\n");

	/* Stop expiration handling */
	cg_thread_stop_with_cond(ctrlPoint->expThread, ctrlPoint->expCond);
	cg_log_debug_s("Expiration thread stopped.\n");
	
	/**** SSDP Server ****/
	ssdpServerList = cg_upnp_controlpoint_getssdpserverlist(ctrlPoint);
	cg_log_debug_s("Stopping ssdp servers.\n");
	cg_upnp_ssdp_serverlist_stop(ssdpServerList);
	cg_log_debug_s("Done\n");
	cg_upnp_ssdp_serverlist_close(ssdpServerList);
	cg_upnp_ssdp_serverlist_clear(ssdpServerList);
	
	/**** SSDP Response Server ****/
	ssdpResServerList = cg_upnp_controlpoint_getssdpresponseserverlist(ctrlPoint);
	cg_log_debug_s("Stopping ssdp response servers.\n");
	cg_upnp_ssdpresponse_serverlist_stop(ssdpResServerList);
	cg_log_debug_s("Done\n");
	cg_upnp_ssdpresponse_serverlist_close(ssdpResServerList);
	cg_upnp_ssdpresponse_serverlist_clear(ssdpResServerList);
	
	/**** HTTP Server ****/
	httpServerList = cg_upnp_controlpoint_gethttpserverlist(ctrlPoint);
	cg_log_debug_s("Stopping http servers.\n");
	cg_http_serverlist_stop(httpServerList);
	cg_log_debug_s("Done\n");
	cg_http_serverlist_close(httpServerList);
	cg_http_serverlist_clear(httpServerList);

	cg_upnp_controlpoint_lock(ctrlPoint);

	cg_log_debug_s("Got controlpoint lock.\n");
	
	/* Unsubscribe from all services */

	for (dev = cg_upnp_controlpoint_getdevices(ctrlPoint); 
	     dev != NULL;
	     dev = cg_upnp_device_next(dev))
	{
		udn = cg_upnp_device_getudn(dev);
		
		/* Call device listener for each device */
		if (udn != NULL && listener != NULL)
		{
			cg_upnp_controlpoint_unlock(ctrlPoint);
			listener(ctrlPoint, udn, CgUpnpDeviceStatusRemoved);
			cg_upnp_controlpoint_lock(ctrlPoint);
		}
	}
	/* Empty device cache */
	cg_upnp_devicelist_clear(ctrlPoint->deviceList);
	cg_log_debug_s("Device list cleared.\n");

	cg_upnp_controlpoint_unlock(ctrlPoint);
		
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/**
 * Check if  the control point is activated.
 *
 * @param ctrlPoint The control point to stop
 *
 * @return TRUE if running; otherwise FALSE
 *
 */
BOOL cg_upnp_controlpoint_isrunning(CgUpnpControlPoint *ctrlPoint);
/**
 * Check if  the control point is activated.
 *
 * @param ctrlPoint The control point to stop
 *
 * @return TRUE if running; otherwise FALSE
 *
 */
BOOL cg_upnp_controlpoint_isrunning(CgUpnpControlPoint *ctrlPoint)
{
	CgHttpServerList *httpServerList;
	
	httpServerList = cg_upnp_controlpoint_gethttpserverlist(ctrlPoint);
	if (cg_http_serverlist_size(httpServerList) == 0)
		return FALSE;
	
	return TRUE;
}

/****************************************************************************
 * Control Point locking
 ****************************************************************************/
/**
 * Lock the control point's mutex. 
 * The control point should be ALWAYS locked, when a CgUpnpDevice*,
 * CgUpnpService*, CgUpnpAction* or other pointer has been taken into use from
 * the stack. This effectively prevents devices/services from being updated/
 * removed or added while the control point is locked. You should release the
 * lock as soon as possible with @ref cg_upnp_controlpoint_unlock
 *
 * @note Do NOT save any CgUpnp* pointers to user-space variables. Use them
 * only as local variables (inside one function) after gaining a mutex lock.
 * Release the lock as soon as you are done accessing the pointer, and then
 * discard the pointer immediately.
 *
 * @param ctrlPoint The control point in question
 */
#ifndef WITH_THREAD_LOCK_TRACE
BOOL cg_upnp_controlpoint_lock(CgUpnpControlPoint *ctrlPoint)
{
	cg_log_debug_l4("Entering...\n");

	return cg_mutex_lock(ctrlPoint->mutex);

	cg_log_debug_l4("Leaving...\n");
}

/**
 * Release a previously locked control point mutex.
 * See @ref cg_upnp_controlpoint_lock for a more detailed description on
 * the control point locking mechanism.
 *
 * @param ctrlPoint The control point in question
 */
BOOL cg_upnp_controlpoint_unlock(CgUpnpControlPoint *ctrlPoint)
{
	cg_log_debug_l4("Entering...\n");

	return cg_mutex_unlock(ctrlPoint->mutex);

	cg_log_debug_l4("Leaving...\n");
}
#endif
/****************************************************************************
 * Device searching
 ****************************************************************************/

/**
 * Find a device from the control point by the exact type of the device.
 * This function searches for devices, whose *complete type string*
 * matches the given string, including version number. For example: 
 * "urn:schemas-upnp-org:device:FooDevice:1". If you need to disregard
 * the version, use \ref cg_upnp_controlpoint_getdevicebytype
 *
 * \param ctrlPoint Controlpoint in question
 * \param exacttype Type of the device
 *
 */
CgUpnpDevice *cg_upnp_controlpoint_getdevicebyexacttype(CgUpnpControlPoint *ctrlPoint,
						   char *exacttype)
{
	CgUpnpDevice *dev = NULL;
	CgUpnpDevice *childDev = NULL;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(exacttype) <= 0 || ctrlPoint == NULL)
	{
		return NULL;
	}
	
	for (dev = cg_upnp_controlpoint_getdevices(ctrlPoint); 
	     dev != NULL;
	     dev = cg_upnp_device_next(dev))
	{
		if (cg_strcmp(cg_upnp_device_getdevicetype(dev),
			      exacttype) == 0)
		{
			return dev;
		}

		childDev = cg_upnp_device_getdevicebyexacttype(dev, exacttype);
		if (childDev != NULL)
		{
			return childDev;
		}
	}

	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/**
 * Find a device from the controlpoint by the type of the device.
 * This function searches for devices, whose *type part* (i.e. not including
 * the version) of the device type string matches the given string.
 * For example: "urn:schemas-upnp-org:device:FooDevice". If you need
 * to know the version of a device, use \ref cg_upnp_devicetype_getversion
 *
 * \param ctrlPoint Controlpoint in question
 * \param type Type of the device
 *
 */
CgUpnpDevice *cg_upnp_controlpoint_getdevicebytype(CgUpnpControlPoint *ctrlPoint,
						   char *type)
{
	CgUpnpDevice *dev = NULL;
	CgUpnpDevice *childDev = NULL;
	char* typeString = NULL;
	char* part = NULL;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(type) <= 0 || ctrlPoint == NULL)
	{
		return NULL;
	}

	for (dev = cg_upnp_controlpoint_getdevices(ctrlPoint);
	     dev != NULL;
	     dev = cg_upnp_device_next(dev))
	{
		typeString = cg_upnp_device_getdevicetype(dev);
		if (typeString != NULL)
		{
			part = cg_upnp_devicetype_getschematype(typeString);
			if (cg_strcmp(part, type) == 0)
			{
				free(part);
				
				return dev;
			}
			else
			{
				free(part);
			}
		}
				
		childDev = cg_upnp_device_getdevicebytype(dev, type);
		if (childDev != NULL)
		{
			return childDev;
		}
	}
	
	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/**
 * Find a device from the controlpoint by the UDN of the device.
 *
 * \param ctrlPoint Controlpoint in question
 * \param udn Type of the device
 *
 */
CgUpnpDevice *cg_upnp_controlpoint_getdevicebyudn(CgUpnpControlPoint *ctrlPoint,
						  char *udn)
{
	CgUpnpDevice *dev = NULL;
	CgUpnpDevice *childDev = NULL;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(udn) <= 0 || ctrlPoint == NULL)
	{
		return NULL;
	}
	
	for (dev = cg_upnp_controlpoint_getdevices(ctrlPoint); 
	     dev != NULL;
	     dev = cg_upnp_device_next(dev))
	{
		if (cg_strcmp(cg_upnp_device_getudn(dev), udn) == 0)
		{
			return dev;
		}
		childDev = cg_upnp_device_getdevicebyudn(dev, udn);
		if (childDev != NULL)
		{
			return childDev;
		}
	}

	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************************************************
 * Service SCPD
 ****************************************************************************/

/**
 * Parse the service description from the service's SCPD URL. Do not call
 * this from user applications.
 * 
 * @param service The service in question
 * @return TRUE if successful; otherwise FALSE
 */
BOOL cg_upnp_controlpoint_parsescservicescpd(CgUpnpService *service)
{
	CgNetURL *scpdURL;
	BOOL scpdParseSuccess;
	
	cg_log_debug_l4("Entering...\n");

	scpdURL = cg_upnp_service_getscpdurl(service); 

	if ( NULL == scpdURL )		
		return FALSE;
	
	cg_log_debug_s("SCPD URL: %s\n", cg_net_url_getrequest(scpdURL));
	scpdParseSuccess = cg_upnp_service_parsedescriptionurl(service, scpdURL);
	
	cg_net_url_delete(scpdURL);
	if (scpdParseSuccess == TRUE)
		return TRUE;

#if defined(CG_UPNP_USE_STDDCP)
	if (cg_upnp_service_hasstddcp(service)) {
		char *stdDCP = cg_upnp_service_getstddcp(service);
		scpdParseSuccess = cg_upnp_service_parsedescription(service, stdDCP, cg_strlen(stdDCP));
	}
#endif

	cg_log_debug_l4("Leaving...\n");

	return scpdParseSuccess;
}

/****************************************
* cg_upnp_controlpoint_parseservicesfordevice
****************************************/

BOOL cg_upnp_controlpoint_parseservicesfordevice(CgUpnpDevice *dev, CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	cg_log_debug_l4("Entering...\n");

	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_upnp_controlpoint_parsescservicescpd(service) == FALSE) {
			return FALSE;
		}
	}
	
	/* Now only root SCPDs for root services are parsed, but also child 
	   devices' services have to be parsed, so parse them */
	for (childDev=cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		if (cg_upnp_controlpoint_parseservicesfordevice(childDev, ssdpPkt) == FALSE)
		{
			return FALSE;
		}
	}
	
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

static CgUpnpDevice *cg_upnp_controlpoint_createdevicefromssdkpacket(CgUpnpSSDPPacket *ssdpPkt)
{
	char *location;
	CgNetURL *url;
	CgUpnpDevice *dev;
	BOOL parseSuccess;
	
	cg_log_debug_l4("Entering...\n");

	location = cg_upnp_ssdp_packet_getlocation(ssdpPkt);
	if (cg_strlen(location) <= 0)
		return NULL;
			
	dev = cg_upnp_device_new();
	
	url = cg_net_url_new();
	cg_net_url_set(url, location);
	parseSuccess =  cg_upnp_device_parsedescriptionurl(dev, url);
	cg_net_url_delete(url);
	
	if (parseSuccess == FALSE) {
		cg_upnp_device_delete(dev);
		return NULL;
	}

	cg_upnp_device_setssdppacket(dev, ssdpPkt);

#ifndef CG_OPTIMIZED_CP_MODE
	if (cg_upnp_controlpoint_parseservicesfordevice(dev, ssdpPkt) == FALSE)
	{
		cg_upnp_device_delete(dev);
		return NULL;
	}
#endif
	
	cg_log_debug_l4("Leaving...\n");

	return dev;
}

/****************************************
* cg_upnp_controlpoint_adddevicebyssdppacket
****************************************/

void cg_upnp_controlpoint_adddevicebyssdppacket(CgUpnpControlPoint *ctrlPoint, CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpDevice *dev = NULL;
	char *usn = NULL;
	char udn[CG_UPNP_UDN_LEN_MAX];
	CG_UPNP_DEVICE_LISTENER listener = NULL;
	CgUpnpDeviceStatus status = 0;

	cg_log_debug_l4("Entering...\n");

	listener = cg_upnp_controlpoint_getdevicelistener(ctrlPoint);
	
	usn = cg_upnp_ssdp_packet_getusn(ssdpPkt);
	cg_upnp_usn_getudn(usn, udn, sizeof(udn));
	
	cg_upnp_controlpoint_lock(ctrlPoint);
	
	dev = cg_upnp_controlpoint_getdevicebyudn(ctrlPoint, udn);
	
	if (dev != NULL)
	{
		/* Device was found from local cache */
		if (cg_upnp_device_updatefromssdppacket(dev, ssdpPkt) == TRUE)
		{
			cg_mutex_lock(ctrlPoint->expMutex);
			cg_cond_signal(ctrlPoint->expCond);
			cg_mutex_unlock(ctrlPoint->expMutex);
			
			/* Device was successfully updated */
			status = CgUpnpDeviceStatusUpdated;
		}
		else
		{	/* Problems occurred in device update */
			status = CgUpnpDeviceStatusInvalid;
		}
	}
	else
	{
		/* This is a new device */
		dev = cg_upnp_controlpoint_createdevicefromssdkpacket(ssdpPkt);
		if (dev == NULL)
		{
			/* Problems occurred in device creation */
			status = CgUpnpDeviceStatusInvalid;
		}
		else
		{
			cg_upnp_controlpoint_adddevice(ctrlPoint, dev);

			/* Device added, wake up expirationhandler thread */
			cg_mutex_lock(ctrlPoint->expMutex);
			cg_cond_signal(ctrlPoint->expCond);
			cg_mutex_unlock(ctrlPoint->expMutex);
			
			status = CgUpnpDeviceStatusAdded;
		}
	}
	
	cg_upnp_controlpoint_unlock(ctrlPoint);
	
	if (listener != NULL)
	{
		listener(ctrlPoint, udn, status);
	}

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_controlpoint_removedevicebyssdppacket
****************************************/

void cg_upnp_controlpoint_removedevicebyssdppacket(CgUpnpControlPoint *ctrlPoint, CgUpnpSSDPPacket *ssdpPkt)
{
	char *usn;
	char udn[CG_UPNP_UDN_LEN_MAX];
	CgUpnpDevice *dev;
	CG_UPNP_DEVICE_LISTENER listener = cg_upnp_controlpoint_getdevicelistener(ctrlPoint);
	
	cg_log_debug_l4("Entering...\n");

	usn = cg_upnp_ssdp_packet_getusn(ssdpPkt);
	cg_upnp_usn_getudn(usn, udn, sizeof(udn));
	
	cg_upnp_controlpoint_lock(ctrlPoint);
	
	dev = cg_upnp_controlpoint_getdevicebyudn(ctrlPoint, udn);

	if (dev == NULL) {
		cg_upnp_controlpoint_unlock(ctrlPoint);
		return;
	}
	
	if (listener != NULL)
	{
		cg_upnp_controlpoint_unlock(ctrlPoint);
		listener(ctrlPoint, udn, CgUpnpDeviceStatusRemoved);
		cg_upnp_controlpoint_lock(ctrlPoint);
	}
	
	cg_upnp_device_delete(dev);
	
	cg_upnp_controlpoint_unlock(ctrlPoint);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************************************************
 * M-SEARCH
 ****************************************************************************/

/**
 * Do an M-SEARCH to look for devices in the network.
 *
 * @param ctrlPoint The control point in question
 * @param target The Search Target parameter (ex. "ssdp:all")
 */
BOOL cg_upnp_controlpoint_search(CgUpnpControlPoint *ctrlPoint, char *target)
{
	CgUpnpSSDPRequest *ssdpReq;
	CgUpnpSSDPResponseServerList *ssdpResServerList;
	int i = 0;
	BOOL retval = FALSE;
	
	cg_log_debug_l4("Entering...\n");

	ssdpReq = cg_upnp_ssdprequest_new();
	cg_upnp_ssdprequest_setmethod(ssdpReq, CG_HTTP_MSEARCH);
	cg_upnp_ssdprequest_setst(ssdpReq, target);
	cg_upnp_ssdprequest_setmx(ssdpReq, cg_upnp_controlpoint_getssdpsearchmx(ctrlPoint));
	cg_upnp_ssdprequest_setman(ssdpReq, CG_UPNP_MAN_DISCOVER);
	
	cg_log_debug("Announcing %d times.\n", cg_upnp_ssdp_getannouncecount());
	cg_upnp_ssdprequest_print(ssdpReq);
	
	for (i = 0; i < cg_upnp_ssdp_getannouncecount(); i++)
	{
		ssdpResServerList = cg_upnp_controlpoint_getssdpresponseserverlist(ctrlPoint);
		retval = ( cg_upnp_ssdpresponse_serverlist_post(ssdpResServerList, ssdpReq ) || retval );
		cg_wait(CG_UPNP_CONTROLPOINT_SSDP_MIN_DELAY);
	}
	
	cg_upnp_ssdprequest_delete(ssdpReq);

	cg_log_debug_l4("Leaving...\n");

	return retval;
}

/****************************************
* cg_upnp_controlpoint_ipchanged
****************************************/

BOOL cg_upnp_controlpoint_ipchanged(CgUpnpControlPoint *ctrlPoint)
{
	CgNetworkInterfaceList *current, *added, *removed;
	CgNetworkInterface *netIf;
	CgUpnpDevice *dev, *tmp;
	CgUpnpSSDPPacket *ssdpPkt;
	char *address;
	
	cg_log_debug_l4("Entering...\n");

	current = cg_net_interfacelist_new();
	added = cg_net_interfacelist_new();
	removed = cg_net_interfacelist_new();

	if (current == NULL || added == NULL || removed == NULL)
	{
		if (current != NULL) cg_net_interfacelist_delete(current);
		if (added != NULL) cg_net_interfacelist_delete(added);
		if (removed != NULL) cg_net_interfacelist_delete(removed);
		return FALSE;
	}
	
	/* Get Interface changes */
	cg_net_gethostinterfaces(current);
	cg_net_interfacelist_getchanges(ctrlPoint->ifCache, current, 
					added, removed);
	
	/* Remove all devices registered through old interface */
	for (netIf = cg_net_interfacelist_gets(removed);
	     netIf != NULL; netIf = cg_net_interface_next(netIf))
	{
		cg_upnp_controlpoint_lock(ctrlPoint);
		tmp = cg_upnp_controlpoint_getdevices(ctrlPoint);
		while (tmp != NULL)
		{
			dev = tmp; tmp = cg_upnp_device_next(dev);
			ssdpPkt = cg_upnp_device_getssdppacket(dev);
			address = cg_upnp_ssdp_packet_getlocaladdress(ssdpPkt);
			
			if (address != NULL && 
			    cg_strcmp(address, cg_net_interface_getaddress(netIf)) == 0)
			{
				/* This device has been received from the 
				   removed interface, so it does not exist */
				cg_upnp_controlpoint_unlock(ctrlPoint);
				cg_upnp_controlpoint_removedevicebyssdppacket(ctrlPoint, 
									      ssdpPkt);
				cg_upnp_controlpoint_lock(ctrlPoint);
				address = NULL; dev = NULL; ssdpPkt = NULL;
			}
		}
		cg_upnp_controlpoint_unlock(ctrlPoint);
	}

	/* Launch new M-SEARCH */
	cg_upnp_controlpoint_search(ctrlPoint, CG_UPNP_ST_ROOT_DEVICE);
	
	/**** Cache current interfaces ****/
	cg_net_gethostinterfaces(ctrlPoint->ifCache);
	
	cg_net_interfacelist_delete(current);
	cg_net_interfacelist_delete(added);
	cg_net_interfacelist_delete(removed);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_upnp_controlpoint_ssdplistner
****************************************/

static void cg_upnp_controlpoint_ssdplistner(CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpControlPoint *ctrlPoint;
	CG_UPNP_SSDP_LISTNER listener;

	cg_log_debug_l4("Entering...\n");

	ctrlPoint = (CgUpnpControlPoint *)cg_upnp_ssdp_packet_getuserdata(ssdpPkt);
	if (ctrlPoint == NULL)
		return;

	/* We filter out all but rootdevice, since it must be advertized by all
	 * devices. This way we avoid lots of device updates during advertizement
	 * cycle. */
	if (cg_upnp_ssdp_packet_isrootdevice(ssdpPkt) == TRUE) {
		if (cg_upnp_ssdp_packet_isalive(ssdpPkt) == TRUE)
			cg_upnp_controlpoint_adddevicebyssdppacket(ctrlPoint, ssdpPkt);
		else if (cg_upnp_ssdp_packet_isbyebye(ssdpPkt) == TRUE)
			cg_upnp_controlpoint_removedevicebyssdppacket(ctrlPoint, ssdpPkt);
	}
				
	listener = cg_upnp_controlpoint_getssdplistener(ctrlPoint);
	if (listener != NULL)
		listener(ssdpPkt);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_controlpoint_ssdpresponselistner
****************************************/

static void cg_upnp_controlpoint_ssdpresponselistner(CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpControlPoint *ctrlPoint;
	CG_UPNP_SSDP_RESPONSE_LISTNER listener;

	cg_log_debug_l4("Entering...\n");

	ctrlPoint = (CgUpnpControlPoint *)cg_upnp_ssdp_packet_getuserdata(ssdpPkt);
	if (ctrlPoint == NULL)
		return;

	cg_upnp_controlpoint_adddevicebyssdppacket(ctrlPoint, ssdpPkt);
				
	listener = cg_upnp_controlpoint_getssdpresponselistener(ctrlPoint);
	if (listener != NULL)
		listener(ssdpPkt);	

	cg_log_debug_l4("Leaving...\n");
}

#endif
