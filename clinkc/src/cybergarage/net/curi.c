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
*	03/09/05
*		- first revision
*	08/16/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Change cg_net_uri_new() to set "/" as the default path.
*		- Changed cg_net_uri_set() to check a ":" and "@" in the password.
*	10/30/05
*		- Thanks for Smolander Visa <visa.smolander@nokia.com>
*		- Changed cg_net_uri_set(). Even a slash wasn't found in URL, 
*		  so it represents only a  part of path. Put slash infront and 
*		  append rest of the string.
*	10/31/05
*		- cg_net_uri_set: Accepts URI (as a path), which doesn't have
*		  a slash.
*	01/16/07 Aapo Makela
*		- Added URI escaping and unescaping functions
*	03/20/07
*		- Added a parameter, CgString, to return the result safety and the implementation without curl for the following functions.
*		  cg_net_uri_unescapestring() and cg_net_uri_escapestring().
*		- Added a define, CG_USE_NET_URI_ESCAPESTRING_SKIP,in cg_net_uri_escapestring() and disabled the function not to skip only the first path.
*		-Changed cg_upnp_device_httprequestrecieved() using the new cg_net_uri_unescapestring().
*		- Added the following functions.
*		  cg_net_uri_isreservedchar()
*		  cg_net_uri_isunreservedchar()
*		  cg_net_uri_isescapechar()
*		  cg_net_uri_isalphanumchar()
*		- Fixed cg_net_uri_escapestring() to use curl_escape() instead of curl_unescape().
*		- Fixed cg_net_uri_unescapestring() to use curl_unescape() instead of curl_escape().
*	05-Jan-08  Satoshi Konno <skonno@cybergarage.org>
*		- Added cg_net_uri_rebuild() and cg_net_url_rebuild().
*
******************************************************************/

#include <cybergarage/net/curi.h>
#include <cybergarage/util/cstring.h>
#include <cybergarage/util/clog.h>

#if defined(CG_HTTP_CURL)
#include <curl/curl.h>
#endif

/****************************************
* cg_net_uri_new
****************************************/

CgNetURI *cg_net_uri_new()
{
	CgNetURI *uri;
	
	cg_log_debug_l4("Entering...\n");

	uri = (CgNetURI *)malloc(sizeof(CgNetURI));

	if  (NULL != uri)
	{
		uri->uri = cg_string_new();
		uri->protocol = cg_string_new();
		uri->user = cg_string_new();
		uri->password = cg_string_new();
		uri->host = cg_string_new();
		uri->port = 0;
		uri->path = cg_string_new();
		uri->query = cg_string_new();
		uri->fragment = cg_string_new();
		uri->request = NULL;
		uri->queryDictionary = NULL;
		
		/**** Thanks for Theo Beisch (2005/08/25) ****/
		cg_string_setvalue(uri->path, CG_NET_URI_DEFAULT_PATH);
	}
		
	cg_log_debug_l4("Leaving...\n");

	return uri;
}

/****************************************
* cg_net_uri_delete
****************************************/

void cg_net_uri_delete(CgNetURI *uri)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_delete(uri->uri);
	cg_string_delete(uri->protocol);
	cg_string_delete(uri->user);
	cg_string_delete(uri->password);
	cg_string_delete(uri->host);
	cg_string_delete(uri->path);
	cg_string_delete(uri->query);
	cg_string_delete(uri->fragment);

	if (uri->request != NULL)
		cg_string_delete(uri->request);
	if (uri->queryDictionary != NULL)
		cg_dictionary_delete(uri->queryDictionary);

	free(uri);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_uri_clear
****************************************/

void cg_net_uri_clear(CgNetURI *uri)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_clear(uri->uri);
	cg_string_clear(uri->protocol);
	cg_string_clear(uri->user);
	cg_string_clear(uri->password);
	cg_string_clear(uri->host);
	uri->port = 0;
	cg_string_clear(uri->path);
	cg_string_clear(uri->query);
	cg_string_clear(uri->fragment);
	if (uri->request != NULL) 
	{
		cg_string_delete(uri->request);
		uri->request = NULL;
	}

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_uri_set
****************************************/

void cg_net_uri_setvalue(CgNetURI *uri, char *value)
{
	char *protocol;
	int uriLen;
	int currIdx;
	int protoIdx;
	int atIdx;
	int colonIdx;
	int shashIdx;
	char *host;
	int eblacketIdx;
	CgString *hostStr;
	CgString *portStr;
	int hostLen;
	int sharpIdx;
	int questionIdx;
	int queryLen;
	
	cg_log_debug_l4("Entering...\n");

	uriLen = cg_strlen(value);
	cg_net_uri_clear(uri);
	cg_net_uri_seturi(uri, value);
		
	currIdx = 0;
	
	/*** Protocol ****/
	protoIdx = cg_strstr(value, CG_NET_URI_PROTOCOL_DELIM);
	if (0 < protoIdx) {
		cg_string_setnvalue(uri->protocol, value,  protoIdx);
		currIdx += protoIdx + cg_strlen(CG_NET_URI_PROTOCOL_DELIM);
	}

	/*** User (Password) ****/
	atIdx = cg_strstr(value+currIdx, CG_NET_URI_USER_DELIM);
	if (0 < atIdx) {
		colonIdx = cg_strstr(value+currIdx, CG_NET_URI_COLON_DELIM);
		/**** Thanks for Theo Beisch (2005/08/25) ****/
		if (0 < colonIdx && colonIdx<atIdx) {
			cg_string_setnvalue(uri->user, value+currIdx,  colonIdx);
			cg_string_setnvalue(uri->password, value+currIdx+colonIdx+1,  atIdx-(colonIdx+1));
		}
		else 
			cg_string_setnvalue(uri->user, value+currIdx,  atIdx - currIdx);
		currIdx += atIdx + 1;
	}

	/*** Host (Port) ****/
	shashIdx = cg_strstr(value+currIdx, CG_NET_URI_SLASH_DELIM);
	if (0 < shashIdx)
		cg_string_setnvalue(uri->host, value+currIdx,  shashIdx);
	else if (cg_net_uri_isabsolute(uri) == TRUE)
		cg_string_setnvalue(uri->host, value+currIdx, cg_strlen(value) - currIdx);
	host = cg_net_uri_gethost(uri);
	colonIdx = cg_strrchr(host, CG_NET_URI_COLON_DELIM, 1);
	eblacketIdx = cg_strrchr(host, CG_NET_URI_EBLACET_DELIM, 1);
	if (0 < colonIdx && eblacketIdx < colonIdx) {
		hostStr = cg_string_new();
		cg_string_setvalue(hostStr, host);
		hostLen = cg_string_length(hostStr);
		/**** host ****/
		cg_string_setnvalue(uri->host, cg_string_getvalue(hostStr),  colonIdx);
		host = cg_net_uri_gethost(uri);
		if (0 < hostLen) {
			if (host[0] == '[' && host[hostLen-1] == ']')
				cg_string_setnvalue(uri->host, cg_string_getvalue(hostStr)+1,  colonIdx-2);
		}
		/**** port ****/
		portStr = cg_string_new();
		cg_string_setnvalue(portStr, cg_string_getvalue(hostStr)+colonIdx+1,  hostLen- colonIdx-1);
		uri->port = atoi(cg_string_getvalue(portStr));
		cg_string_delete(portStr);
		cg_string_delete(hostStr);
	}
	else {
		uri->port = CG_NET_URI_KNKOWN_PORT;
		protocol = cg_net_uri_getprotocol(uri);
		if (cg_strcmp(protocol, CG_NET_URI_PROTOCOL_HTTP) == 0)
			uri->port = CG_NET_URI_DEFAULT_HTTP_PORT;
		if (cg_strcmp(protocol, CG_NET_URI_PROTOCOL_FTP) == 0)
			uri->port = CG_NET_URI_DEFAULT_FTP_PORT;
	}
	
	if (shashIdx > 0) currIdx += shashIdx;
	
	/*
		Handle relative URL
	*/
	if (cg_net_uri_isabsolute(uri) == FALSE)
	{
		cg_string_addvalue(uri->path, value);
		
	} else {
		/* First set path simply to the rest of URI */
		cg_string_setnvalue(uri->path, value+currIdx,  uriLen-currIdx);
	}
		
	/**** Path (Query/Fragment) ****/
	sharpIdx = cg_strstr(value+currIdx, CG_NET_URI_SHARP_DELIM);
	if (0 < sharpIdx) {
		cg_string_setnvalue(uri->path, value+currIdx,  sharpIdx);
		cg_string_setnvalue(uri->fragment, value+currIdx+sharpIdx+1,  uriLen-(currIdx+sharpIdx+1));
	}
	questionIdx = cg_strstr(value+currIdx, CG_NET_URI_QUESTION_DELIM);
	if (0 < questionIdx) {
		cg_string_setnvalue(uri->path, value+currIdx,  questionIdx);
		queryLen = uriLen-(currIdx+questionIdx+1);
		if (0 < sharpIdx)
			queryLen -= uriLen - (currIdx+sharpIdx);
		cg_string_setnvalue(uri->query, value+currIdx+questionIdx+1,  queryLen);
	}

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
 * cg_net_uri_rebuild
 ****************************************/

void cg_net_uri_rebuild(CgNetURI *uri)
{
	char portStr[32];
	char *path;
	
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(uri->uri, cg_net_uri_getprotocol(uri));
	cg_string_addvalue(uri->uri, CG_NET_URI_PROTOCOL_DELIM);
	cg_string_addvalue(uri->uri, cg_net_uri_gethost(uri));
	cg_string_addvalue(uri->uri, CG_NET_URI_COLON_DELIM);
	cg_string_addvalue(uri->uri, cg_int2str(cg_net_uri_getport(uri), portStr, sizeof(portStr)));
	if (0 < cg_strlen(cg_net_uri_getpath(uri))) {
		path = cg_net_uri_getpath(uri);
		if (path[0] != '/')
			cg_string_addvalue(uri->uri, CG_NET_URI_SLASH_DELIM);
		cg_string_addvalue(uri->uri, cg_net_uri_getpath(uri));
		if (cg_strchr(cg_net_uri_getpath(uri), "?", 1) == -1 && 0 < cg_strlen(cg_net_uri_getquery(uri))) {
			cg_string_addvalue(uri->uri, CG_NET_URI_QUESTION_DELIM);
			cg_string_addvalue(uri->uri, cg_net_uri_getquery(uri));
		}
	}
	
	cg_log_debug_l4("Leaving...\n");
}

/****************************************
 * cg_net_uri_getvalue
 ****************************************/

char *cg_net_uri_getvalue(CgNetURI *uri)
{
	cg_net_uri_rebuild(uri);
	return cg_string_getvalue(uri->uri);
}

/****************************************
* cg_net_uri_getrequest
****************************************/

char *cg_net_uri_getrequest(CgNetURI *uri)
{
	cg_log_debug_l4("Entering...\n");

	if (cg_net_uri_hasquery(uri) == FALSE)
	{
		return cg_net_uri_getpath(uri);
	}
	
	if (uri->request == NULL) uri->request = cg_string_new();

	cg_string_setvalue(uri->request, cg_net_uri_getpath(uri));
	cg_string_addvalue(uri->request, CG_NET_URI_QUESTION_DELIM);
	cg_string_addvalue(uri->request, cg_net_uri_getquery(uri));
	
	cg_log_debug_l4("Leaving...\n");

	return cg_string_getvalue(uri->request);
}

/****************************************
* cg_net_uri_isequivalent
****************************************/

BOOL cg_net_uri_isequivalent(char *url, char *relative_url)
{
	CgNetURI *u;
	char *path;
	BOOL ret;

	u = cg_net_uri_new();
	cg_net_uri_set(u, url);

	path = cg_net_uri_getpath(u);
	ret = cg_strcmp(path, relative_url) == 0;

	cg_net_uri_delete(u);
	return ret;
}

/****************************************
* cg_net_uri_isescapedstring
****************************************/

BOOL cg_net_uri_isescapedstring(char *buf, int bufSize)
{
	int idx;

	cg_log_debug_l4("Entering...\n");

	if (bufSize < 1) bufSize = cg_strlen(buf) + 1;
	
	/* We assume that the URI string is escaped, if it has at least one
	   escaped character */
	
	/* First check that there is escaping character */
	idx = cg_strstr(buf, CG_NET_URI_ESCAPING_CHAR);
	if (idx < 0 || idx > bufSize - 4) return FALSE;
	
	/* Check that the next two characters are HEX */
	if (((buf[idx+1] >= '0' && buf[idx+1] <= '9') ||
	     (buf[idx+1] >= 'a' && buf[idx+1] <= 'f') ||
	     (buf[idx+1] >= 'A' && buf[idx+1] <= 'F')) &&
	    ((buf[idx+2] >= '0' && buf[idx+2] <= '9') ||
	     (buf[idx+2] >= 'a' && buf[idx+2] <= 'f') ||
	     (buf[idx+2] >= 'A' && buf[idx+2] <= 'F')))
		return TRUE;
	
	cg_log_debug_l4("Leaving...\n");

	return FALSE;
}

/****************************************
* cg_net_uri_unescapestring
****************************************/

char *cg_net_uri_escapestring(char *buf, int bufSize, CgString *retBuf)
{
#if defined(CG_HTTP_CURL)
	char *tmp;
#else
	int n;
	unsigned char c;
	char hexChar[4];
#endif

	cg_log_debug_l4("Entering...\n");

	if (!retBuf)
		return NULL;

#if defined(CG_HTTP_CURL)
	tmp = (bufSize < 1)?curl_escape(buf, 0):curl_escape(buf, bufSize);
	if (tmp == NULL)
	{
		cg_log_debug_s("Memory allocation problem!\n");
		return NULL;
	}
	cg_string_addvalue(retBuf, tmp);
	curl_free(tmp);
#else
	if (bufSize < 1)
		bufSize = cg_strlen(buf) + 1;

	for (n=0; n<bufSize; n++) {
		c = (unsigned char)buf[n];
		if (!cg_net_uri_isalphanumchar(c)) {
#if defined(HAVE_SNPRINTF)
			snprintf(hexChar, sizeof(hexChar), "%%%02X", c);
#else
			sprintf(hexChar, "%%%02X", c);
#endif
			cg_string_naddvalue(retBuf, hexChar, 3);
		}
		else
			cg_string_naddvalue(retBuf, buf+n, 1);
	}

#endif
	
	cg_log_debug_l4("Leaving...\n");

	return cg_string_getvalue(retBuf);
}

/****************************************
* cg_net_uri_escapestring
****************************************/

char *cg_net_uri_unescapestring(char *buf, int bufSize, CgString *retBuf)
{
#if defined(CG_HTTP_CURL)
	char *tmp;
#else
	int n;
	char hexStr[3];
	long hex;
	unsigned char c;
#endif
	int idx = 0;
#if defined(CG_USE_NET_URI_ESCAPESTRING_SKIP)
	int tmpIdx = 0;
#endif
	
	cg_log_debug_l4("Entering...\n");

	if (!retBuf)
		return NULL;

	/* Check if URI is already escaped */
	if (cg_net_uri_isescapedstring(buf + idx, bufSize) == TRUE) 
		return buf;
	
	/* We can safely assume that the non-path part is already escaped */
#if defined(CG_USE_NET_URI_ESCAPESTRING_SKIP)
	idx = cg_strstr(buf, CG_NET_URI_PROTOCOL_DELIM);
	if (idx > 0)
	{
		idx = idx + cg_strlen(CG_NET_URI_PROTOCOL_DELIM);
		tmpIdx = cg_strstr(buf + idx, CG_NET_URI_SLASH_DELIM);
		if (tmpIdx > 0)
			idx += tmpIdx + cg_strlen(CG_NET_URI_SLASH_DELIM);
	} else {
		idx = 0;
	}
#endif

	if (bufSize < 1)
		bufSize = cg_strlen(buf) + 1;	
	
#if defined(CG_HTTP_CURL)
	tmp = curl_unescape(buf + idx, 0);
	if (tmp == NULL)
		return NULL;
	cg_string_addvalue(retBuf, tmp);
	cg_log_debug_s("%s ==> %s\n", buf + idx, tmp);
	curl_free(tmp);
#else
	for (n=0; n<bufSize;) {
		c = (unsigned char)buf[n];
		if (buf[n] == '%' && cg_net_uri_isalphanumchar(buf[n+1]) && cg_net_uri_isalphanumchar(buf[n+2])) {
			hexStr[0] = buf[n+1];
			hexStr[1] = buf[n+2];
			hexStr[2] = '\0';
			hex = strtol(hexStr, NULL, 16);
			c = (unsigned char)hex;
			n += 3;
		}
		else
			n++;
		cg_string_naddvalue(retBuf, (char *)&c, 1);
	}
#endif
	
	cg_log_debug_l4("Leaving...\n");

	return cg_string_getvalue(retBuf);
}

/****************************************
* cg_net_uri_getupnpbasepath
****************************************/

char *cg_net_uri_getupnpbasepath(CgNetURI *locationURL)
{
        char *path, *c;
        int i;

        path = cg_strdup(cg_net_uri_getpath(locationURL));

	cg_log_debug_s("Mangling url string: %s\n", path);
        
	i = cg_strlen(path);

        if ( 0 >= i )
        {
                cg_log_debug("No base path, doing nothing.\n");
                return NULL;
        }

        /* Truncating out the "file name" from path */
        for ( c=( path + --i); 0<=i; c=( path + --i ))
                if ( '/' == *c )
                {
                        *( path + i + 1 ) = '\0';
			cg_log_debug_s("Truncating string from place %d\n", i);
                        break;
                }

	cg_log_debug_s("url string after mangling: %s\n", path);

        return path;
}

/****************************************
* cg_net_uri_isreservedchar
****************************************/

BOOL cg_net_uri_isreservedchar(char c)
{
	if (c == ';' || c ==  '/' || c == '?' || c == ':' || c == '@' || c ==  '&' || c ==  '=' || c ==  '+' || c == '$' || c ==  ',')
		return TRUE;
	return FALSE;
}

/****************************************
* cg_net_uri_isalphanumchar
****************************************/

BOOL cg_net_uri_isalphanumchar(char c)
{
	if ('a' <= c && c <= 'z')
		return TRUE;
	if ('A' <= c && c <= 'Z')
		return TRUE;
	if ('0' <= c && c <= '9')
		return TRUE;
	return FALSE;
}

/****************************************
* cg_net_uri_isunreservedchar
****************************************/

BOOL cg_net_uri_isunreservedchar(char c)
{
	if (cg_net_uri_isalphanumchar(c))
		return TRUE;
	if (c == '-' || c == '_' || c == '.' || c == '!' || c == '~' || c == '*' || c == '^' || c == '(' || c == ')')
		return TRUE;
	return FALSE;
}

/****************************************
* cg_net_uri_isescapedchar
****************************************/

BOOL cg_net_uri_isescapechar(char c)
{
	if (cg_net_uri_isreservedchar(c))
		return FALSE;
	if (cg_net_uri_isunreservedchar(c))
		return FALSE;
	return TRUE;
}

/****************************************
* cg_net_uri_getquerydictionary
****************************************/

CgDictionary *cg_net_uri_getquerydictionary(CgNetURI *uri)
{
	char *query;
	int queryOffset;
	int eqIdx, ampIdx;
	CgString *paramName;
	CgString *paramValue;

	if (NULL == uri->queryDictionary)
		uri->queryDictionary = cg_dictionary_new();

	paramName = cg_string_new();
	paramValue = cg_string_new();

	query = cg_net_uri_getquery(uri);
	queryOffset = 0;

	eqIdx = cg_strstr(query, "=");
	while (0 < eqIdx) {
		ampIdx = cg_strstr(query + queryOffset, "&");
		if (ampIdx <= 0) {
			ampIdx = cg_strstr(query + queryOffset, "#");
			if (ampIdx <= 0)
				ampIdx = cg_strlen(query + queryOffset);
		}
		if (ampIdx <= eqIdx)
			break;
		cg_string_setnvalue(paramName, query + queryOffset, eqIdx);
		cg_string_setnvalue(paramValue, query + queryOffset + eqIdx + 1, (ampIdx - eqIdx -1));
		cg_dictionary_setvalue(uri->queryDictionary, cg_string_getvalue(paramName), cg_string_getvalue(paramValue));
		queryOffset += ampIdx + 1;
		eqIdx = cg_strstr(query + queryOffset, "=");
	}

	cg_string_delete(paramName);
	cg_string_delete(paramValue);

	return uri->queryDictionary;
}
