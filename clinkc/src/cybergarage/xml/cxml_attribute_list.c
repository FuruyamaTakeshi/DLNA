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
*	File: chttp_attribute_list.cpp
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
* cg_xml_attributelist_new
****************************************/

CgXmlAttributeList *cg_xml_attributelist_new()
{
	CgXmlAttributeList *attrList;

	cg_log_debug_l4("Entering...\n");

	attrList = (CgXmlAttributeList *)malloc(sizeof(CgXmlAttributeList));

	if ( NULL != attrList )
	{
		cg_list_header_init((CgList *)attrList);
		attrList->name = NULL;
		attrList->value = NULL;
	}

	return attrList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_xml_attributelist_delete
****************************************/

void cg_xml_attributelist_delete(CgXmlAttributeList *attrList)
{
	cg_log_debug_l4("Entering...\n");

	cg_xml_attributelist_clear(attrList);
	free(attrList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_xml_attributelist_getattribute
****************************************/

CgXmlAttribute *cg_xml_attributelist_get(CgXmlAttributeList *attrList, char *name)
{
	CgXmlAttribute *attr;
	char *attrName;
		
	cg_log_debug_l4("Entering...\n");

	if (name == NULL)
		return NULL;
		
	for (attr = cg_xml_attributelist_gets(attrList); attr != NULL; attr = cg_xml_attribute_next(attr)) {
		attrName = cg_xml_attribute_getname(attr);
		if (attrName == NULL)
			continue;
		if (cg_strcasecmp(attrName, name) == 0)
			return attr;
	}
	
	return NULL;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_xml_attributelist_setattribute
****************************************/

void cg_xml_attributelist_set(CgXmlAttributeList *attrList, char *name, char *value)
{
	CgXmlAttribute *attr;
	
	cg_log_debug_l4("Entering...\n");

	attr = cg_xml_attributelist_get(attrList, name);
	if (attr == NULL) {
		attr = cg_xml_attribute_new();
		cg_xml_attributelist_add(attrList, attr);
		cg_xml_attribute_setname(attr, name);
	}
	
	cg_xml_attribute_setvalue(attr, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_xml_attributelist_getvalue
****************************************/

char *cg_xml_attributelist_getvalue(CgXmlAttributeList *attrList, char *name)
{
	CgXmlAttribute *attr;

	cg_log_debug_l4("Entering...\n");

	attr = cg_xml_attributelist_get(attrList, name);
	if (attr == NULL)
		return NULL;
	return cg_xml_attribute_getvalue(attr);

	cg_log_debug_l4("Leaving...\n");
}
