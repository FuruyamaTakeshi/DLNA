/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cargument_list.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cargument.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_argument_new
****************************************/

CgUpnpArgumentList *cg_upnp_argumentlist_new()
{
	CgUpnpArgument *argumentList;

	cg_log_debug_l4("Entering...\n");

	argumentList = (CgUpnpArgument *)malloc(sizeof(CgUpnpArgument));

	if ( NULL != argumentList )
		cg_list_header_init((CgList *)argumentList);

	return argumentList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_argument_delete
****************************************/

void cg_upnp_argumentlist_delete(CgUpnpArgumentList *argumentList)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_clear((CgList*)argumentList, (CG_LIST_DESTRUCTORFUNC)cg_upnp_argument_delete);
	free(argumentList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_argumentlist_get
****************************************/

CgUpnpArgument *cg_upnp_argumentlist_get(CgUpnpArgumentList *argList, char *name)
{
	CgUpnpArgument *arg;
	char *argName;
	
	cg_log_debug_l4("Entering...\n");

	if (name == NULL)
		return NULL;
		
	for (arg = cg_upnp_argumentlist_gets(argList); arg != NULL; arg = cg_upnp_argument_next(arg)) {
		argName = cg_upnp_argument_getname(arg);
		if (argName == NULL)
			continue;
		if (cg_strcasecmp(argName, name) == 0)
			return arg;
	}

	return NULL;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_argumentlist_set
****************************************/

void cg_upnp_argumentlist_set(CgUpnpArgumentList *argList, CgUpnpArgumentList *srcArgList)
{
	CgUpnpArgument *arg;
	CgUpnpArgument *srcArg;
	char *srcArgName;
	
	cg_log_debug_l4("Entering...\n");

	for (srcArg = cg_upnp_argumentlist_gets(srcArgList); srcArg != NULL; srcArg = cg_upnp_argument_next(srcArg)) {
		srcArgName = cg_upnp_argument_getname(srcArg);
		arg = cg_upnp_argumentlist_get(argList, srcArgName);
		if (arg == NULL)
			continue;
		cg_upnp_argument_setvalue(arg, cg_upnp_argument_getvalue(srcArg));
	}

	cg_log_debug_l4("Leaving...\n");
}
