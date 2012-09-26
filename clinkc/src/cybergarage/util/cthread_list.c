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
*	File: cthread_list.c
*
*	Revision:
*
*	10/21/05
*		- first revision
*
******************************************************************/

#include <cybergarage/util/cthread.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_threadlist_new
****************************************/

CgThreadList *cg_threadlist_new()
{
	CgThreadList *threadList;

	cg_log_debug_l4("Entering...\n");

	threadList = (CgThreadList *)malloc(sizeof(CgThreadList));

	if ( NULL != threadList )
	{
		cg_list_header_init((CgList *)threadList);
		threadList->runnableFlag = FALSE;
		threadList->action = NULL;
		threadList->userData = NULL;
	}

	return threadList;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_threadlist_delete
****************************************/

void cg_threadlist_delete(CgThreadList *threadList)
{
	cg_log_debug_l4("Entering...\n");

	cg_threadlist_clear(threadList);
	free(threadList);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_threadlist_start
****************************************/

BOOL cg_threadlist_start(CgThreadList *threadList)
{
	CgThreadList *thread;
	
	cg_log_debug_l4("Entering...\n");

	for (thread = cg_threadlist_gets(threadList); thread != NULL; thread = cg_thread_next(thread))
		cg_thread_start(thread);

	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_threadlist_stop
****************************************/

BOOL cg_threadlist_stop(CgThreadList *threadList)
{
	CgThreadList *thread;
	
	cg_log_debug_l4("Entering...\n");

	for (thread = cg_threadlist_gets(threadList); thread != NULL; thread = cg_thread_next(thread))
		cg_thread_stop(thread);

	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}
