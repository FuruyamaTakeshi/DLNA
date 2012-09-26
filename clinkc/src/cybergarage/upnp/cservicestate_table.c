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
*	File: cstataevariable_table.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cstatevariable.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_servicestatetable_new
****************************************/

CgUpnpServiceStateTable *cg_upnp_servicestatetable_new()
{
	CgUpnpServiceStateTable *statevariableTable;

	cg_log_debug_l4("Entering...\n");

	statevariableTable = (CgUpnpServiceStateTable *)malloc(sizeof(CgUpnpServiceStateTable));

	if ( NULL != statevariableTable )
		cg_list_header_init((CgList *)statevariableTable);

	cg_log_debug_l4("Leaving...\n");
	
	return statevariableTable;
}

/****************************************
* cg_upnp_servicestatetable_delete
****************************************/

void cg_upnp_servicestatetable_delete(CgUpnpServiceStateTable *statevariableTable)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)statevariableTable);
	free(statevariableTable);

	cg_log_debug_l4("Leaving...\n");
}
