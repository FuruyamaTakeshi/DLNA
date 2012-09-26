/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cdictionary_elem.c
*
*	Revision:
*
*	05/26/09
*		- first revision
*
******************************************************************/

#include <cybergarage/util/cdictionary.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_dictionary_element_new
****************************************/

CgDictionaryElement *cg_dictionary_element_new()
{
	CgDictionaryElement *dirElem;

	cg_log_debug_l4("Entering...\n");

	dirElem = (CgDictionaryElement *)malloc(sizeof(CgDictionaryElement));

	cg_log_debug_s("Creating dirElem data into %p\n", dirElem);

	if ( NULL != dirElem )
	{
		cg_list_node_init((CgList *)dirElem);
		dirElem->key = cg_string_new();
		dirElem->value = cg_string_new();
	}

	cg_log_debug_l4("Leaving...\n");

	return dirElem;
}

/****************************************
* cg_dictionary_element_delete
****************************************/

BOOL cg_dictionary_element_delete(CgDictionaryElement *dirElem)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)dirElem);

	if (dirElem->key)
		cg_string_delete(dirElem->key);
	if (dirElem->value)
		cg_string_delete(dirElem->value);
	free(dirElem);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}
