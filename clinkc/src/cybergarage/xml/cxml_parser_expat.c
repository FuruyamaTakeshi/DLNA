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
*	File: cxml_parser_expat.c
*
*	Revision:
*
*	02/07/05
*		- first revision
*
*	10/31/05
*		- Added performance measurement functionality under
*		  CG_SHOW_TIMINGS macro (not enabled by default)
*
*	03/30/06 Theo Beisch
*		- ignore whitespace character data (in search of memory...)
*		- added DEBUG_XML configurations
*	05/09/08
*		-  Fixed cg_xml_parse() not to abort when the data is null or the length less than zero.
*	08/05/30
*		-  Added support to include <expat/expat.h>
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/typedef.h>

/****************************************
*	Compiler Switch (BEGIN)
****************************************/

#if !defined(CG_XMLPARSER_LIBXML2) && !defined(TARGET_OS_IPHONE)

/****************************************
* Header Files
****************************************/

#include <cybergarage/xml/cxml.h>
#include <cybergarage/util/clog.h>

#if defined(WIN32)
#define XML_STATIC
#endif

#if !defined(WIN32)
#define XMLCALL
#endif

#if defined(HAVE_EXPAT_EXPAT_H)
#include <expat/expat.h>
#else
#include <expat.h>
#endif

#if defined DEBUG_XML
static int indent = 0;
#endif

#ifdef CG_SHOW_TIMINGS
#include <sys/time.h>
#include <time.h>

extern long int cg_total_elapsed_time;
#endif

/****************************************
* cg_xml_parse (Expat)
****************************************/

typedef struct _CgExpatData {
	CgXmlNode *rootNode;
	CgXmlNode *currNode;
} CgExpatData;

static void XMLCALL cg_expat_element_start(void *userData, const char *el, const char **attr)
{
	CgExpatData *expatData;
	CgXmlNode *node;
	int n;
#if defined DEBUG_XML
	int out,j;
	char* outbuf;
	char* lpout;	
#endif

	cg_log_debug_l4("Entering...\n");

#if defined DEBUG_XML
	outbuf = (char*) malloc(4096);
	lpout = outbuf;	
	outbuf[4095] = (char)0;
	for (out = 0; out < indent; out++) outbuf[out] = (char)' ';
	lpout += out;
	for (j = 0; attr[j]; j += 2) {
		 lpout += sprintf(lpout,"%s='%s'", attr[j], attr[j + 1]);
	} 
	lpout[0] = 0;
	printf("%8x XML Node start %s - %d Attribute %s\n",userData,el,j/2,outbuf);
	free(outbuf);
	++indent;
#endif
	//memdiags_memlist_report_unmarkedsize();

	expatData = (CgExpatData *)userData;

	node = cg_xml_node_new();
	cg_xml_node_setname(node, (char *)el);

	for (n = 0; attr[n]; n += 2)
		cg_xml_node_setattribute(node, (char *)attr[n], (char *)attr[n+1]);

	if (expatData->rootNode != NULL) {
		if (expatData->currNode != NULL)
			cg_xml_node_addchildnode(expatData->currNode, node);
		else
			cg_xml_node_addchildnode(expatData->rootNode, node);
	}
	else
		expatData->rootNode = node;

	expatData->currNode = node;

	cg_log_debug_l4("Leaving...\n");
}

static void XMLCALL cg_expat_element_end(void *userData, const char *el)
{
	CgExpatData *expatData;

	cg_log_debug_l4("Entering...\n");

	expatData = (CgExpatData *)userData;

#if defined DEBUG_XML
	//printf("%8x XML end %s\n",userData,el);
	--indent;
#endif
	//memdiags_memlist_report_unmarkedsize();
	if (expatData->currNode != NULL)
		expatData->currNode = cg_xml_node_getparentnode(expatData->currNode);

	cg_log_debug_l4("Leaving...\n");
}

static void XMLCALL cg_expat_character_data(void *userData, const XML_Char *s, int len)
{
	CgExpatData *expatData;
#if defined XML_IGNORE_WHITESPACE
	int i;
#endif

	cg_log_debug_l4("Entering...\n");

	//	printf("%8x XML data len=%-3d now=%d %s d[0]0x%x\n",userData, len, cg_strlen(out->value), out->value, (char)(out->value[0]));

#if defined XML_IGNORE_WHITESPACE
	for (i=0;i<len;i++){
		if (!isspace((char)s[i])) {
#endif
			expatData = (CgExpatData *)userData;
			if (expatData->currNode != NULL)
				cg_xml_node_naddvalue(expatData->currNode, (char *)s, len);
#if defined XML_IGNORE_WHITESPACE
			break;
		} 
	}
#endif
}

BOOL cg_xml_parse(CgXmlParser *parser, CgXmlNodeList *nodeList, char *data, int len)
{
#if defined DEBUG_XML_RESULT
	CgString* resdata = NULL;
#endif
	XML_Parser p;
	CgExpatData expatData;
#ifdef CG_SHOW_TIMINGS
	struct timeval start_time, end_time, elapsed_time;
#endif	
	
	cg_log_debug_l4("Entering...\n");

#ifdef CG_SHOW_TIMINGS
	gettimeofday(&start_time, NULL);
#endif	

	if (!data || len <= 0)
		return FALSE;

	p = XML_ParserCreate(NULL);
	if (!p)
		return FALSE;
	/* Fix to get expat parser to work with DLink-routers */
	if (data[len-1] == 0) len--;
	
	expatData.rootNode = NULL;
	expatData.currNode = NULL;
	XML_SetUserData(p, &expatData);
	XML_SetElementHandler(p, cg_expat_element_start, cg_expat_element_end);
	XML_SetCharacterDataHandler(p, cg_expat_character_data);

	parser->parseResult = XML_Parse(p, data, len, 1);
	XML_ParserFree(p);

	if (parser->parseResult == 0 /*XML_STATUS_ERROR*/) {
		if (expatData.rootNode != NULL)
			cg_xml_node_delete(expatData.rootNode);
#if defined DEBUG_XML_RESULT
			resdata = cg_string_new();
			cg_string_naddvalue(resdata,data,len);
			printf("XML parse Error on data %s\n time used = %ds\n",
				cg_string_getvalue(resdata),
				time(NULL)-startTime);
			cg_string_delete(resdata);
#endif
			return FALSE;
	}

	cg_xml_nodelist_add(nodeList, expatData.rootNode);

#ifdef CG_SHOW_TIMINGS
	gettimeofday(&end_time, NULL);
	timersub(&end_time, &start_time, &elapsed_time);
cg_log_debug_s("Parsing XML completed. Elapsed time: "
	       "%ld msec\n", ((elapsed_time.tv_sec*1000) + 
			      (elapsed_time.tv_usec/1000)));
	cg_total_elapsed_time += (elapsed_time.tv_sec*1000000)+
				 (elapsed_time.tv_usec);
cg_log_debug_s("Total elapsed time: %ld msec\n", cg_total_elapsed_time / 1000);
#endif	

#if defined DEBUG_XML_RESULT
			resdata = cg_string_new();
			cg_string_naddvalue(resdata,data,len);
			printf("XML parse success - time used %ds\n",time(NULL)-startTime);
			cg_string_delete(resdata);
#endif

	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* Compiler Switch (END)
****************************************/

#endif
