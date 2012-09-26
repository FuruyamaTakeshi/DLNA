/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) 2006 Nokia Corporation
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: ccond.c
*
*	Revision:
*
*	16-Jan-06
*		- first revision
*	09-May-08
*		- Changed cg_cond_signal() using SetEvent() instead of WaitForSingleObject for WIN32 platform.
*
******************************************************************/

#include <cybergarage/util/ccond.h>
#include <cybergarage/util/clog.h>

#if defined(WIN32)
#include <winbase.h>
#else
#include <sys/time.h>
#endif

/****************************************
* cg_cond_new
****************************************/

CgCond *cg_cond_new()
{
	CgCond *cond;

	cg_log_debug_l4("Entering...\n");

	cond = (CgCond *)malloc(sizeof(CgCond));

	if ( NULL != cond )
	{
#if defined(WIN32) && !defined(ITRON)
		cond->condID = CreateEvent(NULL, FALSE, FALSE, NULL);
#elif defined(BTRON)
	/* TODO: Add implementation */
#elif defined(ITRON) 
	/* TODO: Add implementation */
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	/* TODO: Add implementation */
#elif defined(TENGINE) && defined(PROCESS_BASE)
	/* TODO: Add implementation */
#else
		pthread_cond_init(&cond->condID, NULL);
#endif
	}

	return cond;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_cond_delete
****************************************/

BOOL cg_cond_delete(CgCond *cond)
{
	cg_log_debug_l4("Entering...\n");

#if defined(WIN32) && !defined(ITRON)
	CloseHandle(cond->condID);
#elif defined(BTRON)
	/* TODO: Add implementation */
#elif defined(ITRON)
	/* TODO: Add implementation */
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	/* TODO: Add implementation */
#elif defined(TENGINE) && defined(PROCESS_BASE)
	/* TODO: Add implementation */
#else
	pthread_cond_destroy(&cond->condID);
#endif
	free(cond);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_cond_lock
****************************************/

BOOL cg_cond_wait(CgCond *cond, CgMutex *mutex, unsigned long timeout)
{
#if defined(WIN32) && !defined(ITRON)
	DWORD timeout_s = (timeout == 0 ? INFINITE : timeout);
	cg_mutex_unlock(mutex);
	WaitForSingleObject(cond->condID, timeout_s);
	cg_mutex_lock(mutex);
#elif defined(BTRON)
	/* TODO: Add implementation */
#elif defined(ITRON)
	/* TODO: Add implementation */
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	/* TODO: Add implementation */
#elif defined(TENGINE) && defined(PROCESS_BASE)
	/* TODO: Add implementation */
#else
	struct timeval  now;
	struct timespec timeout_s;
	
	cg_log_debug_l4("Entering...\n");

	gettimeofday(&now, NULL);
	
	if (timeout < 1)
	{
		pthread_cond_wait(&cond->condID, &mutex->mutexID);
	} else {
		timeout_s.tv_sec = now.tv_sec + timeout;
		timeout_s.tv_nsec = now.tv_usec * 1000;
		pthread_cond_timedwait(&cond->condID, &mutex->mutexID, &timeout_s);
	}
#endif
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_cond_unlock
****************************************/

BOOL cg_cond_signal(CgCond *cond)
{
	BOOL success = FALSE;

	cg_log_debug_l4("Entering...\n");

#if defined(WIN32) && !defined(ITRON)
	/* TODO: Add implementation */
	/* success = (SignalObjectAndWait(cond->condID, NULL, INFINITE, FALSE) != WAIT_FAILED); */
	/* success = (WaitForSingleObject(cond->condID, INFINITE) != WAIT_FAILED); */
	success = SetEvent(cond->condID);
#elif defined(BTRON)
	/* TODO: Add implementation */
#elif defined(ITRON)
	/* TODO: Add implementation */
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	/* TODO: Add implementation */
#elif defined(TENGINE) && defined(PROCESS_BASE)
	/* TODO: Add implementation */
#else
	success = (pthread_cond_signal(&cond->condID) == 0);
#endif
	cg_log_debug_l4("Leaving...\n");

	return success;
}
