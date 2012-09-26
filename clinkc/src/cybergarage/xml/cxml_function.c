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
*	File: cxml_function.cpp
*
*	Revision:
*
*	04/11/05
*		- first revision
*
*	10/31/05
*		- Added cg_xml_unescapechars() function for DIDL-lite parsing
*
******************************************************************/

#include <cybergarage/xml/cxml.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_xml_escapechars
****************************************/

char *cg_xml_escapechars(CgString *str)
{
	char *fromChars[5];
	char *toChars[5];
	
	cg_log_debug_l4("Entering...\n");

	fromChars[0] = "&";
	fromChars[1] = ">";
	fromChars[2] = "<";
	fromChars[3] = "\"";
	fromChars[4] = "'";
	
	toChars[0] = "&amp;";
	toChars[1] = "&gt;";
	toChars[2] = "&lt;";
	toChars[3] = "&quot;";
	toChars[4] = "&apos;";
		
	cg_log_debug_l4("Leaving...\n");
	
	return cg_string_replace(str, fromChars, toChars, (sizeof(fromChars)/sizeof(fromChars[0])));
}

char *cg_xml_unescapechars(CgString *str)
{
	char *fromChars[5];
	char *toChars[5];
	
	cg_log_debug_l4("Entering...\n");

	fromChars[0] = "&amp;";
	fromChars[1] = "&gt;";
	fromChars[2] = "&lt;";
	fromChars[3] = "&quot;";
	fromChars[4] = "&apos;";
	
	toChars[0] = "&";
	toChars[1] = ">";
	toChars[2] = "<";
	toChars[3] = "\"";
	toChars[4] = "'";
	
	cg_log_debug_l4("Leaving...\n");
	
	return cg_string_replace(str, fromChars, toChars, (sizeof(fromChars)/sizeof(fromChars[0])));
}


