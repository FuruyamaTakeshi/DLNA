/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2008
*
*	File: upnpigddump.c
*
*	Revision:
*       08/05/24
*               - first release.
*
************************************************************/

#include <cybergarage/upnp/cupnp.h>

#define UPNPAVDUMP_IGD_DEVICETYPE "urn:schemas-upnp-org:device:InternetGatewayDevice:1"

#define UPNPAVDUMP_IGD_WANIPCON_SERVICETYPE "urn:schemas-upnp-org:service:WANIPConnection:1"
#define UPNPAVDUMP_IGD_WANCOMIFCFG_SERVICETYPE "urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1"

/////////////////////////////////////////////////////////////////////////////////
// PrintIGDInfo
/////////////////////////////////////////////////////////////////////////////////

void PrintIGDInfo(CgUpnpDevice *dev, int igdNum)
{
	CgUpnpService *ipConService;
	CgUpnpAction *extIpAddrAction;
	CgUpnpService *wanComIfCfgService;
	CgUpnpAction *totalBytesSentAction;
	CgUpnpAction *totalBytesRecvAction;

	if (!cg_upnp_device_isdevicetype(dev, UPNPAVDUMP_IGD_DEVICETYPE))
		return;
	
	printf("[%d] : %s\n", igdNum, cg_upnp_device_getfriendlyname(dev));

	ipConService = cg_upnp_device_getservicebytype(dev, UPNPAVDUMP_IGD_WANIPCON_SERVICETYPE);
	if (ipConService) {
		extIpAddrAction = cg_upnp_service_getactionbyname(ipConService, "GetExternalIPAddress");
		if (extIpAddrAction) {
			if (cg_upnp_action_post(extIpAddrAction))
				printf("  GetExternalIPAddress = %s\n", cg_upnp_action_getargumentvaluebyname(extIpAddrAction, "NewExternalIPAddress"));
		}
	}

	wanComIfCfgService = cg_upnp_device_getservicebytype(dev, UPNPAVDUMP_IGD_WANCOMIFCFG_SERVICETYPE);
	if (wanComIfCfgService) {
		totalBytesSentAction = cg_upnp_service_getactionbyname(ipConService, "GetTotalBytesSent");
		if (totalBytesSentAction) {
			if (cg_upnp_action_post(totalBytesSentAction))
				printf("  GetTotalBytesSent = %s\n", cg_upnp_action_getargumentvaluebyname(totalBytesSentAction, "NewTotalBytesSent"));
		}
		totalBytesRecvAction = cg_upnp_service_getactionbyname(ipConService, "GetTotalBytesReceived");
		if (totalBytesRecvAction) {
			if (cg_upnp_action_post(totalBytesRecvAction))
				printf("  GetTotalBytesSent = %s\n", cg_upnp_action_getargumentvaluebyname(totalBytesRecvAction, "NewTotalBytesReceived"));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
// PrintIGDInfos
/////////////////////////////////////////////////////////////////////////////////

void PrintIGDInfos(CgUpnpControlPoint *ctrlPoint)
{
	CgUpnpDevice *dev;
	int igdNum;
		
	igdNum = 0;
	for (dev = cg_upnp_controlpoint_getdevices(ctrlPoint); dev != NULL; dev = cg_upnp_device_next(dev)) {
		if (cg_upnp_device_isdevicetype(dev, UPNPAVDUMP_IGD_DEVICETYPE))
			PrintIGDInfo(dev, ++igdNum);
	}

	if (igdNum <= 0)
		printf("IGD is not found !!\n");
}

/////////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////////

#if defined(TENGINE)
MBEG
#else
int main( int argc, char* argv[] )
#endif
{
	CgUpnpControlPoint *ctrlPoint;

	ctrlPoint = cg_upnp_controlpoint_new();
	if (cg_upnp_controlpoint_start(ctrlPoint) == FALSE) {
		printf("Couldn't start this control point !!");
		exit(1);
	}
	
	cg_upnp_controlpoint_search(ctrlPoint, CG_UPNP_ST_ROOT_DEVICE);

	cg_sleep(cg_upnp_controlpoint_getssdpsearchmx(ctrlPoint) * 1000);

	PrintIGDInfos(ctrlPoint);
	
	cg_upnp_controlpoint_stop(ctrlPoint);
	cg_upnp_controlpoint_delete(ctrlPoint);
	
	return(0);
}
