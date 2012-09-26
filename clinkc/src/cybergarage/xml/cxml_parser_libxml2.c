/******************************************************************
*
*	CyberXML for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006-2007 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File:	cxml_parser_libxml2.c
*
*	Revision;
*
*	08/19/05
*		- first version
*		- The functions are created using a contribution from
*		  Smolander Visa <visa.smolander@nokia.com> to CyberLink for C++.
*       09/07/05
*               - Thanks for  Smolander Visa <visa.smolander@nokia.com>
*               - Fixed some bugs to parser name spaces.
*       10/31/05
*               - Added performance measurement functionality under
*                 CG_SHOW_TIMINGS macro (not enabled by default)
*	11/18/05
*		- Fixed to use SAX parser and parse predefined XML entities
*	12/13/07 Aapo Makela
*		- Fixes to work in out-of-memory situations
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/typedef.h>

/****************************************
* Compiler Switch (BEGIN)
****************************************/

#if defined(CG_XMLPARSER_LIBXML2) || defined(TARGET_OS_IPHONE)

/****************************************
* Header Files
****************************************/

#include <cybergarage/xml/cxml.h>
#include <cybergarage/util/clog.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/parserInternals.h>

#ifdef CG_SHOW_TIMINGS
#include <sys/time.h>
#include <time.h>

extern long int cg_total_elapsed_time;
#endif

/* 10000000 */
#define UTF_RANGE1_1 (1<<7)
/* 11100000 */
#define UTF_RANGE2_1 ((1<<7) + (1<<6) + (1<<5))
/* 11110000 */
#define UTF_RANGE3_1 ((1<<7) + (1<<6) + (1<<5) + (1<<4))
/* 11111000 */
#define UTF_RANGE4_1 ((1<<7) + (1<<6) + (1<<5) + (1<<4) + (1<<3))
/* 11000000 */
#define UTF_RANGEX_2 ((1<<7) + (1<<6))

/* 00000000 */
#define UTF_RANGE1_1_R 0
/* 11000000 */
#define UTF_RANGE2_1_R ((1<<7) + (1<<6))
/* 11100000 */
#define UTF_RANGE3_1_R ((1<<7) + (1<<6) + (1<<5))
/* 11110000 */
#define UTF_RANGE4_1_R ((1<<7) + (1<<6) + (1<<5) + (1<<4))
/* 10000000 */
#define UTF_RANGEX_2_R (1<<7)

static int cg_libxml2_parsewrapper(xmlSAXHandlerPtr sax, void *user_data, const char *buffer, int size, int recovery);

static xmlEntityPtr cg_libxml2_get_entity(void *user_data, 
					  const xmlChar *name);

static void cg_libxml2_characters(void *user_data,
				  const xmlChar *ch,
				  int len);

static void cg_libxml2_start_element(void *user_data,
				     const xmlChar *name,
				     const xmlChar **attrs);
static void cg_libxml2_end_element(void *user_data,
				   const xmlChar *name);			     

static void cg_xml_force_utf8(char *data, int len);

typedef struct _CgLibxml2Data {
	CgXmlNode *rootNode;
	CgXmlNode *currNode;
} CgLibxml2Data;

enum
{
	LIBXML2_NOFLAGS = 0,
	LIBXML2_RECOVERY = 1
};

static xmlSAXHandler cg_libxml2_handler =
{
	.startElement = cg_libxml2_start_element,
	.endElement = cg_libxml2_end_element,
	.getEntity = cg_libxml2_get_entity,
	.characters = cg_libxml2_characters
};

static void cg_libxml2_start_element(void *user_data,
				     const xmlChar *name,
				     const xmlChar **attrs)
{
	cg_log_debug_l4("Entering...\n");

	CgLibxml2Data *libxml2Data;
	CgXmlNode *node;
	int n;

	libxml2Data = (CgLibxml2Data *)user_data;

	node = cg_xml_node_new();
	if (node == NULL)
	{
		/* Memory allocation failed */
		libxml2Data->currNode = NULL;
		return;
	}

	cg_xml_node_setname(node, (char *)name);

	if (attrs != NULL)
	{
		for (n = 0; attrs[n]; n += 2)
			cg_xml_node_setattribute(node, (char *)attrs[n], (char *)attrs[n+1]);
	}

	if (libxml2Data->rootNode != NULL) {
		if (libxml2Data->currNode != NULL)
			cg_xml_node_addchildnode(libxml2Data->currNode, node);
		else
			cg_xml_node_addchildnode(libxml2Data->rootNode, node);
	}
	else
		libxml2Data->rootNode = node;

	libxml2Data->currNode = node;

	cg_log_debug_l4("Leaving...\n");
}

static void cg_libxml2_end_element(void *user_data,
				   const xmlChar *name)
{
	cg_log_debug_l4("Entering...\n");

	CgLibxml2Data *libxml2Data = (CgLibxml2Data *)user_data;
	if (libxml2Data->currNode != NULL)
		libxml2Data->currNode = cg_xml_node_getparentnode(libxml2Data->currNode);

	cg_log_debug_l4("Leaving...\n");
}

static void cg_libxml2_characters(void *user_data,
				  const xmlChar *ch,
				  int len)
{
	cg_log_debug_l4("Entering...\n");

	CgLibxml2Data *libxml2Data;

	libxml2Data = (CgLibxml2Data *)user_data;

	if (libxml2Data->currNode != NULL)
		cg_xml_node_naddvalue(libxml2Data->currNode, (char *)ch, len);

	cg_log_debug_l4("Leaving...\n");
}

static xmlEntityPtr cg_libxml2_get_entity(void *user_data, const xmlChar *name) 
{
	cg_log_debug_l4("Entering...\n");

	return xmlGetPredefinedEntity(name);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_xml_parse
****************************************/

static void cg_xml_force_utf8(char *data, int len)
{
	int read=0;

        while ( read <= len )
        {
                /* First we check if byte is one byte UTF8 character */
                if ( UTF_RANGE1_1_R == ( *(data + read) & UTF_RANGE1_1 ) )
                {
                        read++;
                        continue;
                }

                /* If not then we check if byte starts two byte sequence. */
                else if ( UTF_RANGE2_1_R == ( *(data + read) & UTF_RANGE2_1 ) )
                {
                        /* We know that if this is correct two byte UTF8 char
                         * there must be at least one byte in data buffer */
                        if ( (read + 1) > len )
                        {
                                *(data + read) = '?';
                                read++;
                                continue;
                        }

                        /* We have bytes left in buffer, next we check if this two
                         * byte sequence can be decoded as two byte UTF8 char. */
                        if ( UTF_RANGEX_2_R == ( *(data + read + 1) & UTF_RANGEX_2 ) )
                        {
                                read+=2;
                                continue;
                        }

			/* Found a byte sequence which is not an UTF8 character, converting
			 * it to proper UTF8 character. */
                        else
                        {
                                *(data + read) = '?';
                                read++;
                                continue;
                        }
                }

		/* Three byte sequence check */
                else if ( UTF_RANGE3_1_R == ( *(data + read) & UTF_RANGE3_1 ) )
                {
                        /* Now we have to have at least two other bytes in buffer for this char
                         * really to be a correct UTF8 char. */
                        if ( (read + 2) > len )
                        {
                                *(data + read) = '?';
                                read++;
                                continue;
                        }

                        /* Checking if we have a valid three byte sequence */
                        if (    UTF_RANGEX_2_R == ( *(data + read + 1) & UTF_RANGEX_2 ) &&
                                UTF_RANGEX_2_R == ( *(data + read + 2) & UTF_RANGEX_2 ) )
                        {
                                read+=3;
                                continue;
                        }

			/* This byte did not start a valid three byte UTF8 character, so
			 * converting and continuing. */
                        else
                        {
                                *(data + read) = '?';
                                read++;
                                continue;
                        }
                }

                /* And same check for four byte character encoding. */
                else if ( UTF_RANGE4_1_R == ( *(data + read) & UTF_RANGE4_1 ) )
                {
			/* Again we have to have at least three extra bytes in buffer */
                        if ( (read + 3) > len )
                        {
                                *(data + read) = '?';
                                read++;
                                continue;
                        }

			/* Check next tree bytes */
                        if (    UTF_RANGEX_2_R == ( *(data + read + 1) & UTF_RANGEX_2 ) &&
                                UTF_RANGEX_2_R == ( *(data + read + 2) & UTF_RANGEX_2 ) &&
                                UTF_RANGEX_2_R == ( *(data + read + 3) & UTF_RANGEX_2 ) )
                        {
                                read+=4;
                                continue;
                        }

			/* This byte did not start a valid four byte sequence, converting
			 * and continuing. */
                        else
                        {
                                *(data + read) = '?';
                                read++;
                                continue;
                        }
                }

                /* We couldn't find one, two, three or four byte encoding sequences,
                 * so just setting current byte as unknown and continuing. */
                else
                {
                        *(data + read) = '?';
                        read++;
                        continue;
                }
        }
}

BOOL cg_xml_parse(CgXmlParser *parser, CgXmlNodeList *nodeList, char *data, int len)
{
	cg_log_debug_l4("Entering...\n");

	CgLibxml2Data libxml2Data;
	int retval;
#ifdef CG_SHOW_TIMINGS
	struct timeval start_time, end_time, elapsed_time;
	
	gettimeofday(&start_time, NULL);
#endif	
	
	libxml2Data.rootNode = NULL;
	libxml2Data.currNode = NULL;

	retval = cg_libxml2_parsewrapper(&cg_libxml2_handler, &libxml2Data, data, len, LIBXML2_NOFLAGS);
	
	switch (retval)
	{
		case XML_ERR_INVALID_CHAR:
			cg_log_debug_s("Trying to recover from error %d.\n", retval);
			
			if (libxml2Data.rootNode != NULL)
				cg_xml_node_delete(libxml2Data.rootNode);

			libxml2Data.rootNode = NULL;
			libxml2Data.currNode = NULL;

			/* Replace non utf8 characters with '?' */
			cg_xml_force_utf8(data, len);

			retval = cg_libxml2_parsewrapper(
					&cg_libxml2_handler, 
					&libxml2Data, 
					data, 
					len, 
					LIBXML2_NOFLAGS);
			break;

		default:
			break;
	}

	if ( 0 != retval )
	{
		cg_log_debug_s("LibXML error %d, not trying recovery.\n", retval);
		if (libxml2Data.rootNode != NULL)
			cg_xml_node_delete(libxml2Data.rootNode);
		return FALSE;
	}

	cg_xml_nodelist_add(nodeList, libxml2Data.rootNode);
	
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
	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

static int cg_libxml2_parsewrapper(xmlSAXHandlerPtr sax, void *user_data, const char *buffer, int size, int flags)
{
	int retval = 0; 
	xmlParserCtxtPtr ctxt;
	xmlSAXHandlerPtr oldsax = NULL;
	
	if (sax == NULL) return -1;
	
	ctxt = xmlCreateMemoryParserCtxt(buffer, size); 
	
	if (ctxt == NULL) return -1; 

	ctxt->recovery = ( flags & LIBXML2_RECOVERY )?1:0;
	
	oldsax = ctxt->sax;
	ctxt->sax = sax; 
	
	if (user_data != NULL) 
		ctxt->userData = user_data; 
	
	xmlParseDocument(ctxt); 
	
	if (ctxt->wellFormed) 
		retval = 0; 
	else 
	{ 
		if (ctxt->errNo != 0) 
			retval = ctxt->errNo; 
		else 
			retval = -1; 
	} 
	
	if (ctxt->myDoc != NULL) 
	{ 
		xmlFreeDoc(ctxt->myDoc); 
		ctxt->myDoc = NULL; 
	} 

	ctxt->sax = oldsax;
	
	xmlFreeParserCtxt(ctxt); 
	
	return retval;
}

/****************************************
* Compiler Switch (END)
****************************************/

#endif
