/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: rss_func.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include <cybergarage/http/chttp.h>
#include <cybergarage/xml/cxml.h>

/************************************************************
* cg_http_getrestresponse
************************************************************/

#define CG_HTTP_URLLEN_MAX 512

BOOL cg_http_getrestresponse(char *url, CgString *str)
{
	CgNetURL *netUrl;
	CgHttpRequest *httpReq;
	CgHttpResponse *httpRes;
	int statusCode;
	char *content;
	long contentLen;
	char path[CG_HTTP_URLLEN_MAX];

	netUrl = cg_net_url_new();
	cg_net_url_set(netUrl, url);

	httpReq = cg_http_request_new();
	cg_http_request_setmethod(httpReq, CG_HTTP_GET);
	
	cg_strncpy(path, cg_net_url_getpath(netUrl), (CG_HTTP_URLLEN_MAX-1));
	if (cg_net_url_getquery(netUrl) != NULL) {
		cg_strncat(path, "?", (CG_HTTP_URLLEN_MAX-cg_strlen(path)-1));
		cg_strncat(path, cg_net_url_getquery(netUrl), (CG_HTTP_URLLEN_MAX-cg_strlen(path)-1));
	}
	
	cg_http_request_seturi(httpReq, path);
	cg_http_request_setcontentlength(httpReq, 0);
	httpRes = cg_http_request_post(httpReq, cg_net_url_gethost(netUrl), cg_net_url_getport(netUrl));
	
	statusCode = cg_http_response_getstatuscode(httpRes);

	if (statusCode != CG_HTTP_STATUS_OK) {
		cg_net_url_delete(netUrl);
		cg_http_request_delete(httpReq);
		return FALSE;
	}
	
	content = cg_http_response_getcontent(httpRes);
	contentLen = (long)cg_http_response_getcontentlength(httpRes);

	cg_string_setvalue(str, content);

	cg_http_request_delete(httpReq);
	cg_net_url_delete(netUrl);

	return TRUE;
}

/************************************************************
* cg_http_getrestresponse
************************************************************/

BOOL cg_http_getredirecturl(char *orgURL, char *redirectURL, int redirectURLSize)
{
	CgNetURL *netUrl;
	CgString *postUri;
	CgHttpRequest *httpReq;
	CgHttpResponse *httpRes;
	int statusCode;
	CgHttpHeader *locationHeader;
	char *locationVal;

	/**** Redirect URL ****/
	netUrl = cg_net_url_new();
	cg_net_url_set(netUrl, orgURL);
	httpReq = cg_http_request_new();
	cg_http_request_setmethod(httpReq, CG_HTTP_GET);
	postUri = cg_string_new();
	cg_string_setvalue(postUri, cg_net_url_getpath(netUrl));
	if (cg_net_url_getquery(netUrl) != NULL) {
		cg_string_addvalue(postUri, "?");
		cg_string_addvalue(postUri, cg_net_url_getquery(netUrl));
	}
	cg_http_request_seturi(httpReq, cg_string_getvalue(postUri));
	cg_string_delete(postUri);
	cg_http_request_setcontentlength(httpReq, 0);
	httpRes = cg_http_request_post(httpReq, cg_net_url_gethost(netUrl), cg_net_url_getport(netUrl));

	statusCode = cg_http_response_getstatuscode(httpRes);
	if (statusCode != 303) {
		cg_net_url_delete(netUrl);
		cg_http_request_delete(httpReq);
		return FALSE;
	}

	locationHeader = cg_http_response_getheader(httpRes, CG_HTTP_LOCATION);
	if (!locationHeader) {
		cg_net_url_delete(netUrl);
		cg_http_request_delete(httpReq);
		return FALSE;
	}
	locationVal =cg_http_header_getvalue(locationHeader);
	if (cg_strlen(locationVal) <=0 ) {
		cg_net_url_delete(netUrl);
		cg_http_request_delete(httpReq);
		return FALSE;
	}
	if (cg_strstr(locationVal, "http://") < 0) {
		cg_strncpy(redirectURL, "http://", redirectURLSize);
		cg_strncat(redirectURL, cg_net_url_gethost(netUrl), (redirectURLSize-cg_strlen(redirectURL)));
		cg_strncat(redirectURL, locationVal, (redirectURLSize-cg_strlen(redirectURL)));
	}
	else
		cg_strncpy(redirectURL, locationVal, redirectURLSize);

	cg_net_url_delete(netUrl);
	cg_http_request_delete(httpReq);

	return TRUE;
}
