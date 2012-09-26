/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: chttp_response.c
*
*	Revision:
*
*	02/01/05
*		- first revision
*
******************************************************************/

#include <cybergarage/util/clist.h>
#include <cybergarage/http/chttp.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_http_response_new
****************************************/

CgHttpResponse *cg_http_response_new()
{
	CgHttpResponse *httpRes;

	cg_log_debug_l4("Entering...\n");

	httpRes = (CgHttpResponse *)malloc(sizeof(CgHttpResponse));
	
	if ( NULL != httpRes )
	{
		cg_http_packet_init((CgHttpPacket *)httpRes);
		httpRes->version = cg_string_new();
		httpRes->reasonPhrase = cg_string_new();
		
		cg_http_response_setversion(httpRes, CG_HTTP_VER11);
		cg_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_BAD_REQUEST);
		cg_http_response_setuserdata(httpRes, NULL);

		cg_http_response_settimeout(httpRes, CG_HTTP_CONN_TIMEOUT);
	}
	
	return httpRes;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_response_delete
****************************************/

void cg_http_response_delete(CgHttpResponse *httpRes)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_packet_clean((CgHttpPacket *)httpRes);
	cg_string_delete(httpRes->version);
	cg_string_delete(httpRes->reasonPhrase);
	free(httpRes);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_response_clear
****************************************/

void cg_http_response_clear(CgHttpResponse *httpRes)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_packet_clear((CgHttpPacket *)httpRes);
	cg_http_response_setversion(httpRes, NULL);
	cg_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_INTERNAL_SERVER_ERROR);
	cg_http_response_setreasonphrase(httpRes, NULL);
	cg_http_response_setuserdata(httpRes, NULL);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_response_setversion
****************************************/

void cg_http_response_setversion(CgHttpResponse *httpRes, char *value)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(httpRes->version, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_response_getversion
****************************************/

char *cg_http_response_getversion(CgHttpResponse *httpRes)
{
	cg_log_debug_l4("Entering...\n");

	return cg_string_getvalue(httpRes->version);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_response_setstatuscode
****************************************/

void cg_http_response_setstatuscode(CgHttpResponse *httpRes, int value)
{
	cg_log_debug_l4("Entering...\n");

	httpRes->statusCode = value;
	cg_http_response_setreasonphrase(httpRes,
			(char*) cg_http_statuscode2reasonphrase(value));

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_response_getstatuscode
****************************************/

int cg_http_response_getstatuscode(CgHttpResponse *httpRes)
{
	cg_log_debug_l4("Entering...\n");

	return httpRes->statusCode;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_response_setreasonphrase
****************************************/

void cg_http_response_setreasonphrase(CgHttpResponse *httpRes, char *value)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(httpRes->reasonPhrase, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_response_getreasonphrase
****************************************/

char *cg_http_response_getreasonphrase(CgHttpResponse *httpRes)
{
	cg_log_debug_l4("Entering...\n");

	return cg_string_getvalue(httpRes->reasonPhrase);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_response_read
****************************************/

BOOL cg_http_response_read(CgHttpResponse *httpRes, CgSocket *sock, BOOL onlyHeader)
{
	char lineBuf[CG_HTTP_READLINE_BUFSIZE];
	CgStringTokenizer *strTok;
	char *token;
	int readLen;
	
	cg_log_debug_l4("Entering...\n");

	cg_http_response_clear(httpRes);

	readLen = cg_socket_readline(sock, lineBuf, sizeof(lineBuf));
	if (readLen <= 0)
		return FALSE;

	strTok = cg_string_tokenizer_new(lineBuf, CG_HTTP_STATUSLINE_DELIM);
	if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
		cg_http_response_setversion(httpRes, cg_string_tokenizer_nexttoken(strTok));
	if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
		cg_http_response_setstatuscode(httpRes, atoi(cg_string_tokenizer_nexttoken(strTok)));
	if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE) {
		token = cg_string_tokenizer_nextalltoken(strTok);
		cg_strrtrim(token, CG_HTTP_STATUSLINE_DELIM, cg_strlen(CG_HTTP_STATUSLINE_DELIM));
		cg_http_response_setreasonphrase(httpRes, token);
	}
	cg_string_tokenizer_delete(strTok);

	cg_http_packet_read((CgHttpPacket *)httpRes, sock, onlyHeader, lineBuf, sizeof(lineBuf));
	
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_http_response_copy
****************************************/

void cg_http_response_copy(CgHttpResponse *destHttpRes, CgHttpResponse *srcHttpRes)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_response_setversion(destHttpRes, cg_http_response_getversion(srcHttpRes));
	cg_http_response_setstatuscode(destHttpRes, cg_http_response_getstatuscode(srcHttpRes));
	cg_http_response_setreasonphrase(destHttpRes, cg_http_response_getreasonphrase(srcHttpRes));

	cg_http_packet_copy((CgHttpPacket *)destHttpRes, (CgHttpPacket *)srcHttpRes);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_response_print
****************************************/

void cg_http_response_print(CgHttpResponse *httpRes)
{
	cg_log_debug_l4("Entering...\n");
	
	cg_log_debug_s("%s %d %s\n",
		cg_http_response_getversion(httpRes),
		cg_http_response_getstatuscode(httpRes),
		cg_http_response_getreasonphrase(httpRes));

	cg_http_packet_print((CgHttpPacket *)httpRes);
	
	cg_log_debug_l4("Leaving...\n");
}
