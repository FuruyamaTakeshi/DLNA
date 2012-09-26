/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Nokia Corporation 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cdictionary.c
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
* cg_dictionary_new
****************************************/

CgDictionary *cg_dictionary_new()
{
	CgDictionary *dictionaryList;

	cg_log_debug_l4("Entering...\n");

	dictionaryList = (CgDictionary *)malloc(sizeof(CgDictionary));

	if ( NULL != dictionaryList )
	{
		cg_list_header_init((CgList *)dictionaryList);
		dictionaryList->key = NULL;
		dictionaryList->value = NULL;
	}

	cg_log_debug_l4("Leaving...\n");

	return dictionaryList;
}

/****************************************
* cg_dictionary_delete
****************************************/

void cg_dictionary_delete(CgDictionary *dictionaryList)
{
	cg_log_debug_l4("Entering...\n");

	cg_dictionary_clear(dictionaryList);
	free(dictionaryList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_dictionary_getelement
****************************************/

CgDictionaryElement *cg_dictionary_getelement(CgDictionary *dir, char *key)
{
	CgDictionaryElement *dirElem;
	char *dirElemKey;

	cg_log_debug_l4("Entering...\n");

	if (NULL == dir || cg_strlen(key) <= 0 )
		return NULL;
	
	for (dirElem = cg_dictionary_gets(dir); dirElem != NULL; dirElem = cg_dictionary_element_next(dirElem)) {
			dirElemKey = cg_dictionary_element_getkey(dirElem);
			if (cg_strlen(dirElemKey) <= 0)
				continue;
			if (cg_streq(dirElemKey, key) == TRUE)
				return dirElem;
	}

	cg_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
* cg_dictionary_setvalue
****************************************/

void cg_dictionary_setvalue(CgDictionary *dir, char *key, char *value)
{
	CgDictionaryElement *dirElem;

	cg_log_debug_l4("Entering...\n");

	dirElem = cg_dictionary_getelement(dir, key);
	if (NULL == dirElem) {
		dirElem = cg_dictionary_element_new();
		cg_dictionary_add(dir, dirElem);
	}
	cg_dictionary_element_setkey(dirElem, key);
	cg_dictionary_element_setvalue(dirElem, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_dictionary_getvalue
****************************************/

char *cg_dictionary_getvalue(CgDictionary *dir, char *key)
{
	CgDictionaryElement *dirElem;

	cg_log_debug_l4("Entering...\n");

	dirElem = cg_dictionary_getelement(dir, key);
	if (NULL == dirElem)
		return NULL;

	cg_log_debug_l4("Leaving...\n");

	return cg_dictionary_element_getvalue(dirElem);
}
