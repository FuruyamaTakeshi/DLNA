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
*	File: cservice_ssdp_server.c
*
*	Revision:
*
*	06/08/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/upnp/cservice.h>
#include <cybergarage/upnp/ssdp/cssdp_server.h>
#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_service_ssdpmessagereceived
****************************************/

void cg_upnp_service_ssdpmessagereceived(CgUpnpService *service, CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpDevice *dev;
	char *ssdpST;
	char serviceNT[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	char serviceUSN[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	char *serviceType;
	
	cg_log_debug_l4("Entering...\n");

	ssdpST = cg_upnp_ssdp_packet_getst(ssdpPkt);
	if (cg_strlen(ssdpST) <= 0)
		return;

	dev = cg_upnp_service_getdevice(service);

	cg_upnp_service_getnotifyservicetypent(service, serviceNT, sizeof(serviceNT));
	cg_upnp_service_getnotifyservicetypeusn(service, serviceUSN, sizeof(serviceUSN));
	
	if (cg_upnp_st_isalldevice(ssdpST) == TRUE) {
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, serviceNT, serviceUSN);
	}
	else if (cg_upnp_st_isurn(ssdpST)  == TRUE) {
		serviceType = cg_upnp_service_getservicetype(service);
		if (cg_streq(ssdpST, serviceType) == TRUE)
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, serviceType, serviceUSN);
	}

	cg_log_debug_l4("Leaving...\n");
}
