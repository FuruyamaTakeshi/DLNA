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
*	File: cdevice_ssdp_server.c
*
*	Revision:
*
*	03/22/05
*		- first revision
*
*	10/31/05
*		- Corrected response to MSearch message for all ST
*		  (search target) types.
*	12/07/05
*		- No longer responds if MX header is empty
*		  or non-integer.
*
******************************************************************/

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/util/clog.h>
#include <cybergarage/util/cstring.h>

#include <ctype.h>
#include <string.h>

static int filter_duplicate_m_search(CgUpnpSSDPPacket *ssdpPkt);
static int simple_string_hash(char *str, int table_size);

/****************************************
* cg_upnp_device_ssdpmessagereceived
****************************************/

void cg_upnp_device_ssdpmessagereceived(CgUpnpDevice *dev, CgUpnpSSDPPacket *ssdpPkt, int filter)
{
	BOOL isRootDev;
	char *ssdpST;
	char *devUDN, *devType;
	char ssdpMsg[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	char deviceUSN[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
#if defined WINCE
	size_t n;
#else
	int n;
#endif
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	char *ssdpMXString;
	int ssdpMX;
	char *ssdpTargetAddr;

	cg_log_debug_l4("Entering...\n");

	ssdpMXString = cg_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_MX);
	ssdpST = cg_upnp_ssdp_packet_getst(ssdpPkt);

	/* Check if this ssdp packet has already been checked + filtered */
	if (filter)
	{
		
		/****************************************
		 * Request line
		 * Check the request line for errors, this is not ideal as it currently only
		 * checks for the presence of the strings and not the order.
		 ***************************************/
		/**** check for M-SEARCH and return if not found ****/
		if (cg_strstr(cg_string_getvalue(ssdpPkt->dgmPkt->data), CG_HTTP_MSEARCH) < 0)
			return;
		/**** check for * and return if not found ****/
		if (cg_strstr(cg_string_getvalue(ssdpPkt->dgmPkt->data), "*") < 0)
			return;
		/**** check HTTP version and return if not found ****/
		if (cg_strstr(cg_string_getvalue(ssdpPkt->dgmPkt->data),  CG_HTTP_VER11) < 0)
			return;

		/****************************************
		 * check HOST header, should always be 239.255.255.250:1900, return if incorrect
		 ***************************************/
		ssdpTargetAddr = cg_upnp_ssdp_packet_gethost(ssdpPkt);
		if (cg_strcmp(ssdpTargetAddr, CG_UPNP_SSDP_MULTICAST_ADDRESS) != 0 && !cg_net_isipv6address(ssdpTargetAddr) )
			return;

		/****************************************
		 * check MAN header, return if incorrect
		 ***************************************/
		if (cg_upnp_ssdp_packet_isdiscover(ssdpPkt) == FALSE)
			return;

		/****************************************
		 * check MX header, return if incorrect
		 ***************************************/
		if (ssdpMXString == NULL || cg_strlen(ssdpMXString)==0)
			/* return if the MX value does not exist or is empty */
			return;
		/* check if MX value is not an integer */
		for (n=0; n<strlen(ssdpMXString); n++) {
			if (isdigit(ssdpMXString[n]) == 0)
				/* MX value contains a non-digit so is invalid */
				return;
		}

		/****************************************
		 * check ST header and if empty return
		 ***************************************/
		if (cg_strlen(ssdpST) <= 0)
			return;

		/* Check if we have received this search recently
		 * and ignore duplicates. */
		if ( filter_duplicate_m_search(ssdpPkt) )
			return;

		ssdpMX = cg_upnp_ssdp_packet_getmx(ssdpPkt);
		cg_log_debug("Sleeping for a while... (MX:%d)\n", ssdpMX);
		cg_waitrandom((ssdpMX*1000)/4);
	}

	isRootDev = cg_upnp_device_isrootdevice(dev);
	
	if (cg_upnp_st_isalldevice(ssdpST) == TRUE) {
		/* for root device only */
		if (isRootDev == TRUE) {
			cg_upnp_device_getnotifydevicent(dev, ssdpMsg, sizeof(ssdpMsg));
			cg_upnp_device_getnotifydeviceusn(dev, deviceUSN, sizeof(deviceUSN));
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, ssdpMsg, deviceUSN);
		}
		/* for all devices send */
		/* device type : device version */
		cg_upnp_device_getnotifydevicetypent(dev, ssdpMsg, sizeof(ssdpMsg));
		cg_upnp_device_getnotifydevicetypeusn(dev, deviceUSN, sizeof(deviceUSN));
		cg_upnp_device_postsearchresponse(dev, ssdpPkt, ssdpMsg, deviceUSN);
		/* device UUID */
		cg_upnp_device_postsearchresponse(dev, ssdpPkt, cg_upnp_device_getudn(dev), cg_upnp_device_getudn(dev));
	}
	else if (cg_upnp_st_isrootdevice(ssdpST)  == TRUE) {
		if (isRootDev == TRUE) {
			cg_upnp_device_getnotifydeviceusn(dev, deviceUSN, sizeof(deviceUSN));
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, CG_UPNP_ST_ROOT_DEVICE, deviceUSN);
		}
	}
	else if (cg_upnp_st_isuuiddevice(ssdpST)  == TRUE) {
		devUDN = cg_upnp_device_getudn(dev);
		if (cg_streq(ssdpST, devUDN) == TRUE)
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, devUDN, devUDN);
	}
	else if (cg_upnp_st_isurn(ssdpST)  == TRUE) {
		devType = cg_upnp_device_getdevicetype(dev);
		if (cg_streq(ssdpST, devType) == TRUE) {
			cg_upnp_device_getnotifydevicetypeusn(dev, deviceUSN, sizeof(deviceUSN));
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, devType, deviceUSN);
		}
	}

	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service))
		cg_upnp_service_ssdpmessagereceived(service, ssdpPkt);

	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev))
		cg_upnp_device_ssdpmessagereceived(childDev, ssdpPkt, FALSE);
	

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_device_ssdplistener
****************************************/

void cg_upnp_device_ssdplistener(CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpDevice *dev;
	
	cg_log_debug_l4("Entering...\n");

	dev = (CgUpnpDevice *)cg_upnp_ssdp_packet_getuserdata(ssdpPkt);
	cg_upnp_device_ssdpmessagereceived(dev, ssdpPkt, TRUE);

	cg_log_debug_l4("Leaving...\n");
}

/* private methods */

static int filter_duplicate_m_search(CgUpnpSSDPPacket *ssdpPkt)
{
	CgSysTime *timestamps = ssdpPkt->timestamps;
	size_t s_length;
	int loc;
	char *id_string, *r_address, *st, port[6];
	CgSysTime curr_time;

	cg_log_debug_l4("Entering...\n");
	
	/* Initializing hash table to zero */
	if (!ssdpPkt->initialized) {
		ssdpPkt->initialized = 1;
		memset(timestamps, '\0', CG_UPNP_SSDP_FILTER_TABLE_SIZE * sizeof( CgSysTime ));
	}

	r_address = cg_string_getvalue(ssdpPkt->dgmPkt->remoteAddress);
	st = cg_upnp_ssdp_packet_getst(ssdpPkt);
	sprintf(port, "%d", ssdpPkt->dgmPkt->remotePort); 

	/* Catenating remote address string with ssdp ST header field. */
	s_length = strlen( r_address ) + strlen( st ) + strlen( port );
	id_string = (char *)malloc( s_length + 1 );

	if ( NULL == id_string )
	{
		cg_log_debug_s("Memory allocation problem!\n");
		return FALSE;
	}

	memset(id_string, '\0', s_length + 1);

	cg_strcat(id_string, r_address );
	cg_strcat(id_string, port);
	cg_strcat(id_string, st );
	
	loc = simple_string_hash(id_string, CG_UPNP_SSDP_FILTER_TABLE_SIZE);

	cg_log_debug("Calculated hash: %d\n", loc);

	free(id_string);

	curr_time = cg_getcurrentsystemtime();

	if ( 0 == timestamps[loc] ) {
		timestamps[loc] = curr_time;
		cg_log_debug("First packet... Updating hash table.\n");
		return FALSE;
	}	
	else if ( ( curr_time - timestamps[loc] ) < CG_UPNP_DEVICE_M_SEARCH_FILTER_INTERVAL ) {
		cg_log_debug("Filtering packet!\n");
		timestamps[loc] = curr_time;
		return TRUE;
	}
	else {
		timestamps[loc] = curr_time;
		cg_log_debug("Old timestamp found, just updating it.\n");
		return FALSE;
	}
	
	cg_log_debug_l4("Leaving...\n");
}

static int simple_string_hash(char *str, int table_size)
{
        int sum=0;

	cg_log_debug_l4("Entering...\n");
	
        if (str==NULL) return -1;
	
	cg_log_debug("Calculating hash from string |%s|, table size: %d\n", str, table_size);

	/* Sum up all the characters in the string */
	for( ; *str; str++) sum += *str;

	cg_log_debug_l4("Leaving...\n");

	return sum % table_size;
}
