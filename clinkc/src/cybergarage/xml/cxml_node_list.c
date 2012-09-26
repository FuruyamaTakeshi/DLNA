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
*	File: chttp_node_list.cpp
*
*	Revision:
*
*	02/07/05
*		- first revision
*
******************************************************************/

#include <cybergarage/xml/cxml.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_xml_nodelist_new
****************************************/

CgXmlNodeList *cg_xml_nodelist_new()
{
	CgXmlNodeList *nodeList;

	cg_log_debug_l4("Entering...\n");

	nodeList = (CgXmlNodeList *)malloc(sizeof(CgXmlNodeList));

	if ( NULL != nodeList )
	{
		cg_list_header_init((CgList *)nodeList);

		nodeList->name = NULL;
		nodeList->value = NULL;
		nodeList->attrList = NULL;
		nodeList->nodeList = NULL;
		nodeList->parentNode = NULL;
		nodeList->userData = NULL;
		nodeList->userDataDestructorFunc = NULL;
	}

	cg_log_debug_l4("Leaving...\n");

	return nodeList;
}

/****************************************
* cg_xml_nodelist_delete
****************************************/

void cg_xml_nodelist_delete(CgXmlNodeList *nodeList)
{
	cg_log_debug_l4("Entering...\n");

	cg_xml_nodelist_clear(nodeList);
	free(nodeList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_xml_nodelist_getbyname
****************************************/

CgXmlNode *cg_xml_nodelist_getbyname(CgXmlNodeList *nodeList, char *name)
{
	CgXmlNode *node;
	char *nodeName;
		
	cg_log_debug_l4("Entering...\n");

	if (name == NULL)
		return NULL;
		
	for (node = cg_xml_nodelist_gets(nodeList); node != NULL; node = cg_xml_node_next(node)) {
		nodeName = cg_xml_node_getname(node);
		if (nodeName == NULL)
			continue;
		if (cg_strcasecmp(nodeName, name) == 0)
			return node;
	}
	
	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
* cg_xml_nodelist_getbyxpath
****************************************/

CgXmlNode *cg_xml_nodelist_getbyxpath(CgXmlNodeList *nodeList, char *name)
{
	CgXmlNode *node;
	char *nodeName;
		
	cg_log_debug_l4("Entering...\n");

	if (name == NULL)
		return NULL;
		
	for (node = cg_xml_nodelist_gets(nodeList); node != NULL; node = cg_xml_node_next(node)) {
		nodeName = cg_xml_node_getname(node);
		if (nodeName == NULL)
			continue;
		if (cg_strcasecmp(nodeName, name) == 0)
			return node;
	}
	
	cg_log_debug_l4("Leaving...\n");

	return NULL;
}
