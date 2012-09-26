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
*	File: cdevice.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*	10/30/05
*		- Thanks for Makela Aapo (aapo.makela@nokia.com)
*		- Added cg_upnp_device_getservicebysid().
*	10/31/05
*		- cg_upnp_device_getdevicebyname: Changed dev to childDev in for
*		- cg_upnp_device_byebyefrom & _announcefrom: Added missing 
*		  advertisement and byebye messages
*		- Added EXT handling to postsearchresponse
*	11/17/05
*		- Fixed many wrong variable names in for loops (dev => childDev)
*	01/09/06 Heikki Junnila
*		- Added cg_upnp_device_getservicebytype() to enable
*		  searching for services without version information.
*	10-Jan-06 Heikki Junnila
*		- Renamed cg_upnp_device_getservicebyname to
*		  cg_upnp_device_getservicebyexacttype and created a compat
*		  macro for _getservicebyname
*	11-Jan-06 Heikki Junnila
*		- Removed cg_upnp_device_isname and _getbyname because
*		  according to UPnP specs UDN, type or friendlyname is
*		  not the same as the device's name.
*		- Added cg_upnp_device_getdevicebyudn()
*	23-Jan-06 Aapo Makela
*		- Modified to try the next HTTP-port, if opening a port fails
*	05-May-06 Rosfran Borges
*		- Propagate some important data (description URI, lease-time), 
*		  from the root device, to the embededd sub-devices, because
*		  all embedded devices advertisements goes with wrong Location
*		  path.
*	06/13/07 Fabrice Fontaine Orange
*		- Fixed compilation issue when using DCG_UPNP_NOUSE_CONTROLPOINT flag in cg_upnp_device_updatefromssdppacket().
*		- Fixed a memory leak in cg_upnp_device_getservicebycontrolurl().
*	12/08/08
 *		- Added cg_upnp_device_getabsoluteiconurl().
*
******************************************************************/

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/upnp/ccontrolpoint.h>
#include <cybergarage/upnp/cupnp_limit.h>
#include <cybergarage/util/ctime.h>
#include <cybergarage/net/cinterface.h>
#include <cybergarage/net/curl.h>
#include <cybergarage/upnp/cupnp_function.h>
#include <cybergarage/upnp/ssdp/cssdp.h>
#include <cybergarage/upnp/ssdp/cssdp_server.h>
#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/io/cfile.h>
#include <cybergarage/util/clog.h>
#include <cybergarage/util/cstring.h>

/****************************************
* prototype define for static functions
****************************************/

static void cg_upnp_device_initchildnodes(CgUpnpDevice *dev);
static void cg_upnp_device_initdevicelist(CgUpnpDevice *dev);
static void cg_upnp_device_initservicelist(CgUpnpDevice *dev);
static void cg_upnp_device_initiconlist(CgUpnpDevice *dev);

static void cg_upnp_device_notifywait(CgUpnpDevice *dev);

/****************************************
* cg_upnp_device_new
****************************************/

CgUpnpDevice *cg_upnp_device_new()
{
	CgUpnpDevice *dev;

	cg_log_debug_l4("Entering...\n");

	dev = (CgUpnpDevice *)malloc(sizeof(CgUpnpDevice));

	if  ( NULL != dev )
	{
		cg_list_node_init((CgList *)dev);

#ifdef CG_HTTP_USE_PERSISTENT_CONNECTIONS
		cg_http_persistentconnection_init();
#endif

		dev->rootNodeList = NULL;
		dev->deviceNode = NULL;

		dev->deviceList = cg_upnp_devicelist_new();
		dev->serviceList = cg_upnp_servicelist_new();
		dev->iconList = cg_upnp_iconlist_new();
		
		dev->mutex = cg_mutex_new();
		
		dev->httpServerList = cg_http_serverlist_new();
		dev->ssdpServerList = cg_upnp_ssdp_serverlist_new();
		
		dev->advertiser = cg_thread_new();
		
		dev->descriptionURI = cg_string_new();

		dev->ssdpPkt = cg_upnp_ssdp_packet_new();
		
		dev->ifCache = NULL;
		
		cg_upnp_device_setparentdevice(dev, NULL);
		cg_upnp_device_setdescriptionuri(dev, CG_UPNP_DEVICE_DEFAULT_DESCRIPTION_URI);
		cg_upnp_device_setleasetime(dev, CG_UPNP_DEVICE_DEFAULT_LEASE_TIME);
		cg_upnp_device_sethttpport(dev, CG_UPNP_DEVICE_HTTP_DEFAULT_PORT);
		cg_upnp_device_sethttplistener(dev, NULL);
		cg_upnp_device_setuserdata(dev, NULL);
	}
	
	cg_log_debug_l4("Leaving...\n");

	return dev;
}

/****************************************
* cg_upnp_device_delete
****************************************/

void cg_upnp_device_delete(CgUpnpDevice *dev)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)dev);
	
	/* Delete cached interfaces */
	if (dev->ifCache != NULL)
		cg_net_interfacelist_delete(dev->ifCache);
	
	cg_upnp_device_clear(dev);
	
	cg_upnp_devicelist_delete(dev->deviceList);
	cg_upnp_servicelist_delete(dev->serviceList);
	cg_upnp_iconlist_delete(dev->iconList);

	cg_mutex_delete(dev->mutex);
	
	cg_http_serverlist_delete(dev->httpServerList);
	cg_upnp_ssdp_serverlist_delete(dev->ssdpServerList);
	cg_thread_delete(dev->advertiser);
	
	cg_string_delete(dev->descriptionURI);

	cg_upnp_ssdp_packet_delete(dev->ssdpPkt);
	
	free(dev);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_device_clear
****************************************/

void cg_upnp_device_clear(CgUpnpDevice *dev)
{
	cg_log_debug_l4("Entering...\n");

	if (dev->rootNodeList != NULL) {
		cg_xml_nodelist_delete(dev->rootNodeList);
		dev->rootNodeList = NULL;
	}
	dev->deviceNode = NULL;
	
	cg_upnp_devicelist_clear(dev->deviceList);
	cg_upnp_servicelist_clear(dev->serviceList);
	cg_upnp_iconlist_clear(dev->iconList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_device_setdevicenode
****************************************/

void cg_upnp_device_setdevicenode(CgUpnpDevice *dev, CgXmlNode *node)
{
	cg_log_debug_l4("Entering...\n");

	dev->deviceNode = node;
	cg_upnp_device_initchildnodes(dev);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_device_getrootdevice
****************************************/

CgUpnpDevice *cg_upnp_device_getrootdevice(CgUpnpDevice *dev)
{
	CgUpnpDevice *rootDev;
	
	cg_log_debug_l4("Entering...\n");

	rootDev = dev;
	while (cg_upnp_device_getparentdevice(rootDev) != NULL)
		rootDev = cg_upnp_device_getparentdevice(rootDev);
	
	cg_log_debug_l4("Leaving...\n");

	return rootDev;
}

/****************************************
* cg_upnp_device_parsedescription
****************************************/

BOOL cg_upnp_device_parsedescription(CgUpnpDevice *dev, char *desciption, int descriptionLen)
{
	CgXmlParser *xmlParser;
	BOOL xmlParseSuccess;
	CgXmlNode *rootNode;
	char uuidBuf[CG_UPNP_UUID_MAX_LEN];
	
	cg_log_debug_l4("Entering...\n");

	cg_upnp_device_clear(dev);
	dev->rootNodeList = cg_xml_nodelist_new();

	xmlParser = cg_xml_parser_new();
	xmlParseSuccess = cg_xml_parse(xmlParser, dev->rootNodeList, desciption, descriptionLen);
	cg_xml_parser_delete(xmlParser);
	if (xmlParseSuccess == FALSE)
		return FALSE;

	if (cg_xml_nodelist_size(dev->rootNodeList) <= 0) {
		cg_upnp_device_clear(dev);
		return FALSE;
	}

	rootNode = cg_upnp_device_getrootnode(dev);
	if (rootNode == NULL) {
		cg_upnp_device_clear(dev);
		return FALSE;
	}
		
	dev->deviceNode = cg_xml_node_getchildnode(rootNode, CG_UPNP_DEVICE_ELEM_NAME);
	if (dev->deviceNode == NULL) {
		cg_upnp_device_clear(dev);
		return FALSE;
	}

	if (cg_upnp_device_hasudn(dev) == FALSE)
		cg_upnp_device_setudn(dev, cg_upnp_createuuid(uuidBuf, sizeof(uuidBuf)));
	
	cg_upnp_device_initchildnodes(dev);
	
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_upnp_device_parsedescriptionurl
****************************************/

BOOL cg_upnp_device_parsedescriptionurl(CgUpnpDevice *dev, CgNetURL *url)
{
	char *host;
	int port;
	char *request;
	CgHttpRequest *httpReq;
	CgHttpResponse *httpRes;
	/*int statusCode;*/
	char *content;
	CgInt64 contentLen;
	BOOL parseSuccess;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_net_url_ishttpprotocol(url) == FALSE)
		return FALSE;
		
	host = cg_net_url_gethost(url);
	port = cg_net_url_getport(url);
	if (port <= 0)
		port = CG_HTTP_DEFAULT_PORT;
	request = cg_net_url_getrequest(url);
	
	httpReq = cg_http_request_new();
	cg_http_request_setmethod(httpReq, CG_HTTP_GET);
	cg_http_request_seturi(httpReq, request);
	cg_http_request_setcontentlength(httpReq, 0);
	httpRes = cg_http_request_post(httpReq, host, port);

	/*statusCode = cg_http_response_getstatuscode(httpRes);
          if (statusCode != CG_HTTP_STATUS_OK) {*/
	if ( !cg_http_response_issuccessful(httpRes)) {
		cg_http_request_delete(httpReq);
		return FALSE;
	}
	
	content = cg_http_response_getcontent(httpRes);
	contentLen = cg_http_response_getcontentlength(httpRes);

	parseSuccess = cg_upnp_device_parsedescription(dev, content, contentLen);

	cg_http_request_delete(httpReq);
	
	cg_log_debug_l4("Leaving...\n");

	return parseSuccess;
}

/****************************************
* cg_upnp_device_loaddescriptionfile
****************************************/

#if defined(CG_USE_CFILE)

BOOL cg_upnp_device_loaddescriptionfile(CgUpnpDevice *dev, char *fileName)
{
	CgFile *file;
	char *description;
	int descriptionLen;
	BOOL parseSuccess;

	cg_log_debug_l4("Entering...\n");

	file = cg_file_new();
	cg_file_setname(file, fileName);
	if (cg_file_load(file) == FALSE) {
		cg_file_delete(file);
		return FALSE;
	}

	description = cg_file_getcontent(file);
	descriptionLen = cg_strlen(description);

	parseSuccess = cg_upnp_device_parsedescription(dev, description, descriptionLen);

	cg_file_delete(file);

	cg_log_debug_l4("Leaving...\n");

	return parseSuccess;
}

#endif

/**
 * Update the device's contents from an SSDP packet if necessary.
 *
 * @param dev The device to potentially update
 * @param ssdpPkt The SSDP packet to make decisions on
 * @return TRUE if the device was updated; otherwise FALSE
 */
BOOL cg_upnp_device_updatefromssdppacket(CgUpnpDevice* dev,
					 CgUpnpSSDPPacket* ssdpPkt)
{
	char *usn = NULL;
	char udn[CG_UPNP_UDN_LEN_MAX];
	CgUpnpSSDPPacket *tmpSsdpPkt = NULL;
	char* oldLocation = NULL;
	char* newLocation = NULL;
	CgNetURL *url = NULL;
	
	cg_log_debug_l4("Entering...\n");

	usn = cg_upnp_ssdp_packet_getusn(ssdpPkt);
	cg_upnp_usn_getudn(usn, udn, sizeof(udn));
	
	tmpSsdpPkt = cg_upnp_device_getssdppacket(dev);
	if (tmpSsdpPkt == NULL)
	{
		return FALSE;
	}
		
	/* Here we check, if the location of the device has changed */
	oldLocation = cg_upnp_ssdp_packet_getlocation(tmpSsdpPkt);
	newLocation = cg_upnp_ssdp_packet_getlocation(ssdpPkt);

	if (cg_streq(oldLocation, newLocation) == TRUE)
	{
		/* The device's location has not changed, just update
		   the SSDP packet */
		cg_upnp_device_setssdppacket(dev, ssdpPkt);
		
		return TRUE;
	}
	else
	{
		/* The device's location HAS changed. We must get a new
		   description. */
		cg_upnp_device_setssdppacket(dev, ssdpPkt);
		
		url = cg_net_url_new();
		if (url == NULL)
		{
			return FALSE;
		}
		
		/* Use the new location as the description URL */
		cg_net_url_set(url, newLocation);
		
		/* Get a new description for the device */
		cg_upnp_device_parsedescriptionurl(dev, url);
		
		cg_net_url_delete(url);
		
/* ADD Fabrice Fontaine Orange 16/04/2007 */
/* Bug correction : Solving compilation issue when using DCG_UPNP_NOUSE_CONTROLPOINT flag */
#ifndef CG_UPNP_NOUSE_CONTROLPOINT
/* ADD END Fabrice Fontaine Orange 16/04/2007 */
#ifndef CG_OPTIMIZED_CP_MODE
		if (cg_upnp_controlpoint_parseservicesfordevice(dev, ssdpPkt) == FALSE)
		{
			cg_upnp_device_delete(dev);
			return FALSE;
		}
#endif
/* ADD Fabrice Fontaine Orange 16/04/2007 */
#endif
/* ADD END Fabrice Fontaine Orange 16/04/2007 */
	}

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/*****************************************************************************
 * Device Type
 *****************************************************************************/

/** 
 * Get the identifier-part of a device type string (usually "urn") 
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref cg_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* cg_upnp_devicetype_getidentifier(char* deviceType)
{
	char* part = NULL;
	int tail = 0;
	int len = 0;
	
	cg_log_debug_l4("Entering...\n");

	if (deviceType == NULL)
	{
		return NULL;
	}
	
	len = cg_strlen(deviceType);
	
	/* Find the next delimiter ':' character */
        for (tail = 0; tail < len; tail++)
        {
                if (deviceType[tail] == ':')
                {
                        tail--; /* Don't take the colon */
                        break;
                }
        }

        /* Copy the part to a new buffer */
        if (tail > 0 && tail < len)
        {
		part = (char*) malloc(tail + 2);

		if ( NULL != part )
		{
			cg_strncpy(part, deviceType, tail + 1);
			part[tail + 1] = '\0';
		}
		else
			cg_log_debug_s("Memory allocation problem!\n");
        }
        else
        {
		part = NULL;
        }

	cg_log_debug_l4("Leaving...\n");

	return part;
}

/** 
 * Get the URN part of a device type string (usually "schemas-upnp-org") 
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref cg_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* cg_upnp_devicetype_geturn(char* deviceType)
{
	char* part = NULL;
	int tail = 0;
	int head = 0;
	int len = 0;
	
	cg_log_debug_l4("Entering...\n");

	if (deviceType == NULL)
	{
		return NULL;
	}

	len = cg_strlen(deviceType);
	
	/* Find the first delimiter ':' character */
	for (head = 0; head < len; head++)
	{
		if (deviceType[head] == ':')
		{
			head++; /* Don't include the colon */
			break;
                }
        }

        /* Find the next delimiter ':' character */
        for (tail = head; tail < len; tail++)
        {
                if (deviceType[tail] == ':')
                {
                        tail--; /* Don't take the colon */
                        break;
                }
        }


        /* Copy the part to a new buffer */
        if (head > 0 && head < len &&
            tail > 0 && tail < len)
        {
                part = (char*) malloc(tail - head + 2);

		if ( NULL != part )
		{
                	cg_strncpy(part, deviceType + head, tail - head + 1);
	                part[tail - head + 1] = '\0';
		}
		else
			cg_log_debug_s("Memory allocation problem!\n");
        }
        else
        {
		part = NULL;
        }

	cg_log_debug_l4("Leaving...\n");

        return part;
}

/** 
 * Get the device part of a device type string (usually just "device")
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref cg_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* cg_upnp_devicetype_getdevice(char* deviceType)
{
	char* part = NULL;
	int tail = 0;
	int head = 0;
	int len = 0;
	int count = 0;

	cg_log_debug_l4("Entering...\n");

	if (deviceType == NULL)
	{
		return NULL;
	}

	len = cg_strlen(deviceType);
	
	/* Skip the first two delimiter ':' characters */
        for (head = 0; head < len; head++)
        {
		if (deviceType[head] == ':')
		{
                        if (count == 1)
                        {
				head++; /* Don't include the colon */
				break;
                        }
                        else
                        {
				count++;
                        }
                }
        }

        /* Find the next delimiter ':' character */
        for (tail = head; tail < len; tail++)
        {
                if (deviceType[tail] == ':')
                {
                        tail--; /* Don't take the colon */
                        break;
                }
        }


        /* Copy the part to a new buffer */
        if (head > 0 && head < len &&
            tail > 0 && tail < len)
        {
                part = (char*) malloc(tail - head + 2);

		if ( NULL != part )
		{
                	cg_strncpy(part, deviceType + head, tail - head + 1);
	                part[tail - head + 1] = '\0';
		}
		else
			cg_log_debug_s("Memory allocation problem!\n");
        }
	else
        {
		part = NULL;
        }

	cg_log_debug_l4("Leaving...\n");

        return part;
}

/** 
 * Get the type part of a device type string (ex. "ContentDirectory")
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref cg_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* cg_upnp_devicetype_gettype(char* deviceType)
{
	char* part = NULL;
	int tail = 0;
	int head = 0;
	int len = 0;
	int count = 0;
	
	cg_log_debug_l4("Entering...\n");

	if (deviceType == NULL)
	{
		return NULL;
	}
	
	len = cg_strlen(deviceType);
	
	/* Skip the first three delimiter ':' characters */
        for (head = 0; head < len; head++)
        {
		if (deviceType[head] == ':')
		{
                        if (count == 2)
                        {
				head++; /* Don't include the colon */
				break;
                        }
                        else
                        {
				count++;
                        }
                }
        }

        /* Find the next delimiter ':' character */
        for (tail = head; tail < len; tail++)
        {
                if (deviceType[tail] == ':')
                {
                        tail--; /* Don't take the colon */
                        break;
                }
        }


        /* Copy the part to a new buffer */
        if (head > 0 && head < len &&
            tail > 0 && tail < len)
        {
                part = (char*) malloc(tail - head + 2);

		if ( NULL != part )
		{
                	cg_strncpy(part, deviceType + head, tail - head + 1);
	                part[tail - head + 1] = '\0';
		}
		else
			cg_log_debug_s("Memory allocation problem!\n");
        }
	else
        {
		part = NULL;
        }

	cg_log_debug_l4("Leaving...\n");

        return part;
}

/** 
 * Get the schema type part of a device type string (without last colon)
 * (ex. "urn:schemas-upnp-org:device:ContentDirectory")
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref cg_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* cg_upnp_devicetype_getschematype(char* deviceType)
{
	char* part = NULL;
	int tail = 0;
	int len = 0;
	int count = 0;
	
	cg_log_debug_l4("Entering...\n");

	if (deviceType == NULL)
	{
		return NULL;
	}
	
	len = cg_strlen(deviceType);
	
	/* Skip the first four delimiter ':' characters */
        for (tail = 0; tail < len; tail++)
        {
		if (deviceType[tail] == ':')
		{
                        if (count == 3)
                        {
				tail--; /* Don't include the colon */
				break;
                        }
                        else
                        {
				count++;
                        }
                }
        }	
	
        /* Copy the part to a new buffer */
        if (tail > 0 && tail < len)
        {
                part = (char*) malloc(tail + 2);

		if ( NULL != part )
		{
                	cg_strncpy(part, deviceType, tail + 1);
	                part[tail + 1] = '\0';
		}
		else
			cg_log_debug_s("Memory allocation problem!\n");
        }
	else
        {
		part = NULL;
        }

	cg_log_debug_l4("Leaving...\n");

	return part;
}

/** 
 * Get the version part of a device type string (ex. "1")
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref cg_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* cg_upnp_devicetype_getversion(char* deviceType)
{
	char* part = NULL;
	int tail = 0;
	int head = 0;
	int len = 0;
	int count = 0;

	cg_log_debug_l4("Entering...\n");

	if (deviceType == NULL)
	{
		return NULL;
	}
	
	len = cg_strlen(deviceType);
	
        /* Skip the first four delimiter ':' characters */
        for (head = 0; head < len; head++)
        {
                if (deviceType[head] == ':')
                {
                        if (count == 3)
                        {
				head++; /* Don't include the colon */
                                break;
                        }
                        else
                        {
                                count++;
                        }
                }
        }

	/* Find the next delimiter ':' character (or stop at the end) */
        for (tail = head; tail < len; tail++)
        {
                if (deviceType[tail] == ':')
                {
                        tail--; /* Don't take the colon */
                        break;
                }
        }
	
        /* Copy the part to a new buffer */
        if (head > 0 && head < len &&
            tail > 0 && tail <= len)
        {
                part = (char*) malloc(tail - head + 2);

		if ( NULL != part )
		{
                	cg_strncpy(part, deviceType + head, tail - head + 1);
	                part[tail - head + 1] = '\0';
		}
		else
			cg_log_debug_s("Memory allocation problem!\n");
        }
        else
        {
                part = NULL;
        }

	cg_log_debug_l4("Leaving...\n");
    
    return part;
}


/****************************************
*
* Child Node
*
****************************************/

/****************************************
* cg_upnp_device_initchildnodes
****************************************/

static void cg_upnp_device_initchildnodes(CgUpnpDevice *dev)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_device_initdevicelist(dev);
	cg_upnp_device_initservicelist(dev);
	cg_upnp_device_initiconlist(dev);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
*
* Embedded Device
*
****************************************/

/****************************************
* cg_upnp_device_initdevicelist
****************************************/

static void cg_upnp_device_initdevicelist(CgUpnpDevice *dev)
{
	CgXmlNode *devNode;
	CgXmlNode *devListNode;
	CgXmlNode *childNode;
	CgUpnpDevice *childDev;
	
	cg_log_debug_l4("Entering...\n");

	cg_upnp_devicelist_clear(dev->deviceList);
	
	devNode = cg_upnp_device_getdevicenode(dev);
	if (devNode == NULL)
		return;
		
	devListNode = cg_xml_node_getchildnode(devNode, CG_UPNP_DEVICELIST_ELEM_NAME);
	if (devListNode == NULL)
		return;
	
	for (childNode = cg_xml_node_getchildnodes(devListNode); childNode != NULL; childNode = cg_xml_node_next(childNode)) {
		if (cg_upnp_device_isdevicenode(childNode) == FALSE)
			continue;
		childDev = cg_upnp_device_new();
		/* Propagate "lease-time" from parent device to the child-devices; call "setleasetime"
		   (line below added by: rborges) */
		cg_upnp_device_setleasetime(childDev, cg_upnp_device_getleasetime(dev));
                /* Propagate important data (description URI, HTTP port) from the parent
	         * device to the child-devices; call "setdescriptionuri" and 
		 * "sethttpport" - (rosfran.borges) */
		cg_upnp_device_setdescriptionuri(childDev, cg_upnp_device_getdescriptionuri(dev));
		cg_upnp_device_sethttpport(childDev, cg_upnp_device_gethttpport(dev));		
		cg_upnp_device_setdevicenode(childDev, childNode);
		cg_upnp_devicelist_add(dev->deviceList, childDev);
		cg_upnp_device_setparentdevice(childDev, dev);
	} 

	cg_log_debug_l4("Leaving...\n");
}

/**
 * Find a device from the device's children by the type of the device.
 * This function searches for devices, whose *type part* (i.e. not including
 * the version) of the device type string matches the given string.
 * For example: "urn:schemas-upnp-org:device:FooDevice". If you need
 * to know the version of a device, use \ref cg_upnp_devicetype_getversion
 *
 * \param dev Device in question
 * \param type Type of the device
 *
 */
CgUpnpDevice *cg_upnp_device_getdevicebytype(CgUpnpDevice *dev, char *type)
{
	CgUpnpDevice *childDev = NULL;
	CgUpnpDevice *moreChildDev = NULL;
	char* typeString = NULL;
	char* part = NULL;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(type) <= 0 || dev == NULL)
	{
		return NULL;
	}
	
	for (childDev = cg_upnp_device_getdevices(dev); 
	     childDev != NULL;
	     childDev = cg_upnp_device_next(childDev))
	{
		typeString = cg_upnp_device_getdevicetype(childDev);
		if (typeString != NULL)
		{
			part = cg_upnp_devicetype_getschematype(typeString);
			if (cg_strcmp(part, type) == 0)
			{
				free(part);
				return childDev;
			}
			else
			{
				free(part);
			}
		}
				
		moreChildDev = cg_upnp_device_getdevicebytype(childDev, type);
		if (moreChildDev != NULL)
		{
			return moreChildDev;
		}
	}

	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/**
 * Find a device from the device's children by the type of the device.
 * This function searches for devices, whose *complete type string*
 * matches the given string, including version number. For example:
 * "urn:schemas-upnp-org:device:FooDevice:1". If you need to
 * disregard the version number, use \ref cg_upnp_device_getdevicebytype
 *
 * \param dev Device in question
 * \param exacttype Type of the device
 * 
 */
CgUpnpDevice* cg_upnp_device_getdevicebyexacttype(CgUpnpDevice* dev,
						  char* exacttype)
{
	CgUpnpDevice *childDev = NULL;
	CgUpnpDevice *moreChildDev = NULL;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(exacttype) <= 0 || dev == NULL)
	{
		return NULL;
	}
	
	for (childDev = cg_upnp_device_getdevices(dev); 
	     childDev != NULL;
	     childDev = cg_upnp_device_next(childDev))
	{
		if (cg_strcmp(cg_upnp_device_getdevicetype(childDev),
			      exacttype) == 0)
		{
			return childDev;
		}

		moreChildDev = cg_upnp_device_getdevicebyexacttype(childDev, 
								   exacttype);
		if (moreChildDev != NULL)
		{
			return moreChildDev;
		}
	}

	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/**
 * Find a device from the device's children by the UDN of the device.
 *
 * \param dev Device in question
 * \param udn Type of the device
 *
 */
CgUpnpDevice *cg_upnp_device_getdevicebyudn(CgUpnpDevice *dev, char *udn)
{
	CgUpnpDevice *childDev = NULL;
	CgUpnpDevice *moreChildDev = NULL;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(udn) <= 0 || dev == NULL)
	{
		return NULL;
	}
	
	for (childDev = cg_upnp_device_getdevices(dev); 
	     childDev != NULL;
	     childDev = cg_upnp_device_next(childDev))
	{
		if (cg_strcmp(cg_upnp_device_getudn(childDev), udn) == 0)
		{
			return childDev;
		}

		moreChildDev = cg_upnp_device_getdevicebyudn(childDev, udn);
		if (moreChildDev != NULL)
		{
			return moreChildDev;
		}
	}

	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
* cg_upnp_device_getdevicebydescriptionuri
****************************************/

CgUpnpDevice *cg_upnp_device_getdevicebydescriptionuri(CgUpnpDevice *dev, char *url)
{
	CgUpnpDevice *childDev;
	CgUpnpDevice *moreChildDev;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(url) <= 0)
		return NULL;
			
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		if (cg_upnp_device_isdescriptionuri(dev, url) == TRUE)
			return dev;
		moreChildDev = cg_upnp_device_getdevicebydescriptionuri(childDev, url);
		if (moreChildDev != NULL)
			return moreChildDev;
	}

	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
*
* Notify
*
****************************************/

/****************************************
* cg_upnp_device_notifywait
****************************************/

static void cg_upnp_device_notifywait(CgUpnpDevice *dev)
{
	cg_log_debug_l4("Entering...\n");

	cg_waitrandom(CG_UPNP_DEVICE_DEFAULT_DISCOVERY_WAIT_TIME);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_device_getlocationurl
****************************************/

char *cg_upnp_device_getlocationurl(CgUpnpDevice *dev, char *host, char *buf, int bufSize)
{
	cg_log_debug_l4("Entering...\n");

	return cg_net_gethosturl(
					host,
					cg_upnp_device_gethttpport(dev),
					cg_upnp_device_getdescriptionuri(dev),
					buf,
					bufSize);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_device_getnotifydevicent
****************************************/

char *cg_upnp_device_getnotifydevicent(CgUpnpDevice *dev, char *buf, int bufSize)
{
	cg_log_debug_l4("Entering...\n");

	if (cg_upnp_device_isrootdevice(dev) == FALSE) {
#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s", cg_upnp_device_getudn(dev));
#else
		cg_strcpy(buf, cg_upnp_device_getudn(dev));
#endif
	} else {
#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s", CG_UPNP_DEVICE_UPNP_ROOTDEVICE);
#else
		cg_strcpy(buf, CG_UPNP_DEVICE_UPNP_ROOTDEVICE);
#endif
	}
	cg_log_debug_l4("Leaving...\n");

	return buf;
}

/****************************************
* cg_upnp_device_getnotifydeviceusn
****************************************/

char *cg_upnp_device_getnotifydeviceusn(CgUpnpDevice *dev, char *buf, int bufSize)
{
	cg_log_debug_l4("Entering...\n");

	if (cg_upnp_device_isrootdevice(dev) == TRUE) {
#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s::%s", cg_upnp_device_getudn(dev), CG_UPNP_DEVICE_UPNP_ROOTDEVICE);
#else
		sprintf(buf, "%s::%s", cg_upnp_device_getudn(dev), CG_UPNP_DEVICE_UPNP_ROOTDEVICE);
#endif
	}
	else {
#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s", cg_upnp_device_getudn(dev));
#else
		sprintf(buf, "%s", cg_upnp_device_getudn(dev));
#endif
	}
	
	cg_log_debug_l4("Leaving...\n");

	return buf;
}

/****************************************
* cg_upnp_device_getnotifydevicetypent
****************************************/

char *cg_upnp_device_getnotifydevicetypent(CgUpnpDevice *dev, char *buf, int bufSize)
{
	cg_log_debug_l4("Entering...\n");

#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize, "%s", cg_upnp_device_getdevicetype(dev));
#else
	sprintf(buf, "%s", cg_upnp_device_getdevicetype(dev));
#endif

	cg_log_debug_l4("Leaving...\n");

	return buf;
}

/****************************************
* cg_upnp_device_getnotifydevicetypeusn
****************************************/

char *cg_upnp_device_getnotifydevicetypeusn(CgUpnpDevice *dev, char *buf, int bufSize)
{
	cg_log_debug_l4("Entering...\n");

#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize, "%s::%s", cg_upnp_device_getudn(dev), cg_upnp_device_getdevicetype(dev));
#else
	sprintf(buf, "%s::%s", cg_upnp_device_getudn(dev), cg_upnp_device_getdevicetype(dev));
#endif

	cg_log_debug_l4("Leaving...\n");

	return buf;
}	

/****************************************
* cg_upnp_device_announcefrom
****************************************/

BOOL cg_upnp_device_announcefrom(CgUpnpDevice *dev, char *bindAddr)
{
	char ssdpLineBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	CgUpnpServiceList *serviceList;
	CgUpnpService *service;
	CgUpnpDeviceList *devList;
	CgUpnpDevice *childDev;
	CgUpnpSSDPRequest *ssdpReq;
	CgUpnpSSDPSocket *ssdpSock;
	BOOL sentResult;
	
	cg_log_debug_l4("Entering...\n");
	cg_log_debug_s("Announcing from %s\n", bindAddr);
	
	ssdpSock = cg_upnp_ssdp_socket_new();
	ssdpReq = cg_upnp_ssdprequest_new();
	
	cg_upnp_ssdprequest_setserver(ssdpReq, cg_upnp_getservername(ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setleasetime(ssdpReq, cg_upnp_device_getleasetime(dev));
	cg_upnp_ssdprequest_setlocation(ssdpReq, cg_upnp_device_getlocationurl(dev, bindAddr, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setnts(ssdpReq, CG_UPNP_SSDP_NTS_ALIVE);
	
	/**** uuid:device-UUID(::upnp:rootdevice) ****/
	if (cg_upnp_device_isrootdevice(dev) == TRUE) {
		cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_device_getnotifydevicent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
		cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_device_getnotifydeviceusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
		cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
		cg_wait(20);
	}

	/**** uuid:device-UUID::urn:schemas-upnp-org:device:deviceType:v ****/
	cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_device_getnotifydevicetypent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_device_getnotifydevicetypeusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	cg_wait(20);

	/**** root or embedded device UUID ****/
	cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_device_getudn(dev));
	cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_device_getudn(dev));
	sentResult = cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	cg_wait(20);
	
	cg_upnp_ssdprequest_delete(ssdpReq);
	cg_upnp_ssdp_socket_close(ssdpSock);
	cg_upnp_ssdp_socket_delete(ssdpSock);

	/**** child services ****/
	serviceList = cg_upnp_device_getservicelist(dev);
	for (service=cg_upnp_servicelist_gets(serviceList); service != NULL; service = cg_upnp_service_next(service))
		cg_upnp_service_announcefrom(service, bindAddr);
	
	/**** child deveices ****/
	devList = cg_upnp_device_getdevicelist(dev);
	for (childDev = cg_upnp_devicelist_gets(devList); childDev != NULL; childDev = cg_upnp_device_next(childDev))
	{
		cg_upnp_device_announcefrom(childDev, bindAddr);
	}
		
	cg_log_debug_l4("Leaving...\n");

	return sentResult;
}

/****************************************
* cg_upnp_device_announce
****************************************/

void cg_upnp_device_announce(CgUpnpDevice *dev)
{
	CgNetworkInterfaceList *netIfList;
	CgNetworkInterface *netIf;
	char *bindAddr;
	int ssdpCount, i;
		
	cg_log_debug_l4("Entering...\n");

	cg_upnp_device_notifywait(dev);
	
	ssdpCount = cg_upnp_ssdp_getannouncecount();
	
	netIfList = cg_net_interfacelist_new();
	cg_net_gethostinterfaces(netIfList);
	for (netIf = cg_net_interfacelist_gets(netIfList); netIf; netIf = cg_net_interface_next(netIf)) {
		bindAddr = cg_net_interface_getaddress(netIf);
		if (cg_strlen(bindAddr) <= 0)
			continue;
		for (i=0; i<ssdpCount; i++)
		{
			cg_upnp_device_announcefrom(dev, bindAddr);
		}
	}

	cg_net_interfacelist_delete(netIfList);

	cg_log_debug_l4("Leaving...\n");
}
	
/****************************************
* cg_upnp_device_byebyefrom
****************************************/

BOOL cg_upnp_device_byebyefrom(CgUpnpDevice *dev, char *bindAddr)
{
	char ssdpLineBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	CgUpnpServiceList *serviceList;
	CgUpnpService *service;
	CgUpnpDeviceList *devList;
	CgUpnpDevice *childDev;
	CgUpnpSSDPRequest *ssdpReq;
	CgUpnpSSDPSocket *ssdpSock;
	BOOL sentResult;

	cg_log_debug_l4("Entering...\n");

	ssdpSock = cg_upnp_ssdp_socket_new();
	ssdpReq = cg_upnp_ssdprequest_new();
	
	cg_upnp_ssdprequest_setnts(ssdpReq, CG_UPNP_SSDP_NTS_BYEBYE);

	/**** uuid:device-UUID(::upnp:rootdevice) ****/
	if (cg_upnp_device_isrootdevice(dev) == TRUE) {
		cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_device_getnotifydevicent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
		cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_device_getnotifydeviceusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
		cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	}

	/**** uuid:device-UUID::urn:schemas-upnp-org:device:deviceType:v ****/
	cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_device_getnotifydevicetypent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_device_getnotifydevicetypeusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);

	/**** root or embedded device UUID ****/
	cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_device_getudn(dev));
	cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_device_getudn(dev));
	sentResult = cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);

	cg_upnp_ssdprequest_delete(ssdpReq);
	cg_upnp_ssdp_socket_close(ssdpSock);
	cg_upnp_ssdp_socket_delete(ssdpSock);

	serviceList = cg_upnp_device_getservicelist(dev);
	for (service=cg_upnp_servicelist_gets(serviceList); service != NULL; service = cg_upnp_service_next(service))
		cg_upnp_service_byebyefrom(service, bindAddr);
	
	devList = cg_upnp_device_getdevicelist(dev);
	for (childDev = cg_upnp_devicelist_gets(devList); childDev != NULL; childDev = cg_upnp_device_next(childDev))
		cg_upnp_device_byebyefrom(childDev, bindAddr);

	cg_log_debug_l4("Leaving...\n");

	return sentResult;
}

/****************************************
* cg_upnp_device_byebye
****************************************/

void cg_upnp_device_byebye(CgUpnpDevice *dev)
{
	CgNetworkInterfaceList *netIfList;
	CgNetworkInterface *netIf;
	char *bindAddr;
	int ssdpCount, i;
		
	cg_log_debug_l4("Entering...\n");

	cg_upnp_device_notifywait(dev);
	
	ssdpCount = cg_upnp_ssdp_getannouncecount();
	
	netIfList = cg_net_interfacelist_new();
	cg_net_gethostinterfaces(netIfList);
	for (netIf = cg_net_interfacelist_gets(netIfList); netIf; netIf = cg_net_interface_next(netIf)) {
		bindAddr = cg_net_interface_getaddress(netIf);
		if (cg_strlen(bindAddr) <= 0)
			continue;
		for (i=0; i<ssdpCount; i++)
			cg_upnp_device_byebyefrom(dev, bindAddr);
	}

	cg_net_interfacelist_delete(netIfList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_device_postsearchresponse
****************************************/

BOOL cg_upnp_device_postsearchresponse(CgUpnpDevice *dev, CgUpnpSSDPPacket *ssdpPkt, char *st, char *usn)
{
	CgUpnpDevice *rootDev;
	CgUpnpSSDPResponse *ssdpRes;
	char httpDateStr[CG_HTTP_DATE_MAXLEN];
	char *localAddr;
	char *remoteAddr;
	int remotePort;
	char rootDevLocation[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	char serverBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	int ssdpCount;
	CgUpnpSSDPSocket *ssdpSock;
	int n;
	
	cg_log_debug_l4("Entering...\n");

	localAddr = cg_upnp_ssdp_packet_getlocaladdress(ssdpPkt);
	cg_log_debug_s("Local address: <%s>\n", localAddr);
	rootDev = cg_upnp_device_getrootdevice(dev);

	cg_upnp_device_getlocationurl(rootDev, localAddr, rootDevLocation, sizeof(rootDevLocation));

	ssdpRes = cg_upnp_ssdpresponse_new();
	cg_upnp_ssdpresponse_setleasetime(ssdpRes, cg_upnp_device_getleasetime(dev));
	cg_upnp_ssdpresponse_setdate(ssdpRes, cg_http_getdate(cg_getcurrentsystemtime(), httpDateStr, sizeof(httpDateStr)));
	cg_upnp_ssdpresponse_setst(ssdpRes, st);

	cg_upnp_ssdpresponse_setext(ssdpRes);
	cg_upnp_getservername(serverBuf, CG_UPNP_SSDP_HEADER_LINE_MAXSIZE);
	cg_upnp_ssdpresponse_setserver(ssdpRes, serverBuf);

	cg_upnp_ssdpresponse_setusn(ssdpRes, usn);
	cg_upnp_ssdpresponse_setlocation(ssdpRes, rootDevLocation);

	remoteAddr = cg_upnp_ssdp_packet_getremoteaddress(ssdpPkt);
	remotePort = cg_upnp_ssdp_packet_getremoteport(ssdpPkt);
	cg_log_debug_s("Remote address: <%s>\n", remoteAddr);
	ssdpCount = cg_upnp_ssdp_getannouncecount();

	ssdpSock = cg_upnp_ssdp_socket_new();

	for (n=0; n<ssdpCount; n++) {
		cg_waitrandom(20);
		cg_upnp_ssdp_socket_postresponse(ssdpSock, ssdpRes, remoteAddr, remotePort);
	}
	
	cg_upnp_ssdp_socket_delete(ssdpSock);

	cg_upnp_ssdpresponse_delete(ssdpRes);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
*
* Start/Stop
*
****************************************/

/****************************************
* cg_upnp_device_start
****************************************/

BOOL cg_upnp_device_start(CgUpnpDevice *dev)
{
	CG_HTTP_LISTENER httpListener;
	int httpPort;
	
	cg_log_debug_l4("Entering...\n");

	cg_upnp_device_stop(dev);

	/* Create interface cache, if it does not exist and cache current */
	if (dev->ifCache == NULL)
		dev->ifCache = cg_net_interfacelist_new();
	cg_net_gethostinterfaces(dev->ifCache);
	
	/**** HTTP Server ****/
	httpPort = cg_upnp_device_gethttpport(dev);
	/* Opening HTTP server may fail, so try many ports */
	while(cg_http_serverlist_open(dev->httpServerList, httpPort) == FALSE)
	{
		cg_upnp_device_sethttpport(dev, httpPort + 1);
		httpPort = cg_upnp_device_gethttpport(dev);
	}
	cg_http_serverlist_setuserdata(dev->httpServerList, dev);
	httpListener = cg_upnp_device_gethttplistener(dev);
	if (httpListener == NULL)
		httpListener = cg_upnp_device_httprequestrecieved;
	cg_http_serverlist_setlistener(dev->httpServerList, httpListener);
	cg_http_serverlist_start(dev->httpServerList);

	/**** SSDP Server ****/
	if (cg_upnp_ssdp_serverlist_open(dev->ssdpServerList) == FALSE)
		return FALSE;
	cg_upnp_ssdp_serverlist_setuserdata(dev->ssdpServerList, dev);
	cg_upnp_ssdp_serverlist_setlistener(dev->ssdpServerList, cg_upnp_device_ssdplistener);
	cg_upnp_ssdp_serverlist_start(dev->ssdpServerList);

	/**** Announce ****/
	cg_upnp_device_announce(dev);
	
	/**** Advertiser ****/
	cg_upnp_device_advertiser_start(dev);	

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}
	
/****************************************
* cg_upnp_device_ipchanged
****************************************/

BOOL cg_upnp_device_ipchanged(CgUpnpDevice *dev)
{
	cg_log_debug_l4("Entering...\n");

	cg_log_debug_l4("Leaving...\n");

	return FALSE;
}

/****************************************
* cg_upnp_device_stop
****************************************/

BOOL cg_upnp_device_stop(CgUpnpDevice *dev)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_device_byebye(dev);
	
	/**** Advertiser ****/
	if (cg_upnp_device_advertiser_isrunning(dev))
		cg_upnp_device_advertiser_stop(dev);	

	/**** HTTP Server ****/
	if (0 < cg_http_headerlist_size(dev->httpServerList)) {
		cg_http_serverlist_stop(dev->httpServerList);
		cg_http_serverlist_close(dev->httpServerList);
		cg_http_serverlist_clear(dev->httpServerList);
	}
	
	/**** SSDP Server ****/
	if (0 < cg_upnp_ssdp_serverlist_size(dev->ssdpServerList)) {
		cg_upnp_ssdp_serverlist_stop(dev->ssdpServerList);
		cg_upnp_ssdp_serverlist_close(dev->ssdpServerList);
		cg_upnp_ssdp_serverlist_clear(dev->ssdpServerList);
	}
	
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
 * cg_upnp_device_isrunning
 ****************************************/

BOOL cg_upnp_device_isrunning(CgUpnpDevice *dev)
{
	return cg_upnp_device_advertiser_isrunning(dev);
}

/****************************************
*
* Embedded Service
*
****************************************/

/****************************************
* cg_upnp_device_initservicelist
****************************************/

static void cg_upnp_device_initservicelist(CgUpnpDevice *dev)
{
	CgXmlNode *devNode;
	CgXmlNode *serviceListNode;
	CgXmlNode *childNode;
	CgUpnpService *childService;
	
	cg_log_debug_l4("Entering...\n");

	cg_upnp_servicelist_clear(dev->serviceList);
	
	devNode = cg_upnp_device_getdevicenode(dev);
	if (devNode == NULL)
		return;
		
	serviceListNode = cg_xml_node_getchildnode(devNode, CG_UPNP_SERVICELIST_ELEM_NAME);
	if (serviceListNode == NULL)
		return;
		
	for (childNode = cg_xml_node_getchildnodes(serviceListNode); childNode != NULL; childNode = cg_xml_node_next(childNode)) {
		if (cg_upnp_service_isservicenode(childNode) == FALSE)
			continue;
		childService = cg_upnp_service_new();
		cg_upnp_service_setservicenode(childService, childNode);
		cg_upnp_servicelist_add(dev->serviceList, childService);
		cg_upnp_service_setdevice(childService, dev);
	} 

	cg_log_debug_l4("Leaving...\n");
}


CgUpnpService *cg_upnp_device_getservicebyserviceid(CgUpnpDevice *dev,
						    char *serviceId)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(serviceId) <= 0 || dev == NULL)
	{
		return NULL;
	}

	for (service = cg_upnp_device_getservices(dev);
	     service != NULL;
	     service = cg_upnp_service_next(service))
	{
		if (cg_strcmp(cg_upnp_service_getserviceid(service),
			      serviceId) == 0)
		{
#ifdef CG_OPTIMIZED_CP_MODE
			if (cg_upnp_service_isparsed(service) == FALSE)
				cg_upnp_controlpoint_parsescservicescpd(service);
#endif
			return service;
		}
	}
		
	for (childDev = cg_upnp_device_getdevices(dev);
	     childDev != NULL;
	     childDev = cg_upnp_device_next(childDev))
	{
		service = cg_upnp_device_getservicebyserviceid(childDev, serviceId);
		if (service != NULL)
		{
			return service;
		}
	}
	
	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/**
 * Find a service from the device by the type of the service.
 * This function searches for services, whose *complete type string*
 * matches the given string, including version number. For example:
 * "urn:schemas-upnp-org:service:ContentDirectory:1". If you need to
 * know the version of a service, use \ref cg_upnp_servicetype_getversion
 *
 * \param dev Device in question
 * \param type Type of the service
 * 
 */
CgUpnpService* cg_upnp_device_getservicebyexacttype(CgUpnpDevice* dev,
						    char* type)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(type) <= 0 || dev == NULL)
	{
		return NULL;
	}

	for (service = cg_upnp_device_getservices(dev);
	     service != NULL;
	     service = cg_upnp_service_next(service))
	{
		if (cg_strcmp(cg_upnp_service_getservicetype(service),
			      type) == 0)
		{
#ifdef CG_OPTIMIZED_CP_MODE
		if (cg_upnp_service_isparsed(service) == FALSE)
			cg_upnp_controlpoint_parsescservicescpd(service);
#endif
		return service;
		}
	}
		
	for (childDev = cg_upnp_device_getdevices(dev);
	     childDev != NULL;
	     childDev = cg_upnp_device_next(childDev))
	{
		service = cg_upnp_device_getservicebyexacttype(childDev, type);
		if (service != NULL)
		{
			return service;
		}
	}
	
	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/**
 * Find a service from the device by the type of the service.
 * This function searches for services, whose *type part* (i.e. not including
 * the version) of the service type string matches the given string.
 * For example: "urn:schemas-upnp-org:service:ContentDirectory". If you need
 * to know the version of a service, use \ref cg_upnp_servicetype_getversion
 *
 * \param dev Device in question
 * \param type Type of the service
 *
 */
CgUpnpService *cg_upnp_device_getservicebytype(CgUpnpDevice *dev, char *type)
{
	CgUpnpService *service = NULL;
	CgUpnpDevice *childDev = NULL;
	char* typeString = NULL;
	char* part = NULL;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(type) <= 0)
	{
		return NULL;
	}

	for (service = cg_upnp_device_getservices(dev);
	     service != NULL;
	     service = cg_upnp_service_next(service))
	{
		typeString = cg_upnp_service_getservicetype(service);
		if (typeString != NULL)
		{
			if (cg_strcmp(typeString, type) == 0)
				return service;
			part = cg_upnp_servicetype_getschematype(typeString);
			if (cg_strcmp(part, type) == 0)
			{
				free(part);
#ifdef CG_OPTIMIZED_CP_MODE
				if (cg_upnp_service_isparsed(service) == FALSE)
					cg_upnp_controlpoint_parsescservicescpd(service);
#endif
				return service;
			}
			else
			{
				free(part);
			}
		}
	}

	for (childDev = cg_upnp_device_getdevices(dev); 
	     childDev != NULL;
	     childDev = cg_upnp_device_next(childDev))
	{
		service = cg_upnp_device_getservicebytype(childDev, type);
		if (service != NULL)
		{
			return service;
		}
	}

	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
* cg_upnp_device_getservicebyscpdurl
****************************************/

CgUpnpService *cg_upnp_device_getservicebyscpdurl(CgUpnpDevice *dev, char *url)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(url) <= 0)
		return NULL;
			
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		cg_log_debug_s("Child node v: %s\n", cg_xml_node_getchildnodevalue(cg_upnp_service_getservicenode(service), CG_UPNP_SERVICE_SCPDURL));
		if (cg_upnp_service_isscpdurl(service, url) == TRUE)
			return service;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		service = cg_upnp_device_getservicebyscpdurl(childDev, url);
		if (service != NULL)
			return service;
	}
	
	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
* cg_upnp_device_getservicebycontrolurl
****************************************/

CgUpnpService *cg_upnp_device_getservicebycontrolurl(CgUpnpDevice *dev, char *url)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	CgNetURL* service_url;

	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(url) <= 0)
		return NULL;
			
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		/* cg_log_debug_s("<%s> == <%s> ?\n", url, cg_net_url_getrequest(cg_upnp_service_getcontrolurl(service))); */
		/* MODIFICATION Fabrice Fontaine Orange 23/04/07
		if (cg_strstr(cg_net_url_getrequest(cg_upnp_service_getcontrolurl(service)), url) != -1)*/
		/* Memory leak correction : cg_upnp_service_getcontrolurl return a malloc */
		/* structure, this structure must be freed after use */
		service_url=cg_upnp_service_getcontrolurl(service);
		if (service_url) {
			if (cg_strstr(cg_net_url_getrequest(service_url), url) != -1)
			{
				cg_net_url_delete(service_url);
			/* MODIFICATION END Fabrice Fontaine Orange 23/04/07 */
				return service;
			/* ADD Fabrice Fontaine Orange 23/04/07 */
			}
			cg_net_url_delete(service_url);
		}
		/* ADD END Fabrice Fontaine Orange 23/04/07 */	
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		service = cg_upnp_device_getservicebycontrolurl(childDev, url);
		if (service != NULL)
			return service;
	}
	
	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
* cg_upnp_device_getservicebysid
****************************************/

CgUpnpService *cg_upnp_device_getservicebysid(CgUpnpDevice *dev, char *sid)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(sid) <= 0)
		return NULL;
			
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_upnp_service_getsubscriberbysid(service, sid) != NULL)
		{
			return service;
		} else if (cg_upnp_service_issubscribed(service) == TRUE &&
			   cg_strcmp(cg_upnp_service_getsubscriptionsid(service), sid) == 0)
		{
			return service;
		}
			
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		service = cg_upnp_device_getservicebysid(childDev, sid);
		if (service != NULL)
			return service;
	}
	
	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
* cg_upnp_device_setactionlistener
****************************************/

void cg_upnp_device_setactionlistener(CgUpnpDevice *dev, CG_UPNP_ACTION_LISTNER actionListner)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	cg_log_debug_l4("Entering...\n");

	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service))
		cg_upnp_service_setactionlistener(service, actionListner);
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev))
		cg_upnp_device_setactionlistener(childDev, actionListner);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_device_setquerylistener
****************************************/

void cg_upnp_device_setquerylistener(CgUpnpDevice *dev, CG_UPNP_STATEVARIABLE_LISTNER queryListner)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	cg_log_debug_l4("Entering...\n");

	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service))
		cg_upnp_service_setquerylistener(service, queryListner);
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev))
		cg_upnp_device_setquerylistener(childDev, queryListner);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_device_getservicebyeventsuburl
****************************************/

CgUpnpService *cg_upnp_device_getservicebyeventsuburl(CgUpnpDevice *dev, char *url)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(url) <= 0)
		return NULL;
			
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_streq(cg_net_url_getpath(cg_upnp_service_geteventsuburl(service)), url) == TRUE)
			return service;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		service = cg_upnp_device_getservicebyeventsuburl(childDev, url);
		if (service != NULL)
			return service;
	}
	
	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
 * cg_upnp_device_getsmallesticonbymimetype
 ****************************************/

CgUpnpIcon *cg_upnp_device_getsmallesticonbymimetype(CgUpnpDevice *dev, char *mimeType)
{
	CgUpnpIcon *icon;
	CgUpnpIcon *smallestIcon;
	
	smallestIcon = NULL;
	for (icon = cg_upnp_device_geticons(dev); icon; icon = cg_upnp_icon_next(icon)) {
		if (0 < cg_strlen(mimeType)) {
			if (!cg_streq(cg_upnp_icon_getmimetype(icon), mimeType))
				continue;
		}
		if (!smallestIcon) {
			smallestIcon = icon;
			continue;
		}
		if (cg_upnp_icon_getwidth(icon) < cg_upnp_icon_getwidth(smallestIcon))
			smallestIcon = icon;			
	}
	
	return smallestIcon;
}

/****************************************
* cg_upnp_device_getsmallesticon
****************************************/
				
CgUpnpIcon *cg_upnp_device_getsmallesticon(CgUpnpDevice *dev)
{
	return cg_upnp_device_getsmallesticonbymimetype(dev, "");
}
				
/****************************************
 * cg_upnp_device_getabsoluteiconurl
 ****************************************/

BOOL cg_upnp_device_getabsoluteiconurl(CgUpnpDevice *dev, CgUpnpIcon *icon, CgString *buf)
{
	CgNetURI *uri;
	CgNetURI *ssdpUri;
	CgUpnpDevice *rootDev;
	char *ssdplocation;
	
	uri = cg_net_uri_new();

	cg_net_uri_set(uri, cg_upnp_icon_geturl(icon));
	if (cg_net_uri_isabsolute(uri)) {
		cg_string_setvalue(buf, cg_net_uri_geturi(uri));
		cg_net_uri_delete(uri);
		return TRUE;
	}
	
	rootDev = cg_upnp_device_getrootdevice(dev);
	if (rootDev) {
		ssdplocation = cg_upnp_device_getlocationfromssdppacket(rootDev);
		ssdpUri = cg_net_uri_new();
		if (0 < cg_strlen(ssdplocation)) {
			cg_net_uri_set(uri, ssdplocation);
			cg_net_uri_setpath(uri, cg_upnp_icon_geturl(icon));
			cg_string_setvalue(buf, cg_net_uri_getvalue(uri));
			cg_net_uri_delete(uri);
			return TRUE;
		}
		cg_net_uri_delete(ssdpUri);
	}
	
	cg_net_uri_delete(uri);

	return FALSE;
}

/****************************************
*
* Embedded Action
*
****************************************/

/****************************************
* cg_upnp_device_getactionbyname
****************************************/

CgUpnpAction *cg_upnp_device_getactionbyname(CgUpnpDevice *dev, char *name)
{
	CgUpnpService *service;
	CgUpnpAction *action;
	CgUpnpDevice *childDev;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(name) <= 0)
		return NULL;
			
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		action = cg_upnp_service_getactionbyname(service, name);
		if (action != NULL)
			return action;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		action = cg_upnp_device_getactionbyname(childDev, name);
		if (action != NULL)
			return action;
	}
	
	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
*
* Embedded StateVariable
*
****************************************/

/****************************************
* cg_upnp_device_getstatevariablebyname
****************************************/

CgUpnpStateVariable *cg_upnp_device_getstatevariablebyname(CgUpnpDevice *dev, char *name)
{
	CgUpnpService *service;
	CgUpnpStateVariable *statVar;
	CgUpnpDevice *childDev;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(name) <= 0)
		return NULL;
			
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		statVar = cg_upnp_service_getstatevariablebyname(service, name);
		if (statVar != NULL)
			return statVar;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		statVar = cg_upnp_device_getstatevariablebyname(childDev, name);
		if (statVar != NULL)
			return statVar;
	}
	
	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
*
* Embedded Icon
*
****************************************/

/****************************************
* cg_upnp_device_initiconlist
****************************************/

static void cg_upnp_device_initiconlist(CgUpnpDevice *dev)
{
	CgXmlNode *devNode;
	CgXmlNode *iconListNode;
	CgXmlNode *childNode;
	CgUpnpIcon *childIcon;
	
	cg_log_debug_l4("Entering...\n");

	cg_upnp_iconlist_clear(dev->iconList);
	
	devNode = cg_upnp_device_getdevicenode(dev);
	if (devNode == NULL)
		return;
		
	iconListNode = cg_xml_node_getchildnode(devNode, CG_UPNP_ICONLIST_ELEM_NAME);
	if (iconListNode == NULL)
		return;
		
	for (childNode = cg_xml_node_getchildnodes(iconListNode); childNode != NULL; childNode = cg_xml_node_next(childNode)) {
		if (cg_upnp_icon_isiconnode(childNode) == FALSE)
			continue;
		childIcon = cg_upnp_icon_new();
		cg_upnp_icon_seticonnode(childIcon, childNode);
		cg_upnp_iconlist_add(dev->iconList, childIcon);
	} 

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
 * cg_upnp_device_addicon
 ****************************************/

BOOL cg_upnp_device_addicon(CgUpnpDevice *dev, CgUpnpIcon *icon)
{
	CgXmlNode *devNode;
	CgXmlNode *iconListNode;
	CgXmlNode *iconNode;
	CgXmlNode *copyIconNode;
	CgUpnpIcon *copyIcon;

	iconNode = cg_upnp_icon_geticonnode(icon);
	if (iconNode == NULL)
		return FALSE;
	
	devNode = cg_upnp_device_getdevicenode(dev);
	if (devNode == NULL)
		return FALSE;

	iconListNode = cg_xml_node_getchildnode(devNode, CG_UPNP_ICONLIST_ELEM_NAME);
	if (iconListNode == NULL) {
		iconListNode = cg_xml_node_new();
		cg_xml_node_setname(iconListNode, CG_UPNP_ICONLIST_ELEM_NAME);
		cg_xml_node_addchildnode(devNode, iconListNode);
	}
	
	copyIconNode = cg_xml_node_new();
	cg_xml_node_copy(copyIconNode, iconNode);
	cg_xml_node_addchildnode(iconListNode, copyIconNode);
	
	copyIcon = cg_upnp_icon_new();
	cg_upnp_icon_seticonnode(copyIcon, copyIconNode);
	cg_upnp_iconlist_add(dev->iconList, copyIcon);
	
	return TRUE;
}

/****************************************
 * cg_upnp_device_updateudn
 ****************************************/

void cg_upnp_device_updateudn(CgUpnpDevice *dev)
{
	char uuid[CG_UPNP_UUID_MAX_LEN];
	cg_upnp_createuuid(uuid, sizeof(uuid));
	cg_upnp_device_setudn(dev, uuid);
}

