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
*	File: chttp_attribute.cpp
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
* cg_xml_attribute_new
****************************************/

CgXmlAttribute *cg_xml_attribute_new()
{
	CgXmlAttribute *attr;

	cg_log_debug_l4("Entering...\n");

	attr = (CgXmlAttribute *)malloc(sizeof(CgXmlAttribute));

	if ( NULL != attr )
	{
		cg_list_node_init((CgList *)attr);
		attr->name = cg_string_new();
		attr->value = cg_string_new();
	}
	return attr;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_xml_attribute_delete
****************************************/

void cg_xml_attribute_delete(CgXmlAttribute *attr)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)attr);
	cg_string_delete(attr->name);
	cg_string_delete(attr->value);
	free(attr);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_xml_attribute_setname
****************************************/

void cg_xml_attribute_setname(CgXmlAttribute *attr, char *name)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(attr->name, name);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_xml_attribute_getname
****************************************/

char *cg_xml_attribute_getname(CgXmlAttribute *attr)
{
	cg_log_debug_l4("Entering...\n");

	return cg_string_getvalue(attr->name);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_xml_attribute_isname
****************************************/

BOOL cg_xml_attribute_isname(CgXmlAttribute *attr, char *name)
{
	cg_log_debug_l4("Entering...\n");

	return ((cg_strcmp(cg_xml_attribute_getname(attr), name) == 0) ? TRUE : FALSE);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_xml_attribute_setvalue
****************************************/

void cg_xml_attribute_setvalue(CgXmlAttribute *attr, char *value)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(attr->value, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_xml_attribute_setvalue
****************************************/

char *cg_xml_attribute_getvalue(CgXmlAttribute *attr)
{
	cg_log_debug_l4("Entering...\n");

	return cg_string_getvalue(attr->value);

	cg_log_debug_l4("Leaving...\n");
}
