/******************************************************************
*
*	CyberXML for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cxml_parser.c
*
*	Revision:
*
*	02/07/05
*		- first revision
*
******************************************************************/

#include <cybergarage/xml/cxml.h>
#include <cybergarage/http/chttp.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_xml_parser_new
****************************************/

CgXmlParser *cg_xml_parser_new()
{
	CgXmlParser *parser;

	cg_log_debug_l4("Entering...\n");

	parser = (CgXmlParser *)malloc(sizeof(CgXmlParser));

	if ( NULL != parser )
	{
		parser->parseResult = FALSE;
	}
	return parser;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_xml_parser_delete
****************************************/

void cg_xml_parser_delete(CgXmlParser *parser)
{
	cg_log_debug_l4("Entering...\n");

	free(parser);

	cg_log_debug_l4("Leaving...\n");
}
