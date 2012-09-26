/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006-2007 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cservice.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*	08/16/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Changed cg_upnp_service_parsedescriptionurl() to set "/" as the default path when the url has no path.
*
*	09-Jan-06 Heikki Junnila
*		- Added service type parsing functions:
*		  cg_upnp_servicetype_getidentifier
*		  cg_upnp_servicetype_geturn
*		  cg_upnp_servicetype_getservice
*		  cg_upnp_servicetype_gettype
*		  cg_upnp_servicetype_getversion
*
*	10-Jan-06 Heikki Junnila
*		- Added cg_upnp_servicetype_getschematype() and fixed
*		  existing servicetype functions to accept 1-character strings
*
*	11-Jan-06 Heikki Junnila
*		- Removed cg_upnp_service_isname() and _getname() because
*		  according to UPnP specs, services have only types, not names
*
*	22-Oct-07 Aapo Makela
*		- Fixed memory leak (free location_str in cg_upnp_service_mangleurl())
*
 *	05-Jan-08  Satoshi Konno <skonno@cybergarage.org>
 *		- Fixed cg_upnp_service_mangleurl() to return correct url using cg_net_uri_rebuild() when a device has URLBase and the service's URL is relative.
 ******************************************************************/

#include <cybergarage/upnp/cservice.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/upnp/cupnp.h>
#include <cybergarage/upnp/cupnp_function.h>
#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/upnp/ssdp/cssdp.h>
#include <cybergarage/util/clog.h>
#include <cybergarage/util/cstring.h>
#include <cybergarage/net/curl.h>
#include <cybergarage/net/curi.h>

#ifdef CG_OPTIMIZED_CP_MODE
#include <cybergarage/upnp/ccontrolpoint.h>
#endif

/****************************************
* prototype define for static functions
****************************************/

static void cg_upnp_service_initchildnodes(CgUpnpService *service);
static void cg_upnp_service_initactionlist(CgUpnpService *service);
static void cg_upnp_service_initservicestatetable(CgUpnpService *service);
static CgNetURL *cg_upnp_service_mangleurl(CgUpnpService *service, char *type);

/****************************************
* cg_upnp_service_new
****************************************/

CgUpnpService *cg_upnp_service_new()
{
	CgUpnpService *service;

	cg_log_debug_l4("Entering...\n");

	service = (CgUpnpService *)malloc(sizeof(CgUpnpService));

	if ( NULL != service )
	{
		cg_list_node_init((CgList *)service);

		service->scpdNodeList = NULL;
		service->serviceNode = NULL;

		service->actionList = cg_upnp_actionlist_new();
		service->serviceStateTable = cg_upnp_servicestatetable_new();

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
		service->subscriberList = cg_upnp_subscriberlist_new();
#endif
	
#ifdef CG_OPTIMIZED_CP_MODE
		service->parsed = FALSE;
#endif

		service->mutex = cg_mutex_new();
		service->subscriptionSid = cg_string_new();
		
		cg_upnp_service_setdevice(service, NULL);
		cg_upnp_service_setsubscriptiontimeout(service, 0);
		cg_upnp_service_setsubscriptiontimestamp(service, 0);
		
		cg_upnp_service_setuserdata(service, NULL);
	}	
	
	cg_log_debug_l4("Leaving...\n");
	
	return service;
}

/****************************************
* cg_upnp_service_delete
****************************************/

void cg_upnp_service_delete(CgUpnpService *service)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)service);
	
	cg_upnp_service_clear(service);

	cg_upnp_actionlist_delete(service->actionList);
	cg_upnp_servicestatetable_delete(service->serviceStateTable);

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	cg_upnp_subscriberlist_delete(service->subscriberList);
#endif
	
	cg_mutex_delete(service->mutex);
	cg_string_delete(service->subscriptionSid);
	
	free(service);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_service_clear
****************************************/

void cg_upnp_service_clear(CgUpnpService *service)
{
	cg_log_debug_l4("Entering...\n");

	/* Don't clear the service node */
	/* service->serviceNode = NULL; */
	
	if (service->scpdNodeList != NULL) {
		cg_xml_nodelist_delete(service->scpdNodeList);
		service->scpdNodeList = NULL;
	}

	cg_upnp_actionlist_clear(service->actionList);
	cg_upnp_servicestatetable_clear(service->serviceStateTable);
#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	cg_upnp_subscriberlist_clear(service->subscriberList);
#endif
	
	cg_upnp_service_setsubscriptionsid(service, NULL);
	cg_upnp_service_setsubscriptiontimeout(service, 0);

	cg_log_debug_l4("Leaving...\n");
}	

/**
 * Compare the service's SCPD URL and the given location
 * 
 * @param service The service in question
 * @param url The URL (location) to compare
 * @return TRUE if location is found from URL; otherwise FALSE
 */
BOOL cg_upnp_service_isscpdurl(CgUpnpService *service, char *url)
{
	CgXmlNode *s = cg_upnp_service_getservicenode(service);
	char      *v = cg_xml_node_getchildnodevalue(s, CG_UPNP_SERVICE_SCPDURL);
	return cg_net_uri_isequivalent(v, url);
}

/**
 * Get the service's event subscription URL
 *
 * @param service The service in question
 * @return char*
 */
CgNetURL *cg_upnp_service_geteventsuburl(CgUpnpService *service) 
{
	return cg_upnp_service_mangleurl(service, CG_UPNP_SERVICE_EVENT_SUB_URL);
}

/**
 * Get the service's control URL
 * @param service The service in question
 * @return char*
 */
CgNetURL *cg_upnp_service_getcontrolurl(CgUpnpService *service)
{ 
	return cg_upnp_service_mangleurl(service, CG_UPNP_SERVICE_CONTROL_URL);
}

/**
 * Get the service's SCPD (service description) URL
 *
 * @param service The service in question                                                                 
 * @return CgNetURL Pointer to URL/URI structure
 */
CgNetURL *cg_upnp_service_getscpdurl(CgUpnpService *service)
{
	return cg_upnp_service_mangleurl(service, CG_UPNP_SERVICE_SCPDURL);
}



/****************************************
* cg_upnp_service_parsedescription
****************************************/

BOOL cg_upnp_service_parsedescription(CgUpnpService *service, char *desciption, int descriptionLen)
{
	CgXmlParser *xmlParser;
	BOOL xmlParseSuccess;
	CgXmlNode *scpdNode;
	
	cg_log_debug_l4("Entering...\n");

	cg_upnp_service_lock(service);
	cg_upnp_service_clear(service);

#ifdef CG_OPTIMIZED_CP_MODE
	service->parsed = FALSE;
#endif	
	service->scpdNodeList = cg_xml_nodelist_new();

	xmlParser = cg_xml_parser_new();
	xmlParseSuccess = cg_xml_parse(xmlParser, service->scpdNodeList, desciption, descriptionLen);
	cg_xml_parser_delete(xmlParser);
	if (xmlParseSuccess == FALSE)
	{
		cg_upnp_service_unlock(service);
		return FALSE;
	}
	
	if (cg_xml_nodelist_size(service->scpdNodeList) <= 0) {
		cg_upnp_service_clear(service);
		cg_upnp_service_unlock(service);
		return FALSE;
	}

	scpdNode = cg_upnp_service_getscpdnode(service);
	if (scpdNode == NULL) {
		cg_upnp_service_clear(service);
		cg_upnp_service_unlock(service);
		return FALSE;
	}
		
	cg_upnp_service_initchildnodes(service);

#ifdef CG_OPTIMIZED_CP_MODE
	service->parsed = TRUE;
#endif
	cg_upnp_service_unlock(service);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_upnp_service_parsedescriptionurl
****************************************/

BOOL cg_upnp_service_parsedescriptionurl(CgUpnpService *service, CgNetURL *url)
{
	char *host;
	int port;
	char *request;
	CgHttpRequest *httpReq;
	CgHttpResponse *httpRes;
	char *content;
	long contentLen;
	BOOL parseSuccess;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_net_url_ishttpprotocol(url) == FALSE)
		return FALSE;
		
	host = cg_net_url_gethost(url);
	port = cg_net_url_getport(url);
	if (port <= 0)
		port = CG_HTTP_DEFAULT_PORT;
	/**** Thanks for Theo Beisch (08/16/05) ****/
	request = cg_net_url_getrequest(url);
	if (cg_strlen(request) <= 0)
		request = "/";

	httpReq = cg_http_request_new();
	cg_http_request_setmethod(httpReq, CG_HTTP_GET);
	cg_http_request_seturi(httpReq, request);
	cg_http_request_setcontentlength(httpReq, 0);
	httpRes = cg_http_request_post(httpReq, host, port);
	
	/*statusCode = cg_http_response_getstatuscode(httpRes);
          if (statusCode != CG_HTTP_STATUS_OK) {*/
        if ( !cg_http_response_issuccessful(httpRes) ) {
		cg_http_request_delete(httpReq);
		return FALSE;
	}
	
	content = cg_http_response_getcontent(httpRes);
	contentLen = cg_http_response_getcontentlength(httpRes);

	parseSuccess = cg_upnp_service_parsedescription(service, content, contentLen);

	cg_http_request_delete(httpReq);
	
	cg_log_debug_l4("Leaving...\n");
	
	return parseSuccess;
}

/****************************************
* cg_upnp_service_getdescription
****************************************/

char *cg_upnp_service_getdescription(CgUpnpService *service, CgString *descStr)
{
	CgXmlNode *scpdNode;
	
	cg_log_debug_l4("Entering...\n");

	scpdNode = cg_upnp_service_getscpdnode(service);
	if (scpdNode != NULL) {
		cg_string_addvalue(descStr, CG_UPNP_XML_DECLARATION);
		cg_string_addvalue(descStr, "\n");
		cg_xml_node_tostring(scpdNode, TRUE, descStr);
	}
	
	cg_log_debug_l4("Leaving...\n");
	
	return cg_string_getvalue(descStr);
}

/****************************************
 * Service Type
 ****************************************/

/** 
 * Get the identifier-part of a service type string (usually "urn") 
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref cg_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* cg_upnp_servicetype_getidentifier(char* serviceType)
{
	char* part = NULL;
	int tail = 0;
	int len = 0;
	
	cg_log_debug_l4("Entering...\n");

	if (serviceType == NULL)
	{
		return NULL;
	}
	
	len = cg_strlen(serviceType);
	
	/* Find the next delimiter ':' character */
        for (tail = 0; tail < len; tail++)
        {
                if (serviceType[tail] == ':')
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
			cg_strncpy(part, serviceType, tail + 1);
			part[tail + 1] = '\0';
		}
		else
			cg_log_debug("Memory allocation problem!\n");
        }
        else
        {
		part = NULL;
        }

	cg_log_debug_l4("Leaving...\n");
	
	return part;
}

/** 
 * Get the URN part of a service type string (usually "schemas-upnp-org") 
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref cg_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* cg_upnp_servicetype_geturn(char* serviceType)
{
	char* part = NULL;
	int tail = 0;
	int head = 0;
	int len = 0;
	
	cg_log_debug_l4("Entering...\n");

	if (serviceType == NULL)
	{
		return NULL;
	}

	len = cg_strlen(serviceType);
	
	/* Find the first delimiter ':' character */
	for (head = 0; head < len; head++)
	{
		if (serviceType[head] == ':')
		{
			head++; /* Don't include the colon */
			break;
                }
        }

        /* Find the next delimiter ':' character */
        for (tail = head; tail < len; tail++)
        {
                if (serviceType[tail] == ':')
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
                	cg_strncpy(part, serviceType + head, tail - head + 1);
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
 * Get the service part of a service type string (usually just "service")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref cg_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* cg_upnp_servicetype_getservice(char* serviceType)
{
	char* part = NULL;
	int tail = 0;
	int head = 0;
	int len = 0;
	int count = 0;

	cg_log_debug_l4("Entering...\n");

	if (serviceType == NULL)
	{
		return NULL;
	}

	len = cg_strlen(serviceType);
	
	/* Skip the first two delimiter ':' characters */
        for (head = 0; head < len; head++)
        {
		if (serviceType[head] == ':')
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
                if (serviceType[tail] == ':')
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
                	cg_strncpy(part, serviceType + head, tail - head + 1);
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
 * Get the type part of a service type string (ex. "ContentDirectory")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref cg_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* cg_upnp_servicetype_gettype(char* serviceType)
{
	char* part = NULL;
	int tail = 0;
	int head = 0;
	int len = 0;
	int count = 0;
	
	cg_log_debug_l4("Entering...\n");

	if (serviceType == NULL)
	{
		return NULL;
	}
	
	len = cg_strlen(serviceType);
	
	/* Skip the first three delimiter ':' characters */
        for (head = 0; head < len; head++)
        {
		if (serviceType[head] == ':')
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
                if (serviceType[tail] == ':')
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
                	cg_strncpy(part, serviceType + head, tail - head + 1);
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
 * Get the schema type part of a service type string (without last colon)
 * (ex. "urn:schemas-upnp-org:service:ContentDirectory")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref cg_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* cg_upnp_servicetype_getschematype(char* serviceType)
{
	char* part = NULL;
	int tail = 0;
	int len = 0;
	int count = 0;
	
	if (serviceType == NULL)
	{
		return NULL;
	}
	
	cg_log_debug_l4("Entering...\n");

	len = cg_strlen(serviceType);
	
	/* Skip the first four delimiter ':' characters */
        for (tail = 0; tail < len; tail++)
        {
		if (serviceType[tail] == ':')
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
                	cg_strncpy(part, serviceType, tail + 1);
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
 * Get the version part of a service type string (ex. "1")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref cg_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* cg_upnp_servicetype_getversion(char* serviceType)
{
	char* part = NULL;
	int tail = 0;
	int head = 0;
	int len = 0;
	int count = 0;

	cg_log_debug_l4("Entering...\n");

	if (serviceType == NULL)
	{
		return NULL;
	}
	
	len = cg_strlen(serviceType);
	
        /* Skip the first four delimiter ':' characters */
        for (head = 0; head < len; head++)
        {
                if (serviceType[head] == ':')
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
                if (serviceType[tail] == ':')
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
                	cg_strncpy(part, serviceType + head, tail - head + 1);
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
* Announce
*
****************************************/

char *cg_upnp_service_getnotifyservicetypent(CgUpnpService *service, char *buf, int bufSize)
{
	cg_log_debug_l4("Entering...\n");

	cg_strncpy(buf, cg_upnp_service_getservicetype(service), bufSize);
	buf[bufSize-1]='\0';
	
	cg_log_debug_l4("Leaving...\n");
	
	return buf;
}

char *cg_upnp_service_getnotifyservicetypeusn(CgUpnpService *service, char *buf, int bufSize)
{
	CgUpnpDevice *dev;

	cg_log_debug_l4("Entering...\n");

	dev = cg_upnp_service_getdevice(service);
#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize, "%s::%s", cg_upnp_device_getudn(dev), cg_upnp_service_getservicetype(service));
#else
	sprintf(buf, "%s::%s", cg_upnp_device_getudn(dev), cg_upnp_service_getservicetype(service));
#endif
	
	cg_log_debug_l4("Leaving...\n");
	
	return buf;
}

BOOL cg_upnp_service_announcefrom(CgUpnpService *service, char *bindAddr)
{
	/**** uuid:device-UUID::urn:schemas-upnp-org:service:serviceType:v ****/
	char ssdpLineBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	CgUpnpDevice *rootDev;
	CgUpnpDevice *dev;
	CgUpnpSSDPRequest *ssdpReq;
	CgUpnpSSDPSocket *ssdpSock;
	BOOL sentResult = TRUE;
	
	cg_log_debug_l4("Entering...\n");

	rootDev = cg_upnp_service_getrootdevice(service);
	dev = cg_upnp_service_getdevice(service);
	
	ssdpReq = cg_upnp_ssdprequest_new();
	cg_upnp_ssdprequest_setserver(ssdpReq, cg_upnp_getservername(ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setleasetime(ssdpReq, cg_upnp_device_getleasetime(dev));
	cg_upnp_ssdprequest_setlocation(ssdpReq, cg_upnp_device_getlocationurl(rootDev, bindAddr, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setnts(ssdpReq, CG_UPNP_SSDP_NTS_ALIVE);
	cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_service_getnotifyservicetypent(service, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_service_getnotifyservicetypeusn(service, ssdpLineBuf, sizeof(ssdpLineBuf)));
	
	ssdpSock = cg_upnp_ssdp_socket_new();
	sentResult = cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	cg_wait(20);
	cg_upnp_ssdp_socket_delete(ssdpSock);

	cg_upnp_ssdprequest_delete(ssdpReq);
	
	cg_log_debug_l4("Leaving...\n");

	return sentResult;
}

BOOL cg_upnp_service_byebyefrom(CgUpnpService *service, char *bindAddr)
{
	/**** uuid:device-UUID::urn:schemas-upnp-org:service:serviceType:v ****/
	char ssdpLineBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	CgUpnpSSDPRequest *ssdpReq;
	CgUpnpSSDPSocket *ssdpSock;
	BOOL sentResult;
		
	cg_log_debug_l4("Entering...\n");

	ssdpReq = cg_upnp_ssdprequest_new();
	
	cg_upnp_ssdprequest_setnts(ssdpReq, CG_UPNP_SSDP_NTS_BYEBYE);
	cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_service_getnotifyservicetypent(service, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_service_getnotifyservicetypeusn(service, ssdpLineBuf, sizeof(ssdpLineBuf)));

	ssdpSock = cg_upnp_ssdp_socket_new();
	sentResult = cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	cg_upnp_ssdp_socket_delete(ssdpSock);
	
	cg_upnp_ssdprequest_delete(ssdpReq);

	cg_log_debug_l4("Leaving...\n");

	return sentResult;
}


/****************************************
*
* Child Node
*
****************************************/

/****************************************
* cg_upnp_service_initchildnodes
****************************************/

static void cg_upnp_service_initchildnodes(CgUpnpService *service)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_service_initactionlist(service);
	cg_upnp_service_initservicestatetable(service);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
*
* Embedded Action
*
****************************************/

/****************************************
* cg_upnp_service_initactionlist
****************************************/

static void cg_upnp_service_initactionlist(CgUpnpService *service)
{
	CgXmlNode *scdpNode;
	CgXmlNode *actionListNode;
//	CgXmlNode *serviceNode;
	CgXmlNode *childNode;
	CgUpnpAction *action;
	
	cg_log_debug_l4("Entering...\n");

	cg_upnp_actionlist_clear(service->actionList);
	
	scdpNode = cg_upnp_service_getscpdnode(service);
	if (scdpNode == NULL)
		return;
		
	actionListNode = cg_xml_node_getchildnode(scdpNode, CG_UPNP_ACTIONLIST_ELEM_NAME);
	if (actionListNode == NULL)
		return;
		
	//serviceNode = cg_upnp_service_getservicenode(service);
	for (childNode = cg_xml_node_getchildnodes(actionListNode); childNode != NULL; childNode = cg_xml_node_next(childNode)) {
		if (cg_upnp_action_isactionnode(childNode) == FALSE)
			continue;
		action = cg_upnp_action_new();
		cg_upnp_action_setservice(action, service);
		cg_upnp_action_setactionnode(action, childNode);
		cg_upnp_actionlist_add(service->actionList, action);
	} 

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_service_getactionbyname
****************************************/

CgUpnpAction *cg_upnp_service_getactionbyname(CgUpnpService *service, char *name)
{
	CgUpnpActionList *actionList;
	CgUpnpAction *action;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(name) <= 0)
		return NULL;
			
#ifdef CG_OPTIMIZED_CP_MODE
	if (cg_upnp_service_isparsed(service) == FALSE)
		cg_upnp_controlpoint_parsescservicescpd(service);
#endif
	actionList = cg_upnp_service_getactionlist(service);
	for (action=cg_upnp_actionlist_gets(actionList); action != NULL; action = cg_upnp_action_next(action)) {
		if (cg_upnp_action_isname(action, name) == TRUE)
			return action;
	}
	
	cg_log_debug_l4("Leaving...\n");
	
	return NULL;
}

/****************************************
*
* Embedded ServiceStateTable
*
****************************************/

/****************************************
* cg_upnp_service_initservicestatetable
****************************************/

static void cg_upnp_service_initservicestatetable(CgUpnpService *service)
{
	CgXmlNode *scdpNode;
	CgXmlNode *stateTableNode;
//	CgXmlNode *serviceNode;
	CgXmlNode *childNode;
	CgUpnpStateVariable *statVar;
	
	cg_log_debug_l4("Entering...\n");

	cg_upnp_servicestatetable_clear(service->serviceStateTable);
	
	scdpNode = cg_upnp_service_getscpdnode(service);
	if (scdpNode == NULL)
		return;
		
	stateTableNode = cg_xml_node_getchildnode(scdpNode, CG_UPNP_SERVICESTATETALBE_ELEM_NAME);
	if (stateTableNode == NULL)
		return;
		
//	serviceNode = cg_upnp_service_getservicenode(service);
	for (childNode = cg_xml_node_getchildnodes(stateTableNode); childNode != NULL; childNode = cg_xml_node_next(childNode)) {
		if (cg_upnp_statevariable_isstatevariablenode(childNode) == FALSE)
			continue;
		statVar = cg_upnp_statevariable_new();
		cg_upnp_statevariable_setservice(statVar, service);
		cg_upnp_statevariable_setstatevariablenode(statVar, childNode);
		cg_upnp_servicestatetable_add(service->serviceStateTable, statVar);
	} 

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_service_getstatevariablebyname
****************************************/

CgUpnpStateVariable *cg_upnp_service_getstatevariablebyname(CgUpnpService *service, char *name)
{
	CgUpnpServiceStateTable *stateTable;
	CgUpnpStateVariable *stateVar;
	
	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(name) <= 0)
		return NULL;
			
#ifdef CG_OPTIMIZED_CP_MODE
	if (cg_upnp_service_isparsed(service) == FALSE)
		cg_upnp_controlpoint_parsescservicescpd(service);
#endif
	stateTable = cg_upnp_service_getservicestatetable(service);
	for (stateVar=cg_upnp_servicestatetable_gets(stateTable); stateVar != NULL; stateVar = cg_upnp_statevariable_next(stateVar)) {
		if (cg_upnp_statevariable_isname(stateVar, name) == TRUE)
			return stateVar;
	}
	
	cg_log_debug_l4("Leaving...\n");
	
	return NULL;
}

/****************************************
* cg_upnp_service_setactionlistener
****************************************/

void cg_upnp_service_setactionlistener(CgUpnpService *service, CG_UPNP_ACTION_LISTNER actionListener)
{
	CgUpnpActionList *actionList;
	CgUpnpAction *action;
	
	cg_log_debug_l4("Entering...\n");

	actionList = cg_upnp_service_getactionlist(service);
	for (action=cg_upnp_actionlist_gets(actionList); action != NULL; action = cg_upnp_action_next(action))
		cg_upnp_action_setlistner(action, actionListener);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_service_setquerylistener
****************************************/

void cg_upnp_service_setquerylistener(CgUpnpService *service, CG_UPNP_STATEVARIABLE_LISTNER queryListener)
{
	CgUpnpServiceStateTable *stateTable;
	CgUpnpStateVariable *stateVar;
	
	cg_log_debug_l4("Entering...\n");

	stateTable = cg_upnp_service_getservicestatetable(service);
	for (stateVar=cg_upnp_servicestatetable_gets(stateTable); stateVar != NULL; stateVar = cg_upnp_statevariable_next(stateVar)) 
		cg_upnp_statevariable_setlistener(stateVar, queryListener);

	cg_log_debug_l4("Leaving...\n");
}

CgUpnpActionList *cg_upnp_service_getactionlist(CgUpnpService *service)
{
	cg_log_debug_l4("Entering...\n");

#ifdef CG_OPTIMIZED_CP_MODE
	if (cg_upnp_service_isparsed(service) == FALSE)
		cg_upnp_controlpoint_parsescservicescpd(service);
#endif
	return service->actionList;

	cg_log_debug_l4("Leaving...\n");
}

CgUpnpAction *cg_upnp_service_getactions(CgUpnpService *service)
{
	cg_log_debug_l4("Entering...\n");

#ifdef CG_OPTIMIZED_CP_MODE
	if (cg_upnp_service_isparsed(service) == FALSE)
		cg_upnp_controlpoint_parsescservicescpd(service);
#endif
	return cg_upnp_actionlist_gets(service->actionList);

	cg_log_debug_l4("Leaving...\n");
}

CgUpnpServiceStateTable *cg_upnp_service_getservicestatetable(CgUpnpService *service)
{
	cg_log_debug_l4("Entering...\n");

#ifdef CG_OPTIMIZED_CP_MODE
	if (cg_upnp_service_isparsed(service) == FALSE)
		cg_upnp_controlpoint_parsescservicescpd(service);
#endif
	return service->serviceStateTable;

	cg_log_debug_l4("Leaving...\n");
}

CgUpnpStateVariable *cg_upnp_service_getstatevariables(CgUpnpService *service)
{
	cg_log_debug_l4("Entering...\n");

#ifdef CG_OPTIMIZED_CP_MODE
	if (cg_upnp_service_isparsed(service) == FALSE)
		cg_upnp_controlpoint_parsescservicescpd(service);
#endif
	return cg_upnp_servicestatetable_gets(service->serviceStateTable);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
*
*	Subscription
*
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* cg_upnp_service_addsubscriber
****************************************/

BOOL cg_upnp_service_addsubscriber(CgUpnpService *service, CgUpnpSubscriber *sub) 
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_subscriberlist_add(service->subscriberList, sub);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* `
****************************************/

BOOL cg_upnp_service_removesubscriber(CgUpnpService *service, CgUpnpSubscriber *sub) 
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_subscriber_remove(sub);
	cg_upnp_subscriber_delete(sub);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_upnp_service_getsubscriberbysid
****************************************/

CgUpnpSubscriber *cg_upnp_service_getsubscriberbysid(CgUpnpService *service, char *sid)
{
	CgUpnpSubscriber *sub;
	char *subSid;

	cg_log_debug_l4("Entering...\n");

	if (cg_strlen(sid) <= 0)
		return NULL;
	
	if (0 <= cg_strstr(sid, CG_UPNP_ST_UUID_DEVICE))
		sid += cg_strlen(CG_UPNP_ST_UUID_DEVICE) + 1;

	for (sub = cg_upnp_service_getsubscribers(service); sub != NULL; sub = cg_upnp_subscriber_next(sub)) {
		subSid = cg_upnp_subscriber_getsid(sub);
		if (0 <= cg_strstr(subSid, CG_UPNP_ST_UUID_DEVICE))
			subSid += cg_strlen(CG_UPNP_ST_UUID_DEVICE) + 1;
		if (cg_streq(sid, subSid) == TRUE)
			return sub;
	}
	
	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/* Private helper functions */

static CgNetURL *cg_upnp_service_mangleurl(CgUpnpService *service, char *type)
{
	char *gen_url_str;
	char *location_str;
	CgNetURL *genURL;
	CgUpnpDevice *rootDev;
	CgNetURL *temp;

	gen_url_str = cg_xml_node_getchildnodevalue(cg_upnp_service_getservicenode(service), type);
	location_str = NULL;
	genURL = cg_net_url_new();

	if (cg_strlen(gen_url_str) <= 0)
		return NULL;

	cg_log_debug_s("URL string: %s type: %s\n", gen_url_str, type);
	cg_net_url_set(genURL, gen_url_str);

	/* Absolute URL case */
	if (cg_net_url_isabsolute(genURL) == TRUE)
	{
		cg_log_debug_s("Mangled URL: %s\n", cg_net_url_getrequest(genURL));
		return genURL;
	}

	/* URL base + absolute/relative path */
	rootDev = cg_upnp_service_getrootdevice(service);

	if (rootDev != NULL) {
		if (0 < cg_strlen(cg_upnp_device_geturlbase(rootDev))) {
			CgNetURL *temp = cg_net_url_new();
			cg_net_url_set(temp, gen_url_str);

			cg_net_url_set(genURL, cg_upnp_device_geturlbase(rootDev));


			if ( cg_net_url_isabsolutepath(gen_url_str) ) {
				cg_net_url_setpath(genURL, cg_net_url_getpath(temp));
				cg_net_url_setquery(genURL, cg_net_url_getquery(temp));
			}
			else {
				cg_net_url_addpath(genURL, cg_net_url_getpath(temp));
				cg_net_url_setquery(genURL, cg_net_url_getquery(temp));
				cg_net_url_rebuild(genURL);
			}

			cg_net_url_delete(temp);

			cg_log_debug_s("Mangled URL: %s\n", cg_net_url_getrequest(genURL));
			return genURL;
		}
		/* URL base from location + absolute/relative path */
		location_str = cg_upnp_device_getlocationfromssdppacket(rootDev);
		cg_log_debug_s("Location from SSDP packet: %s\n", location_str);
		
		if (cg_strlen(location_str) <= 0) {
			cg_log_debug_s("Could not get location string from SSDP packet!\n");
			return genURL;
        }

        cg_net_url_set(genURL, location_str);
		
		cg_log_debug_s("URL from location: %s - %d -%s", 
					   cg_net_url_gethost(genURL),
					   cg_net_url_getport(genURL),
					   cg_net_url_getpath(genURL));

        /* UPnP spec says that path in location URL with last part removed should be 
		 * considered as base path when getting service descriptions, if relative
		 * paths are used in description XML.
         *
         * So we convert location http://192.168.1.1/base/path/description
         * into http://192.168.1.1/base/path/
         */

		location_str = cg_net_url_getupnpbasepath(genURL);
		
		if (location_str) {
			cg_net_url_setpath(genURL, location_str);
			free(location_str);
			location_str = NULL;
		}

		temp = cg_net_url_new();
		cg_net_url_set(temp, gen_url_str);
		
		if ( cg_net_url_isabsolutepath(gen_url_str) ) {
			cg_net_url_setpath(genURL, cg_net_url_getpath(temp));
			cg_net_url_setquery(genURL, cg_net_url_getquery(temp));
		}
		else {
			cg_net_url_addpath(genURL, cg_net_url_getpath(temp));
			cg_net_url_setquery(genURL, cg_net_url_getquery(temp));
		}
		
		cg_net_url_delete(temp);

		cg_log_debug_s("Mangled URL: %s\n", cg_net_url_getrequest(genURL));
		
        return genURL;
	}

	return NULL;
}

#endif
