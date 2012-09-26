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
*	File: chttp_header_list.c
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#include <cybergarage/http/chttp.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_http_headerlist_new
****************************************/

CgHttpHeaderList *cg_http_headerlist_new()
{
	CgHttpHeaderList *headerList;

	cg_log_debug_l4("Entering...\n");

	headerList = (CgHttpHeaderList *)malloc(sizeof(CgHttpHeaderList));

	if ( NULL != headerList )
	{
		cg_list_header_init((CgList *)headerList);
		headerList->name = NULL;
		headerList->value = NULL;
	}

	return headerList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_headerlist_delete
****************************************/

void cg_http_headerlist_delete(CgHttpHeaderList *headerList)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_headerlist_clear(headerList);
	free(headerList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_headerlist_getheader
****************************************/

CgHttpHeader *cg_http_headerlist_get(CgHttpHeaderList *headerList, char *name)
{
	CgHttpHeader *header;
	char *headerName;
		
	cg_log_debug_l4("Entering...\n");

	if (name == NULL)
		return NULL;
		
	for (header = cg_http_headerlist_gets(headerList); header != NULL; header = cg_http_header_next(header)) {
		headerName = cg_http_header_getname(header);
		if (headerName == NULL)
			continue;
		if (cg_strcasecmp(headerName, name) == 0)
			return header;
	}
	
	return NULL;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_headerlist_setheader
****************************************/

void cg_http_headerlist_set(CgHttpHeaderList *headerList, char *name, char *value)
{
	CgHttpHeader *header;
	
	cg_log_debug_l4("Entering...\n");

	header = cg_http_headerlist_get(headerList, name);
	if (header == NULL) {
		header = cg_http_header_new();
		cg_http_headerlist_add(headerList, header);
		cg_http_header_setname(header, name);
	}
	
	cg_http_header_setvalue(header, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_headerlist_setheader
****************************************/

char *cg_http_headerlist_getvalue(CgHttpHeaderList *headerList, char *name)
{
	CgHttpHeader *header;
	
	cg_log_debug_l4("Entering...\n");

	header = cg_http_headerlist_get(headerList, name);
	if (header != NULL)
		return cg_http_header_getvalue(header);
		
	return NULL;

	cg_log_debug_l4("Leaving...\n");
}
