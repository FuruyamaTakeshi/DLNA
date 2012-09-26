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
*	File: cargument.c
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

CgUpnpArgument *cg_upnp_argument_new()
{
	CgUpnpArgument *arg;

	cg_log_debug_l4("Entering...\n");

	arg = (CgUpnpArgument *)malloc(sizeof(CgUpnpArgument));

	if ( NULL != arg )
	{
		cg_list_node_init((CgList *)arg);
		
		arg->serviceNode = NULL;
		arg->argumentNode = NULL;
		
		arg->value = cg_string_new();
	}
	
	cg_log_debug_l4("Leaving...\n");
	
	return arg;
}

/****************************************
* cg_upnp_argument_delete
****************************************/

void cg_upnp_argument_delete(CgUpnpArgument *arg)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)arg);
	
	cg_string_delete(arg->value);
	
	cg_log_debug_l4("Leaving...\n");

	free(arg);
}
