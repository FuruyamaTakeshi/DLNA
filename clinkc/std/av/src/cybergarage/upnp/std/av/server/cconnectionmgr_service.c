/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cconnectionmgr_service.c
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
* Service Description (Connection Manager)
****************************************/

static char *CG_UPNPAV_DMS_CONNECTIONMANAGER_SERVICE_DESCRIPTION = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\n"
"  <specVersion>\n"
"     <major>1</major>\n"
"     <minor>0</minor>\n"
"  </specVersion>\n"
"  <actionList>\n"
"     <action>\n"
"        <name>GetCurrentConnectionInfo</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>ConnectionID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>RcsID</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_RcsID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>AVTransportID</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_AVTransportID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>ProtocolInfo</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_ProtocolInfo</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>PeerConnectionManager</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_ConnectionManager</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>PeerConnectionID</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>Direction</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_Direction</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>Status</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_ConnectionStatus</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>GetProtocolInfo</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>Source</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>SourceProtocolInfo</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>Sink</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>SinkProtocolInfo</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>GetCurrentConnectionIDs</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>ConnectionIDs</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>CurrentConnectionIDs</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"   </actionList>\n"
"   <serviceStateTable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_ProtocolInfo</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_ConnectionStatus</name>\n"
"         <dataType>string</dataType>\n"
"         <allowedValueList>\n"
"            <allowedValue>OK</allowedValue>\n"
"            <allowedValue>ContentFormatMismatch</allowedValue>\n"
"            <allowedValue>InsufficientBandwidth</allowedValue>\n"
"            <allowedValue>UnreliableChannel</allowedValue>\n"
"            <allowedValue>Unknown</allowedValue>\n"
"         </allowedValueList>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_AVTransportID</name>\n"
"         <dataType>i4</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_RcsID</name>\n"
"         <dataType>i4</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_ConnectionID</name>\n"
"         <dataType>i4</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_ConnectionManager</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"yes\">\n"
"         <name>SourceProtocolInfo</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"yes\">\n"
"         <name>SinkProtocolInfo</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_Direction</name>\n"
"         <dataType>string</dataType>\n"
"         <allowedValueList>\n"
"            <allowedValue>Input</allowedValue>\n"
"            <allowedValue>Output</allowedValue>\n"
"         </allowedValueList>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"yes\">\n"
"         <name>CurrentConnectionIDs</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"   </serviceStateTable>\n"
"</scpd>\n";

/****************************************
* cg_upnpav_dms_conmgr_actionreceived
****************************************/

BOOL cg_upnpav_dms_conmgr_actionreceived(CgUpnpAction *action)
{
	CgUpnpAvServer *dms;
	CgUpnpDevice *dev;
	char *actionName;
	CgUpnpArgument *arg;
	CgString *protocolInfos;
	CgUpnpAvProtocolInfo *protocolInfo;

	actionName = cg_upnp_action_getname(action);
	if (cg_strlen(actionName) <= 0)
		return FALSE;

	dev = (CgUpnpDevice *)cg_upnp_service_getdevice(cg_upnp_action_getservice(action));
	if (!dev) 
		return FALSE;

	dms = (CgUpnpAvServer *)cg_upnp_device_getuserdata(dev);
	if (!dms)
		return FALSE;

	/* GetProtocolInfo*/
	if (cg_streq(actionName, CG_UPNPAV_DMS_CONNECTIONMANAGER_GET_PROTOCOL_INFO)) {
		arg = cg_upnp_action_getargumentbyname(action, CG_UPNPAV_DMS_CONNECTIONMANAGER_SOURCE);
		if (!arg)
			return FALSE;
		protocolInfos = cg_string_new();
		for (protocolInfo = cg_upnpav_dms_getprotocolinfos(dms); protocolInfo; protocolInfo = cg_upnpav_protocolinfo_next(protocolInfo)) {
			if (0 < cg_string_length(protocolInfos))
				cg_string_addvalue(protocolInfos, ",");
			cg_string_addvalue(protocolInfos, cg_upnpav_protocolinfo_getstring(protocolInfo));
		}
		cg_upnp_argument_setvalue(arg, cg_string_getvalue(protocolInfos));
		cg_string_delete(protocolInfos);
		return TRUE;
	}

	return FALSE;
}

/****************************************
* cg_upnpav_dms_conmgr_init
****************************************/

BOOL cg_upnpav_dms_conmgr_init(CgUpnpAvServer *dms)
{
	CgUpnpDevice *dev;
	CgUpnpService *service;
	CgUpnpAction *action;

	dev = cg_upnpav_dms_getdevice(dms);
	if (!dev)
		return FALSE;

	service = cg_upnp_device_getservicebytype(dev, CG_UPNPAV_DMS_CONNECTIONMANAGER_SERVICE_TYPE);
	if (!service)
		return FALSE;
	
	if (cg_upnp_service_parsedescription(service, CG_UPNPAV_DMS_CONNECTIONMANAGER_SERVICE_DESCRIPTION, cg_strlen(CG_UPNPAV_DMS_CONNECTIONMANAGER_SERVICE_DESCRIPTION)) == FALSE)
		return FALSE;

	cg_upnp_service_setuserdata(service, dms);
	for (action=cg_upnp_service_getactions(service); action; action=cg_upnp_action_next(action))
		cg_upnp_action_setuserdata(action, dms);

	return TRUE;
}
