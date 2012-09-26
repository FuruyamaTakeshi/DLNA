/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006-2007 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: chttp_packet.cpp
*
*	Revision:
*
*	01/25/05
*		- first revision
*	08/21/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Changed cg_http_packet_read() to store length in HTTPPacket.
*	10/25/05
*		- The following functions move to define macros.
*		  cg_http_packet_setcontent()
*		  cg_http_packet_setcontentpointer()
*		  cg_http_packet_getcontent()
*		- cg_http_packet_setncontent() is added.
*	10/31/05
*		- cg_http_packet_sethost:
*		  port was an excess parameter for s(n)printf when port <= 0
*	11/11/05
*		- Added cg_http_packet_setheaderlonglong() and cg_http_packet_getheaderlonglong().
*		- Extended cg_http_packet_setcontentlength() and cg_http_packet_getcontentlength() to 64bit
*		  when the compiler is supported C99 or the platform is WIN32.
*	02/01/07
*		- Fixed cg_http_request_post() not to hung up when the request method is HEAD.
*		- Added a onlyHeader parameter to cg_http_response_read() and cg_http_response_packet().
*	03/18/07
*		- Changed the following functions to use CgInt64.
*		  cg_http_packet_setheaderlonglong()
*		  cg_http_packet_getheaderlonglong()
*	11/16\07  Satoshi Konno <skonno@cybergarage.org>
*		- Fixed cg_http_packet_read_body()not to lost data when the response packet is huge.
*	12/13/07  Aapo makela <aapo.makela@nokia.com>
*		- Fix to cg_http_packet_sethost() not to crash in out-of-memory situation
*
******************************************************************/

#include <cybergarage/http/chttp.h>
#include <cybergarage/util/clog.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/net/cinterface.h>

#include <limits.h>

#define READBUF_LENGTH 1024

/****************************************
* cg_http_packet_new
****************************************/

CgHttpPacket *cg_http_packet_new()
{
	CgHttpPacket *httpPkt;

	cg_log_debug_l4("Entering...\n");

	httpPkt= (CgHttpPacket *)malloc(sizeof(CgHttpPacket));

	if ( NULL != httpPkt )
	{
		cg_http_packet_init(httpPkt);
	}

	cg_log_debug_l4("Leaving...\n");

	return httpPkt;
}

/****************************************
* cg_http_packet_delete
****************************************/

void cg_http_packet_delete(CgHttpPacket *httpPkt)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_packet_clean(httpPkt);
	free(httpPkt);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_init
****************************************/

void cg_http_packet_init(CgHttpPacket *httpPkt)
{
	cg_log_debug_l4("Entering...\n");

	httpPkt->headerList = cg_http_headerlist_new();
	httpPkt->content = cg_string_new();

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_clean
****************************************/

void cg_http_packet_clean(CgHttpPacket *httpPkt)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_headerlist_delete(httpPkt->headerList);
	cg_string_delete(httpPkt->content);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_clear
****************************************/

void cg_http_packet_clear(CgHttpPacket *httpPkt)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_headerlist_clear(httpPkt->headerList);
	cg_string_setvalue(httpPkt->content, NULL);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_setheader
****************************************/

void cg_http_packet_setheadervalue(CgHttpPacket *httpPkt, char* name, char *value)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_headerlist_set(httpPkt->headerList, name, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_setheaderinteger
****************************************/

void cg_http_packet_setheaderinteger(CgHttpPacket *httpPkt, char* name, int value)
{
	char svalue[CG_STRING_INTEGER_BUFLEN];

	cg_log_debug_l4("Entering...\n");

	cg_http_packet_setheadervalue(httpPkt, name, cg_int2str(value, svalue, sizeof(svalue)));

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_setheaderlong
****************************************/

void cg_http_packet_setheaderlong(CgHttpPacket *httpPkt, char* name, long value)
{
	char svalue[CG_STRING_LONG_BUFLEN];

	cg_log_debug_l4("Entering...\n");

	cg_http_packet_setheadervalue(httpPkt, name, cg_long2str(value, svalue, sizeof(svalue)));

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_setheaderlonglong
****************************************/

#if defined(CG_USE_INT64)
void cg_http_packet_setheaderlonglong(CgHttpPacket *httpPkt, char* name, CgInt64 value)
{
	char svalue[CG_STRING_LONGLONG_BUFLEN];
	cg_http_packet_setheadervalue(httpPkt, name, cg_longlong2str(value, svalue, sizeof(svalue)));
}
#endif

/****************************************
* cg_http_packet_getheadervalue
****************************************/

char *cg_http_packet_getheadervalue(CgHttpPacket *httpPkt, char* name)
{
	return cg_http_headerlist_getvalue(httpPkt->headerList, name);
}

/****************************************
* cg_http_packet_getheadervalue
****************************************/

int cg_http_packet_getheaderinteger(CgHttpPacket *httpPkt, char* name)
{
	char *value;

	cg_log_debug_l4("Entering...\n");

	value = cg_http_packet_getheadervalue(httpPkt, name); 

	cg_log_debug_l4("Leaving...\n");

	return (value != NULL) ? atoi(value) : 0;
}

/****************************************
* cg_http_packet_getheadervalue
****************************************/

long cg_http_packet_getheaderlong(CgHttpPacket *httpPkt, char* name)
{
	char *value;

	cg_log_debug_l4("Entering...\n");

	value = cg_http_packet_getheadervalue(httpPkt, name); 

	cg_log_debug_l4("Leaving...\n");

	return (value != NULL) ? atol(value) : 0;
}

/****************************************
* cg_http_packet_getheaderlonglong
****************************************/

#if defined(CG_USE_INT64)
CgInt64 cg_http_packet_getheaderlonglong(CgHttpPacket *httpPkt, char* name)
{
	char *value;

	cg_log_debug_l4("Entering...\n");

	value = cg_http_packet_getheadervalue(httpPkt, name); 

	cg_log_debug_l4("Leaving...\n");

	return (value != NULL) ? cg_str2longlong(value) : 0;
}
#endif

/****************************************
* cg_http_packet_getheadervalue
****************************************/

void cg_http_packet_sethost(CgHttpPacket *httpPkt, char *addr, int port)
{
	char *host;
	int hostMaxLen;

	cg_log_debug_l4("Entering...\n");
	
	if (addr == NULL)
		return;

	hostMaxLen = cg_strlen(addr) + CG_NET_IPV6_ADDRSTRING_MAXSIZE + CG_STRING_INTEGER_BUFLEN;
	host = malloc(sizeof(char) * hostMaxLen);

	if (host == NULL)
		/* Memory allocation failure */
		return;

#if defined(HAVE_SNPRINTF)
	if (0 < port && port != CG_HTTP_DEFAULT_PORT) {
		if (cg_net_isipv6address(addr) == TRUE)
			snprintf(host, hostMaxLen, "[%s]:%d", addr, port);
		else
			snprintf(host, hostMaxLen, "%s:%d", addr, port);
	}
	else {
		if (cg_net_isipv6address(addr) == TRUE)
			snprintf(host, hostMaxLen, "[%s]", addr);
		else
			snprintf(host, hostMaxLen, "%s", addr);
	}
#else	
	if (0 < port && port != CG_HTTP_DEFAULT_PORT) {
		if (cg_net_isipv6address(addr) == TRUE)
			sprintf(host, "[%s]:%d", addr, port);
		else
			sprintf(host, "%s:%d", addr, port);
	}
	else {
		if (cg_net_isipv6address(addr) == TRUE)
			sprintf(host, "[%s]", addr);
		else
			sprintf(host, "%s", addr);
	}
#endif

	cg_http_packet_setheadervalue(httpPkt, CG_HTTP_HOST, host);

	free(host);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_post
****************************************/

void cg_http_packet_post(CgHttpPacket *httpPkt, CgSocket *sock)
{
	CgHttpHeader *header;
	char *name, *value;
	char *content;
	CgInt64 contentLen;
	
	cg_log_debug_l4("Entering...\n");

	/**** send headers ****/
	for (header = cg_http_packet_getheaders(httpPkt); header != NULL; header = cg_http_header_next(header)) {
		name = cg_http_header_getname(header);
		if (name == NULL)
			continue;
		cg_socket_write(sock, name, cg_strlen(name));
		cg_socket_write(sock, CG_HTTP_COLON, sizeof(CG_HTTP_COLON)-1);
		cg_socket_write(sock, CG_HTTP_SP, sizeof(CG_HTTP_SP)-1);
		value = cg_http_header_getvalue(header);
		if (value != NULL)
			cg_socket_write(sock, value, cg_strlen(value));
		cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
	}
	cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
	
	/**** send content ****/
	content = cg_http_packet_getcontent(httpPkt);
	contentLen = cg_http_packet_getcontentlength(httpPkt);
	if (content != NULL && 0 < contentLen)
		cg_socket_write(sock, content, contentLen);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_read_headers
****************************************/

void cg_http_packet_read_headers(CgHttpPacket *httpPkt, CgSocket *sock, char *lineBuf, int lineBufSize)
{
	CgStringTokenizer *strTok;
	CgHttpHeader *header;
	int readLen;
	char *name, *value;
	
	cg_log_debug_l4("Entering...\n");

	while (1) {
		readLen = cg_socket_readline(sock, lineBuf, lineBufSize);
		if (readLen <= 2)
			break;
		name = NULL;
		value = NULL;
		strTok = cg_string_tokenizer_new(lineBuf, CG_HTTP_HEADERLINE_DELIM);
		if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
			name = cg_string_tokenizer_nexttoken(strTok);
		if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE) {
			value = cg_string_tokenizer_nextalltoken(strTok);
			cg_strrtrim(value, CG_HTTP_HEADERLINE_DELIM, cg_strlen(CG_HTTP_HEADERLINE_DELIM));
		}
		if (name != NULL) {
			if (value == NULL)
				value = "";
			header = cg_http_header_new();
			cg_http_header_setname(header, name);
			cg_http_header_setvalue(header, value);
			cg_http_packet_addheader(httpPkt, header);
		}
		cg_string_tokenizer_delete(strTok);
	}

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_read_chunk
****************************************/

long cg_http_packet_read_chunk(CgHttpPacket *httpPkt, CgSocket *sock, char *lineBuf, int lineBufSize)
{
	long readLen = 0;
	long conLen = 0;
	int tries = 0;
	char *content = NULL;
	
	cg_log_debug_l4("Entering...\n");

	/* Read chunk header */
	readLen = cg_socket_readline(sock, lineBuf, lineBufSize);
	
	conLen = cg_strhex2long(lineBuf);
	if (conLen < 1) return 0;
	
	content = (char *)malloc(conLen+1);

	if (content == NULL)
	{
		cg_log_debug_s("Memory allocation problem!\n");
		return 0;
	}
		
	content[conLen] = 0;
	
	readLen = 0;
	/* Read content until conLen is reached, or tired of trying */
	while (readLen < conLen && tries < 20)
	{
		readLen += cg_socket_read(sock, (content+readLen), (conLen-readLen));
		tries++;
	}
	
	/* Append content to packet */
	cg_http_packet_appendncontent(httpPkt, content, readLen);
	free(content); content = NULL;
	
	if (readLen == conLen)
	{
		/* Read CRLF bytes */
		cg_socket_readline(sock, lineBuf, lineBufSize);
	}
	
	cg_log_debug_l4("Leaving...\n");

	return readLen;
}

/****************************************
* cg_http_packet_read_body
****************************************/

BOOL cg_http_packet_read_body(CgHttpPacket *httpPkt, CgSocket *sock, char *lineBuf, int lineBufSize)
{
	long readLen;
	int conLen;
	char *content;
	char readBuf[READBUF_LENGTH + 1];
	int tries = 0;

	cg_log_debug_l4("Entering...\n");

	conLen = cg_http_packet_getcontentlength(httpPkt);
	content = NULL;
	if (0 < conLen) {
		content = (char *)malloc(conLen+1);
		if (content == NULL)
		{
			cg_log_debug_s("Memory allocation problem!\n");
			return FALSE;
		}
		
		content[0] = '\0';
		readLen = 0;
		
		/* Read content until conLen is reached, or tired of trying */
		while (readLen < conLen && tries < 20)
		{
			readLen += cg_socket_read(sock, (content+readLen), (conLen-readLen));
			/* Fixed to increment the counter only when cg_socket_read() doesn't read data */
			if (readLen <= 0)
				tries++;
		}
		
		if (readLen <= 0)
			return TRUE;
		content[readLen] = '\0';
		cg_http_packet_setcontentpointer(httpPkt, content, readLen);
	}
	else if (cg_http_packet_getheadervalue(httpPkt, 
					CG_HTTP_CONTENT_LENGTH) == NULL)
	{
		/* header existance must be checked! otherwise packets which
		   rightly report 0 as content length, will jam the http */
		
		/* Check if we read chunked encoding */
		if (cg_http_packet_ischunked(httpPkt) == TRUE)
		{
			conLen = 0;
			do {
				readLen = cg_http_packet_read_chunk(httpPkt, sock, lineBuf, lineBufSize);
				conLen += readLen;
			} while (readLen > 0);
			
			cg_http_packet_setcontentlength(httpPkt,conLen);
		} else {
			readLen = 0;
			conLen = 0;
			while ((readLen = cg_socket_read(sock, readBuf, READBUF_LENGTH)) > 0)
			{
				cg_http_packet_appendncontent(httpPkt, readBuf, readLen);
				conLen += readLen;
			}

			cg_http_packet_setcontentlength(httpPkt, conLen);
		}
	}

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_http_packet_read
****************************************/

BOOL cg_http_packet_read(CgHttpPacket *httpPkt, CgSocket *sock, BOOL onlyHeader, char *lineBuf, int lineBufSize)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_packet_clear(httpPkt);
	cg_http_packet_read_headers(httpPkt, sock, lineBuf, lineBufSize);
	
	cg_log_debug_l4("Leaving...\n");

	if (onlyHeader)
		return TRUE;

	return cg_http_packet_read_body(httpPkt, sock, lineBuf, lineBufSize);
}

/****************************************
* cg_http_packet_getheadersize
****************************************/

int cg_http_packet_getheadersize(CgHttpPacket *httpPkt)
{
	CgHttpHeader *header;
	int headerSize;
	char *name;
	char *value;
	
	cg_log_debug_l4("Entering...\n");

	headerSize = 0;
	for (header = cg_http_packet_getheaders(httpPkt); header != NULL; header = cg_http_header_next(header)) {
		name = cg_http_header_getname(header);
		value = cg_http_header_getvalue(header);
		headerSize += cg_strlen(name); 
		headerSize += sizeof(CG_HTTP_COLON)-1; 
		headerSize += sizeof(CG_HTTP_SP)-1; 
		headerSize += cg_strlen(value); 
		headerSize += sizeof(CG_HTTP_CRLF)-1; 
	}
	headerSize += sizeof(CG_HTTP_CRLF)-1; 

	cg_log_debug_l4("Leaving...\n");

	return headerSize;
}

/****************************************
* cg_http_packet_copy
****************************************/

void cg_http_packet_copy(CgHttpPacket *destHttpPkt, CgHttpPacket *srcHttpPkt)
{
	CgHttpHeader *srcHeader;
	CgHttpHeader *destHeader;
	
	cg_log_debug_l4("Entering...\n");

	cg_http_packet_clear(destHttpPkt);
	
	/**** copy headers ****/
	for (srcHeader = cg_http_packet_getheaders(srcHttpPkt); srcHeader != NULL; srcHeader = cg_http_header_next(srcHeader)) {
		destHeader = cg_http_header_new();
		cg_http_header_setname(destHeader, cg_http_header_getname(srcHeader));
		cg_http_header_setvalue(destHeader, cg_http_header_getvalue(srcHeader));
		cg_http_packet_addheader(destHttpPkt, destHeader);
	}
	
	/**** copy content ****/
	cg_http_packet_setcontent(destHttpPkt, cg_http_packet_getcontent(srcHttpPkt));

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_print
****************************************/

void cg_http_packet_print(CgHttpPacket *httpPkt)
{
	CgHttpHeader *header;
	char *content;
	long contentLen;

	cg_log_debug_l4("Entering...\n");

	/**** print headers ****/
	for (header = cg_http_packet_getheaders(httpPkt); header != NULL; header = cg_http_header_next(header)) {
	cg_log_debug_s("%s: %s\n",
			cg_http_header_getname(header),
			cg_http_header_getvalue(header));
	}
	cg_log_debug_s("\n");
	
	/**** print content ****/
	content = cg_http_packet_getcontent(httpPkt);
	contentLen = cg_http_packet_getcontentlength(httpPkt);
	
	if (content != NULL && 0 < contentLen)
		cg_log_debug_s("%s\n", content);

	cg_log_debug_l4("Leaving...\n");
}
