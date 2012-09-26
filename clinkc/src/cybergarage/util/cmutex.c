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
*	File: cmutex.cpp
*
*	Revision:
*
*	01/17/05
*		- first revision
*
******************************************************************/

#include <cybergarage/util/cmutex.h>
#include <cybergarage/util/clog.h>

#if !defined(WINCE)
#include <errno.h>
#endif

#if !defined(CG_MUTEX_LOG_ENABLED)
#undef cg_log_debug_l4
#define cg_log_debug_l4(msg)
#endif

/****************************************
* cg_mutex_new
****************************************/

CgMutex *cg_mutex_new()
{
	CgMutex *mutex;

	cg_log_debug_l4("Entering...\n");
	
	mutex = (CgMutex *)malloc(sizeof(CgMutex));

	if ( NULL != mutex )
	{
#if defined(WIN32) && !defined(ITRON)
		mutex->mutexID = CreateMutex(NULL, FALSE, NULL);
#elif defined(BTRON)
		mutex->mutexID = cre_sem(1, SEM_EXCL);
#elif defined(ITRON) 
		T_CSEM	csem;
		csem.sematr = TA_TFIFO;
		csem.isemcnt = 1;
		csem.maxsem = 1;
		csem.name = NULL;
		mutex->mutexID = acre_sem(&csem);
#elif defined(TENGINE) && !defined(PROCESS_BASE)
		T_CSEM	csem;
		csem.exinf = 0;
		csem.sematr = TA_TFIFO | TA_FIRST;
		csem.isemcnt = 0;
		csem.maxsem = 1;
		mutex->mutexID = tk_cre_sem(&csem);
#elif defined(TENGINE) && defined(PROCESS_BASE)
		mutex->mutexID = b_cre_sem(1, SEM_EXCL);
#else
		pthread_mutex_init(&mutex->mutexID, NULL);
#endif
	}

	cg_log_debug_l4("Leaving...\n");

	return mutex;
}

/****************************************
* cg_mutex_delete
****************************************/

BOOL cg_mutex_delete(CgMutex *mutex)
{
	if (!mutex)
		return FALSE;

	cg_log_debug_l4("Entering...\n");

#if defined(WIN32) && !defined(ITRON)
	CloseHandle(mutex->mutexID);
#elif defined(BTRON)
	del_sem(mutex->mutexID);
#elif defined(ITRON)
	del_sem(mutex->mutexID);
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	tk_del_sem(mutex->mutexID);
#elif defined(TENGINE) && defined(PROCESS_BASE)
	b_del_sem(mutex->mutexID);
#else
	pthread_mutex_destroy(&mutex->mutexID);
#endif
	free(mutex);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_mutex_lock
****************************************/
#if defined(WITH_THREAD_LOCK_TRACE) && defined(__USE_ISOC99)
#include <string.h>
#include <pthread.h>

/* Contains record for every thread which has lock or is
 * waiting for lock
 */
CgLockInfo *cg_tlt_list = NULL;

/* Used to synchronize the thread lock record db access.
 */
pthread_mutex_t tlt_mutex = PTHREAD_MUTEX_INITIALIZER;

BOOL cg_mutex_lock_trace(	const char *file, 
				int line, 
				const char *function,
				CgMutex *mutex)
{
	pthread_t thid;
	int found;
	CgLockInfo *temp;

	if (!mutex)
		return FALSE;

	thid = pthread_self();
	pthread_mutex_lock(&tlt_mutex);

	found = 0;

	/* Searching existing lock record for this thread.
	 */
	temp = cg_tlt_list;
	while ( NULL != temp ) {
		if ( pthread_equal(temp->thread_id, thid) && ( temp->mutex_id == (int)mutex ) ) {
			found = 1;
			break;
		}
		temp = temp->next;
	}

	if (found)
	{
		/* Double locking detected */
		FILE *error_dump = NULL;

		system("date >>/tmp/cg_td_errors.log");
		error_dump = fopen("/tmp/cg_td_errors.log", "a");
		fprintf(stderr, "Double locking detected, locked from\n\tfile: %s\n\tline: %d\n\tfunction: %s\ntried to lock again from\n\tfile: %s\n\tline: %d\n\tfunction: %s\nfailing fast...\n",
                        temp->file,
                        temp->line,
                        temp->function,
                        file,
                        line,
                        function);
                fprintf(error_dump, "Double locking detected, locked from\n\tfile: %s\n\tline: %d\n\tfunction: %s\ntried to lock again from\n\tfile: %s\n\tline: %d\n\tfunction: %s\nfailing fast...\n",
                        temp->file,
                        temp->line,
                        temp->function,
                        file,
                        line,
                        function);
                fclose(error_dump);
                exit(-1);
        }

	/* No locks found for this thread, inserting new record.
	 */

	temp = malloc(sizeof(CgLockInfo));
	temp->thread_id = thid;
        temp->file = strdup(file);
        temp->line = line;
        temp->function = strdup(function);
	/* Using the memory address for differentiating between different
	 * mutexes... */
	temp->mutex_id = (int)mutex;

        temp->next = cg_tlt_list;
	cg_tlt_list = temp;

	pthread_mutex_unlock(&tlt_mutex);

	if ( EBUSY == pthread_mutex_trylock(&mutex->mutexID) )
	{
		/* Lock already held by someone, printing out information
		 * about thread having lock and threads waiting for lock.
		 */
		pthread_mutex_lock(&tlt_mutex);
		temp = cg_tlt_list;

		while ( NULL != temp  && ( temp->mutex_id == (int)mutex ) ) {
			FILE *error_dump = NULL;

			system("date >>/tmp/cg_td_errors.log");
			error_dump = fopen("/tmp/cg_td_errors.log", "a");
			fprintf(stderr, "Already locked from\n\tfile: %s\n\tline: %d\n\tfunction: %s\ntried to lock from\n\tfile: %s\n\tline: %d\n\tfunction: %s\ngoing to be blocked for a while...\n",
				temp->file,
				temp->line,
				temp->function,
				file,
				line,
				function);
			fprintf(error_dump, "Already locked from\n\tfile: %s\n\tline: %d\n\tfunction: %s\ntried to lock from\n\tfile: %s\n\tline: %d\n\tfunction: %s\ngoing to be blocked for a while...\n",
				temp->file,
				temp->line,
				temp->function,
				file,
				line,
				function);
			fclose(error_dump);

			temp = temp->next;
		}

		pthread_mutex_unlock(&tlt_mutex);

		/* Actual locking */
		pthread_mutex_lock(&mutex->mutexID);
	}

	return TRUE;
}

BOOL cg_mutex_unlock_trace(	const char *file, 
				int line, 
				const char *function,
				CgMutex *mutex)
{
        pthread_t thid;
        int found;
        CgLockInfo *temp, *ptemp;

		if (!mutex)
			return FALSE;

        thid = pthread_self();
        pthread_mutex_lock(&tlt_mutex);

        found = 0;

        temp = cg_tlt_list;
        ptemp = NULL;
        while ( NULL != temp ) {
                if ( pthread_equal(temp->thread_id, thid)  && ( temp->mutex_id == (int)mutex ) ) {
                        found = 1;
                        break;
                }
                ptemp = temp;
                temp = temp->next;
        }

        /* Removing lock record from list
         */
        if (found && ( NULL != ptemp ) )
        {
                ptemp->next = temp->next;
                free(temp->file);
                free(temp->function);
                free(temp);
                temp = NULL;
        }
        /* First list record handled as a special case
         */
        else if ( found && ( NULL == ptemp ) )
        {
                cg_tlt_list = temp->next;
                free(temp->file);
                free(temp->function);
                free(temp);
        }
        else
        {
                /* Double unlock */
                FILE *error_dump = NULL;

                system("date >>/tmp/cg_td_errors.log");
                error_dump = fopen("/tmp/cg_td_errors.log", "a");

                fprintf(stderr, "Unlocking from %s, (f:%s l:%d), but cannot find lock record...\n",
                        function, file, line);
                fprintf(error_dump, "Unlocking from %s, (f:%s l:%d), but cannot find lock record...\n",
                        function, file, line);
                fclose(error_dump);
                exit(-1);
        }

        pthread_mutex_unlock(&tlt_mutex);

	pthread_mutex_unlock(&mutex->mutexID);

	return TRUE;
}
#else
BOOL cg_mutex_lock(CgMutex *mutex)
{
	if (!mutex)
		return FALSE;

	cg_log_debug_l4("Entering...\n");

#if defined(WIN32) && !defined(ITRON)
	WaitForSingleObject(mutex->mutexID, INFINITE);
#elif defined(BTRON)
	wai_sem(mutex->mutexID, T_FOREVER);
#elif defined(ITRON)
	twai_sem(mutex->mutexID, TMO_FEVR);
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	tk_wai_sem(mutex->mutexID, 1, TMO_FEVR);
#elif defined(TENGINE) && defined(PROCESS_BASE)
	b_wai_sem(mutex->mutexID, T_FOREVER);
#else
	pthread_mutex_lock(&mutex->mutexID);
#endif

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_mutex_unlock
****************************************/

BOOL cg_mutex_unlock(CgMutex *mutex)
{
	if (!mutex)
		return FALSE;

	cg_log_debug_l4("Entering...\n");

#if defined(WIN32) && !defined(ITRON)
	ReleaseMutex(mutex->mutexID);
#elif defined(BTRON)
	sig_sem(mutex->mutexID);
#elif defined(ITRON)
	sig_sem(mutex->mutexID);
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	tk_sig_sem(mutex->mutexID, 1);
#elif defined(TENGINE) && defined(PROCESS_BASE)
	b_sig_sem(mutex->mutexID);
#else
	pthread_mutex_unlock(&mutex->mutexID);
#endif
	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}
#endif /* WITH_THREAD_LOCK_TRACE */
