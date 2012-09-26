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
*	File: cssdp_packet.c
*
*	Revision:
*
*	06/01/05
*		- first revision
*
*	10/31/05
*		- Delete also ssdpTok in cg_upnp_ssdp_packet_set_header()

******************************************************************/

#include <cybergarage/upnp/ssdp/cssdp_server.h>
#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/util/cstring.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_ssdp_packet_new
****************************************/

CgUpnpSSDPPacket *cg_upnp_ssdp_packet_new()
{
	CgUpnpSSDPPacket *ssdpPkt;

	cg_log_debug_l4("Entering...\n");

	ssdpPkt = (CgUpnpSSDPPacket *)malloc(sizeof(CgUpnpSSDPPacket));
	
	if ( NULL != ssdpPkt )
	{
		ssdpPkt->dgmPkt = cg_socket_datagram_packet_new();
		ssdpPkt->headerList = cg_http_headerlist_new();
		ssdpPkt->initialized = 0;

		cg_upnp_ssdp_packet_setuserdata(ssdpPkt, NULL);
		cg_upnp_ssdp_packet_settimestamp(ssdpPkt, cg_getcurrentsystemtime());

		ssdpPkt->timestamps = (CgSysTime *)malloc(CG_UPNP_SSDP_FILTER_TABLE_SIZE * sizeof(CgSysTime));
	}
	
	return ssdpPkt;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_packet_delete
****************************************/

void cg_upnp_ssdp_packet_delete(CgUpnpSSDPPacket *ssdpPkt)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_ssdp_packet_clear(ssdpPkt);
	
	cg_socket_datagram_packet_delete(ssdpPkt->dgmPkt);
	cg_http_headerlist_delete(ssdpPkt->headerList);

	free(ssdpPkt->timestamps);
	
	free(ssdpPkt);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_packet_clear
****************************************/

void cg_upnp_ssdp_packet_clear(CgUpnpSSDPPacket *ssdpPkt)
{
	cg_log_debug_l4("Entering...\n");

	cg_socket_datagram_packet_setdata(ssdpPkt->dgmPkt, NULL);
	cg_http_headerlist_clear(ssdpPkt->headerList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_packet_isrootdevice
****************************************/

BOOL cg_upnp_ssdp_packet_isrootdevice(CgUpnpSSDPPacket *ssdpPkt)
{
	cg_log_debug_l4("Entering...\n");

	if (cg_upnp_nt_isrootdevice(cg_upnp_ssdp_packet_getnt(ssdpPkt)) == TRUE)
		return TRUE;
	if (cg_upnp_st_isrootdevice(cg_upnp_ssdp_packet_getst(ssdpPkt)) == TRUE)
		return TRUE;
	return cg_upnp_usn_isrootdevice(cg_upnp_ssdp_packet_getusn(ssdpPkt));

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_packet_setheader
****************************************/

void cg_upnp_ssdp_packet_setheader(CgUpnpSSDPPacket *ssdpPkt, char *ssdpMsg)
{
	CgStringTokenizer *ssdpTok;
	CgStringTokenizer *ssdpLineTok;
	CgHttpHeader *header;
	char *lineMsg;
	char *name;
	char *value;
		
	cg_log_debug_l4("Entering...\n");

	ssdpTok = cg_string_tokenizer_new(ssdpMsg, CG_HTTP_CRLF);

	/**** skip the first line ****/	
	if (cg_string_tokenizer_hasmoretoken(ssdpTok) == FALSE)
		return;
	lineMsg = cg_string_tokenizer_nexttoken(ssdpTok);
	
	while (cg_string_tokenizer_hasmoretoken(ssdpTok) == TRUE) {
		lineMsg = cg_string_tokenizer_nexttoken(ssdpTok);
		name = NULL;
		value = NULL;
		ssdpLineTok = cg_string_tokenizer_new(lineMsg, CG_HTTP_HEADERLINE_DELIM);
		if (cg_string_tokenizer_hasmoretoken(ssdpLineTok) == TRUE)
			name = cg_string_tokenizer_nexttoken(ssdpLineTok);
		if (cg_string_tokenizer_hasmoretoken(ssdpLineTok) == TRUE) {
			value = cg_string_tokenizer_nextalltoken(ssdpLineTok);
			cg_strrtrim(value, CG_HTTP_HEADERLINE_DELIM, cg_strlen(CG_HTTP_HEADERLINE_DELIM));
		}
		if (name != NULL) {
			if (value == NULL)
				value = "";
			header = cg_http_header_new();
			cg_http_header_setname(header, name);
			cg_http_header_setvalue(header, value);
			cg_http_headerlist_add(ssdpPkt->headerList, header);
		}
		cg_string_tokenizer_delete(ssdpLineTok);
	}

	cg_string_tokenizer_delete(ssdpTok);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
 * cg_upnp_ssdp_packet_getmaxage
 ****************************************/

long cg_upnp_ssdp_packet_getmaxage(CgUpnpSSDPPacket *ssdpPkt)
{
	char *cachecontrol = NULL;
	int maxageIdx = 0;
	
	cg_log_debug_l4("Entering...\n");

	cachecontrol = cg_upnp_ssdp_packet_getcachecontrol(ssdpPkt);
	if (cachecontrol == NULL) return 0;
	
	maxageIdx = cg_strstr(cachecontrol, CG_HTTP_MAX_AGE);
	if (maxageIdx < 0) return 0;
		
	maxageIdx = cg_strstr(cachecontrol+maxageIdx, "=");
	if (maxageIdx <= 0) return 0;
		
	maxageIdx++;
	
	return cg_str2long(cachecontrol+maxageIdx);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_packet_copy
****************************************/

void cg_upnp_ssdp_packet_copy(CgUpnpSSDPPacket *dstSsdpPkt, CgUpnpSSDPPacket *srcSsdpPkt)
{
	CgHttpHeader *srcHeader;
	CgHttpHeader *destHeader;
	
	cg_log_debug_l4("Entering...\n");

	cg_socket_datagram_packet_copy(dstSsdpPkt->dgmPkt, srcSsdpPkt->dgmPkt);

	/**** copy headers ****/
	cg_upnp_ssdp_packet_clear(dstSsdpPkt);
	for (srcHeader = cg_upnp_ssdp_packet_getheaders(srcSsdpPkt); srcHeader != NULL; srcHeader = cg_http_header_next(srcHeader)) {
		destHeader = cg_http_header_new();
		cg_http_header_setname(destHeader, cg_http_header_getname(srcHeader));
		cg_http_header_setvalue(destHeader, cg_http_header_getvalue(srcHeader));
		cg_upnp_ssdp_packet_addheader(dstSsdpPkt, destHeader);
	}
	
	/* Set timestamp */
	cg_upnp_ssdp_packet_settimestamp(dstSsdpPkt, cg_getcurrentsystemtime());

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_ssdp_packet_print
****************************************/

void cg_upnp_ssdp_packet_print(CgUpnpSSDPPacket *ssdpPkt)
{
	CgHttpHeader *header;
	
	cg_log_debug_l4("Entering...\n");

	cg_log_debug_s("ssdp from %s %d\n",
		cg_upnp_ssdp_packet_getremoteaddress(ssdpPkt),
		cg_upnp_ssdp_packet_getremoteport(ssdpPkt));
		
	/**** print headers ****/
	for (header = cg_http_headerlist_gets(ssdpPkt->headerList); header != NULL; header = cg_http_header_next(header)) {
	cg_log_debug_s("%s: %s\n",
			cg_http_header_getname(header),
			cg_http_header_getvalue(header));
	}


	cg_log_debug_l4("Leaving...\n");
}
