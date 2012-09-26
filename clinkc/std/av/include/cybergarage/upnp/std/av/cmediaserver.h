/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cmediaserver.h
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifndef _CG_CLINKCAV_SERVER_H_
#define _CG_CLINKCAV_SERVER_H_

#include <cybergarage/upnp/cupnp.h>
#include <cybergarage/upnp/std/av/cupnpav.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Struct
****************************************/

typedef struct _CgUpnpAvServer {
CgMutex *mutex;
CgUpnpDevice *dev;
CgUpnpAvContent *rootContent;
int systemUpdateID;
CG_UPNPAV_HTTP_LISTENER httplistener;
CG_UPNPAV_ACTION_LISTNER actionListner;
CG_UPNPAV_STATEVARIABLE_LISTNER queryListner;
CgUpnpAvProtocolInfoList *protocolInfoList;
CgNetworkInterfaceList *networkInterfaceList;
void *userData;
} CgUpnpAvServer;

/****************************************
* Constants (Media Server)
****************************************/

#define CG_UPNPAV_DMS_DEVICE_TYPE "urn:schemas-upnp-org:device:MediaServer:1"
#define CG_UPNPAV_DMS_DEFAULT_HTTP_PORT 38520

/****************************************
* Constants (Content Directory)
****************************************/

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SERVICE_TYPE "urn:schemas-upnp-org:service:ContentDirectory:1"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE "Browse"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SEARCH "Search"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_GET_SEARCH_CAPABILITIES "GetSearchCapabilities"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SEARCH_CAPS "SearchCaps"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_GET_SORT_CAPABILITIES "GetSortCapabilities"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SORT_CAPS "SortCaps"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_GET_SYSTEM_UPDATE_ID "GetSystemUpdateID"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_ID "Id"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SYSTEM_UPDATE_ID "SystemUpdateID"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SEARCH_CAPABILITIES "SearchCapabilities"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SORT_CAPABILITIES "SortCapabilities"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_CONTENT_EXPORT_URI "/ExportContent"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_CONTENT_IMPORT_URI "/ImportContent"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_CONTENT_ID "id"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_DEFAULT_SYSTEMUPDATEID_INTERVAL 2000
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_DEFAULT_CONTENTUPDATE_INTERVAL 60000

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID "ObjectID"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_FLAG "BrowseFlag"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_FILTER "Filter"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_STARTING_INDEX "StartingIndex"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_REQUESTED_COUNT "RequestedCount"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_SORT_CRITERIA "SortCriteria"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_METADATA "BrowseMetadata"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_DIRECT_CHILDREN "BrowseDirectChildren"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_RESULT "Result"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_NUMBER_RETURNED "NumberReturned"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_TOTAL_MACHES "TotalMatches"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_UPDATE_ID "UpdateID"

/****************************************
* Constants (Connection Manager)
****************************************/

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_SERVICE_TYPE "urn:schemas-upnp-org:service:ConnectionManager:1"

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_HTTP_GET "http-get"

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_GET_PROTOCOL_INFO "GetProtocolInfo"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_SOURCE "Source"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_SINK "Sink"

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_PREPARE_FOR_CONNECTION "PrepareForConnection"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_REMOTE_PROTOCOL_INFO "RemoteProtocolInfo"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_PEER_CONNECTION_MANAGER "PeerConnectionManager"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_PEER_CONNECTION_ID "PeerConnectionID"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_DIRECTION "Direction"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_CONNECTION_ID "ConnectionID"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_AV_TRNSPORT_ID "AVTransportID"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_RCS_ID "RcsID"

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_CONNECTION_COMPLETE "ConnectionComplete"

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_GET_CURRENT_CONNECTION_IDS "GetCurrentConnectionIDs"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_CONNECTION_IDS "ConnectionIDs"

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_GET_CURRENT_CONNECTION_INFO "GetCurrentConnectionInfo"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_PROTOCOL_INFO "ProtocolInfo"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_STATUS "Status"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_INPUT "Input"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_OUTPUT "Output"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_OK "OK"

/****************************************
* Constants (Media Receiver, Microsoft)
****************************************/

#define CG_UPNPAV_DMS_MEDIARECEIVER_SERVICE_TYPE "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1"
#define CG_UPNPAV_DMS_MEDIARECEIVER_REGISTRATION_RESP_MSG "RegistrationRespMsg"
#define CG_UPNPAV_DMS_MEDIARECEIVER_REGISTER_DEVICE "RegisterDevice"
#define CG_UPNPAV_DMS_MEDIARECEIVER_IS_AUTHORIZED "IsAuthorized"
#define CG_UPNPAV_DMS_MEDIARECEIVER_IS_VALIDATED "IsValidated"
#define CG_UPNPAV_DMS_MEDIARECEIVER_RESULT "Result"



/****************************************
* Public Functions
****************************************/

CgUpnpAvServer *cg_upnpav_dms_new();
void cg_upnpav_dms_delete(CgUpnpAvServer *dms);

#define cg_upnpav_dms_getdevice(dms) (dms->dev)
#define cg_upnpav_dms_getrootcontent(dms) (dms->rootContent)

#define cg_upnpav_dms_start(dms) cg_upnp_device_start(dms->dev)
#define cg_upnpav_dms_stop(dms) cg_upnp_device_stop(dms->dev)

#define cg_upnpav_dms_lock(dms) cg_mutex_lock(dms->mutex)
#define cg_upnpav_dms_unlock(dms) cg_mutex_unlock(dms->mutex)

#define cg_upnpav_dms_setfriendlyname(dms, value) cg_upnp_device_setfriendlyname(dms->dev, value)
#define cg_upnpav_dms_getfriendlyname(dms) cg_upnp_device_getfriendlyname(dms->dev)

#define cg_upnpav_dms_setudn(dms, value) cg_upnp_device_setudn(dms->dev, value)
#define cg_upnpav_dms_getudn(dms) cg_upnp_device_getudn(dms->dev)

CgUpnpAvContent *cg_upnpav_dms_findcontentbytitle(CgUpnpAvServer *dms, char *name);
CgUpnpAvContent *cg_upnpav_dms_findcontentbyid(CgUpnpAvServer *dms, char *objectID);

#define cg_upnpav_dms_sethttplistener(dms,func) (dms->httplistener = func)
#define cg_upnpav_dms_gethttplistener(dms) (dms->httplistener)

#define cg_upnpav_dms_setactionlistener(dms,func) (dms->actionListner = func)
#define cg_upnpav_dms_getactionlistener(dms) (dms->actionListner)

#define cg_upnpav_dms_setquerylistener(dms,func) (dms->queryListner = func)
#define cg_upnpav_dms_getquerylistener(dms) (dms->queryListner)

#define cg_upnpav_dms_setuserdata(dms,data) (dms->userData = data)
#define cg_upnpav_dms_getuserdata(dms) (dms->userData)

#define cg_upnpav_dms_addprotocolinfo(dms, info) cg_upnpav_protocolinfolist_add(dms->protocolInfoList, info)
#define cg_upnpav_dms_getprotocolinfos(dms) cg_upnpav_protocolinfolist_gets(dms->protocolInfoList)

/****************************************
* Connection Manager
****************************************/

BOOL cg_upnpav_dms_conmgr_init(CgUpnpAvServer *dms);

/****************************************
* Network Interface
****************************************/

#define cg_upnpav_dms_getnetworkinterfaces(dms) cg_net_interfacelist_gets(dms->networkInterfaceList)
#define cg_upnpav_dms_getnnetworkinterfaces(dms) cg_net_interfacelist_size(dms->networkInterfaceList)

BOOL cg_upnpav_dms_updatenetworkinterfaces(CgUpnpAvServer *dms);
CgNetworkInterface *cg_upnpav_dms_getnetworkinterface(CgUpnpAvServer *dms);

/****************************************
* Content Directory
****************************************/

BOOL cg_upnpav_dms_condir_init(CgUpnpAvServer *dms);

void cg_upnpav_dms_condir_setsystemupdateid(CgUpnpAvServer *dms, int id);
int cg_upnpav_dms_condir_getsystemupdateid(CgUpnpAvServer *dms);
void cg_upnpav_dms_condir_updatesystemupdateid(CgUpnpAvServer *dms);

/****************************************
* Media Receiveer
****************************************/

BOOL cg_upnpav_dms_medrec_init(CgUpnpAvServer *dms);


#ifdef  __cplusplus
}
#endif

#endif
