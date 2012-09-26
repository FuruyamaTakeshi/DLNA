/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cmediaserver_devicer.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/upnp/std/av/cmediaserver.h>

/****************************************
* Device Description
****************************************/

static char *CG_UPNPAV_DMS_DEVICE_DESCRIPTION =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\" xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">\n"
"   <specVersion>\n"
"      <major>1</major>\n"
"      <minor>0</minor>\n"
"   </specVersion>\n"
"   <device>\n"
"      <deviceType>urn:schemas-upnp-org:device:MediaServer:1</deviceType>\n"
"      <friendlyName>Cyber Garage Media Server (MediaServer) : 1</friendlyName>\n"
"      <manufacturer>Cyber Garage</manufacturer>\n"
"      <manufacturerURL>http://www.cybergarage.org</manufacturerURL>\n"
"      <modelDescription>CyberGarage</modelDescription>\n"
"      <modelName>Windows Media Connect</modelName>\n"
"      <modelNumber>1.0</modelNumber>\n"
"      <UDN>uuid:BA2E90A0-3669-401a-B249-F85196ADFC44</UDN>\n"
"      <modelURL>http://www.cybergarage.org</modelURL>\n"
"      <dlna:X_DLNADOC>DMS-1.00</dlna:X_DLNADOC>\n"
"      <serviceList>\n"
"         <service>\n"
"            <serviceType>urn:schemas-upnp-org:service:ContentDirectory:1</serviceType>\n"
"            <serviceId>urn:upnp-org:serviceId:urn:schemas-upnp-org:service:ContentDirectory</serviceId>\n"
"            <SCPDURL>/service/ContentDirectory1.xml</SCPDURL>\n"
"            <controlURL>/service/ContentDirectory_control</controlURL>\n"
"            <eventSubURL>/service/ContentDirectory_event</eventSubURL>\n"
"         </service>\n"
"         <service>\n"
"            <serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>\n"
"            <serviceId>urn:upnp-org:serviceId:urn:schemas-upnp-org:service:ConnectionManager</serviceId>\n"
"            <SCPDURL>/service/ConnectionManager1.xml</SCPDURL>\n"
"            <controlURL>/service/ConnectionManager_control</controlURL>\n"
"            <eventSubURL>/service/ConnectionManager_event</eventSubURL>\n"
"         </service>\n"
"         <service>\n"
"            <serviceType>urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1</serviceType>\n"
"            <serviceId>urn:microsoft.com:serviceId:X_MS_MediaReceiverRegistrar</serviceId>\n"
"            <SCPDURL>/service/MediaReceiverRegistrar1.xml</SCPDURL>\n"
"            <controlURL>/service/MediaReceiverRegistrar_control</controlURL>\n"
"            <eventSubURL>/service/MediaReceiverRegistrar_event</eventSubURL>\n"
"         </service>\n"
"      </serviceList>\n"
"   </device>\n"
"</root>\n";

/****************************************
* Functions
****************************************/

BOOL cg_upnpav_dms_condir_actionreceived(CgUpnpAction *action);
BOOL cg_upnpav_dms_conmgr_actionreceived(CgUpnpAction *action);
BOOL cg_upnpav_dms_medrec_actionreceived(CgUpnpAction *action);
BOOL cg_upnpav_dms_condir_queryreceived(CgUpnpStateVariable *var);

/****************************************
* cg_upnpav_dms_actionreceived
****************************************/

BOOL cg_upnpav_dms_actionreceived(CgUpnpAction *action)
{
	CgUpnpDevice *dev;
	CgUpnpService *service;
	CgUpnpAvServer *dms;
	CG_UPNP_ACTION_LISTNER userActionListener;

	service = cg_upnp_action_getservice(action);
	if (!service)
		return FALSE;

	dev = (CgUpnpDevice *)cg_upnp_service_getdevice(service);
	if (!dev)
		return FALSE;

	dms = (CgUpnpAvServer *)cg_upnp_device_getuserdata(dev);
	if (!dms)
		return FALSE;

	userActionListener = cg_upnpav_dms_getactionlistener(dms);
	if (userActionListener) {
		if (userActionListener(action))
			return TRUE;
	}

	if (cg_streq(cg_upnp_service_getservicetype(service), CG_UPNPAV_DMS_CONTENTDIRECTORY_SERVICE_TYPE))
		return cg_upnpav_dms_condir_actionreceived(action);

	if (cg_streq(cg_upnp_service_getservicetype(service), CG_UPNPAV_DMS_MEDIARECEIVER_SERVICE_TYPE))
		return cg_upnpav_dms_medrec_actionreceived(action);

	if (cg_streq(cg_upnp_service_getservicetype(service), CG_UPNPAV_DMS_CONNECTIONMANAGER_SERVICE_TYPE))
		return cg_upnpav_dms_conmgr_actionreceived(action);

	return FALSE;
}

/****************************************
* cg_upnpav_dms_queryreceived
****************************************/

BOOL cg_upnpav_dms_queryreceived(CgUpnpStateVariable *statVar)
{
	CgUpnpDevice *dev;
	CgUpnpService *service;
	CgUpnpAvServer *dms;
	CG_UPNPAV_STATEVARIABLE_LISTNER userQueryListener;

	service = cg_upnp_statevariable_getservice(statVar);
	if (!service)
		return FALSE;

	dev = (CgUpnpDevice *)cg_upnp_service_getdevice(service);
	if (!dev)
		return FALSE;

	dms = (CgUpnpAvServer *)cg_upnp_device_getuserdata(dev);
	if (!dms)
		return FALSE;

	userQueryListener = cg_upnpav_dms_getquerylistener(dms);
	if (userQueryListener) {
		if (userQueryListener(statVar))
			return TRUE;
	}

	if (cg_streq(cg_upnp_service_getservicetype(service), CG_UPNPAV_DMS_CONTENTDIRECTORY_SERVICE_TYPE))
		return cg_upnpav_dms_condir_queryreceived(statVar);

	return FALSE;
}

/****************************************
* cg_upnpav_dms_device_httprequestrecieved
****************************************/

void cg_upnpav_dms_device_httprequestrecieved(CgHttpRequest *httpReq)
{
	CgUpnpAvServer *dms;
	CgUpnpDevice *dev;
	CG_UPNPAV_HTTP_LISTENER userHttpListener;

	dev = (CgUpnpDevice *)cg_http_request_getuserdata(httpReq);
	if (!dev) {
		cg_upnp_device_httprequestrecieved(httpReq);
		return;
	}

	dms = (CgUpnpAvServer *)cg_upnp_device_getuserdata(dev);
	if (!dms) {
		cg_upnp_device_httprequestrecieved(httpReq);
		return;
	}

	userHttpListener = cg_upnpav_dms_gethttplistener(dms);
	if (userHttpListener) {
		if (userHttpListener(httpReq))
			return;
	}

	cg_upnp_device_httprequestrecieved(httpReq);
}

/****************************************
* cg_upnpav_dms_new
****************************************/

CgUpnpAvServer *cg_upnpav_dms_new()
{
	CgUpnpAvServer *dms;

	dms = (CgUpnpAvServer *)malloc(sizeof(CgUpnpAvServer));

	dms->dev = cg_upnp_device_new();
	if (!dms->dev) {
		free(dms);
		return NULL;
	}

	if (cg_upnp_device_parsedescription(dms->dev, CG_UPNPAV_DMS_DEVICE_DESCRIPTION, cg_strlen(CG_UPNPAV_DMS_DEVICE_DESCRIPTION)) == FALSE) {
		cg_upnp_device_delete(dms->dev);
		free(dms);
		return NULL;
	}

	if (cg_upnpav_dms_conmgr_init(dms) == FALSE) {
		cg_upnp_device_delete(dms->dev);
		free(dms);
		return NULL;
	}

	if (cg_upnpav_dms_condir_init(dms) == FALSE) {
		cg_upnp_device_delete(dms->dev);
		free(dms);
		return NULL;
	}

	if (cg_upnpav_dms_medrec_init(dms) == FALSE) {
		cg_upnp_device_delete(dms->dev);
		free(dms);
		return NULL;
	}

	dms->rootContent = cg_upnpav_content_new();
	cg_upnpav_content_settype(dms->rootContent, CG_UPNPAV_CONTENT_CONTAINER);
    cg_upnpav_content_settitle(dms->rootContent, CG_UPNPAV_ROOT_CONTENT_TITLE);
	cg_upnpav_content_setid(dms->rootContent, CG_UPNPAV_ROOT_CONTENT_ID);
	cg_upnpav_content_setparentid(dms->rootContent, CG_UPNPAV_ROOT_CONTENT_PARENTID);

	if (!dms->rootContent) {
		cg_upnpav_dms_delete(dms);
		return NULL;
	}

	dms->mutex = cg_mutex_new();
	if (!dms->mutex) {
		cg_upnpav_dms_delete(dms);
		return NULL;
	}

	dms->networkInterfaceList = cg_net_interfacelist_new();
	if (!dms->networkInterfaceList) {
		cg_upnpav_dms_delete(dms);
		return NULL;
	}

	cg_upnp_device_setactionlistener(dms->dev, cg_upnpav_dms_actionreceived);
	cg_upnp_device_setquerylistener(dms->dev, cg_upnpav_dms_queryreceived);
	cg_upnp_device_sethttplistener(dms->dev, cg_upnpav_dms_device_httprequestrecieved);

	cg_upnpav_dms_setactionlistener(dms, NULL);
	cg_upnpav_dms_setquerylistener(dms, NULL);
	cg_upnpav_dms_sethttplistener(dms, NULL);

	cg_upnp_device_setuserdata(dms->dev, dms);
	cg_upnp_device_updateudn(dms->dev);

	dms->protocolInfoList = cg_upnpav_protocolinfolist_new();

	return dms;
}

/****************************************
* cg_upnpav_dms_delete
****************************************/

void cg_upnpav_dms_delete(CgUpnpAvServer *dms)
{
	if (dms == NULL)
		return;

	if (dms->rootContent)
		cg_upnpav_content_delete(dms->rootContent);

	if (dms->mutex)
		cg_mutex_delete(dms->mutex);

	if (dms->protocolInfoList)
		cg_upnpav_protocolinfolist_delete(dms->protocolInfoList);

	if (dms->networkInterfaceList)
		cg_net_interfacelist_delete(dms->networkInterfaceList);

	cg_upnp_device_delete(dms->dev);

	free(dms);
}

/****************************************
 * cg_upnpav_dms_updatenetworkinterfaces
 ****************************************/

BOOL cg_upnpav_dms_updatenetworkinterfaces(CgUpnpAvServer *dms)
{
	cg_upnpav_dms_lock(dms);
	cg_net_interfacelist_clear(dms->networkInterfaceList);
	cg_net_gethostinterfaces(dms->networkInterfaceList);
	cg_upnpav_dms_unlock(dms);
	return (0 < cg_net_interfacelist_size(dms->networkInterfaceList)) ? TRUE : FALSE;
}

/****************************************
 * cg_upnpav_dms_getnetworkinterface
 ****************************************/

CgNetworkInterface *cg_upnpav_dms_getnetworkinterface(CgUpnpAvServer *dms)
{
	CgNetworkInterface *netIf = cg_upnpav_dms_getnetworkinterfaces(dms);
	if (netIf)
		return netIf;
	return NULL;
}

/****************************************
* cg_upnpav_dms_findcontentbyname
****************************************/

CgUpnpAvContent *cg_upnpav_dms_findcontentbytitle(CgUpnpAvServer *dms, char *title)
{
	return cg_upnpav_content_getbytitle(cg_upnpav_dms_getrootcontent(dms), title);
}

/****************************************
* cg_upnpav_dms_findcontentbyobjectid
****************************************/

CgUpnpAvContent *cg_upnpav_dms_findcontentbyid(CgUpnpAvServer *dms, char *objID)
{
	return cg_upnpav_content_getbyid(cg_upnpav_dms_getrootcontent(dms), objID);
}

