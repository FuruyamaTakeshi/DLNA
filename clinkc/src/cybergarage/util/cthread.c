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
*	File: cthread.cpp
*
*	Revision:
*
*	01/17/05
*		- first revision
*	09/11/05
*		- Thanks for Visa Smolander <visa.smolander@nokia.com>
*		- Changed cg_thread_start() to set the flag at first.
*	10/31/05
*		- Added a signal handler to block all signals to posix threads
*	02/12/06 Theo Beisch
*		- added WINCE support
*	03/10/06 tb
*		- added 'self delete' for thread on real exit
*		  (WINCE thread termination does not work reliably)
*		- added some debug configurations
*	06/13/07 Fabrice Fontaine Orange
*		- Fixed cg_thread_start(). Threads used to answer UPnP requests are created
*		   in joinable state but the main thread doesn't call pthread_join on them.
*		   So, they are kept alive until the end of the program. By creating them
*		   in detached state, they are correctly clean up.
*		- Fixed cg_thread_stop_with_cond() to wait one second for thread termination instead of using pthread_join(). 
*
******************************************************************/

#if !defined (WIN32) && !defined(WINCE)
#include <signal.h>
#endif

#include <cybergarage/util/cthread.h>
#include <cybergarage/util/ctime.h>
#include <cybergarage/util/clog.h>
#include <string.h>

/* Private function prototypes */
static void sig_handler(int sign);

/****************************************
* Thread Function
****************************************/

#if defined(WIN32) && !defined (WINCE) && !defined(ITRON)
static DWORD WINAPI Win32ThreadProc(LPVOID lpParam)
{
	CgThread *thread;

	cg_log_debug_l4("Entering...\n");

	thread = (CgThread *)lpParam;
	if (thread->action != NULL)
		thread->action(thread);
	
	return 0;
}
#elif defined(WINCE)
static DWORD WINAPI Win32ThreadProc(LPVOID lpParam)
{
	CgThread *thread = (CgThread *)lpParam;

	//Theo Beisch: make sure we're runnable
	//thread->runnableFlag = TRUE;
	//(moved to start() with Visa Smolander's mod)

	thread->isRunning = TRUE;

#if defined DEBUG_LOCKS
	memdiags_tlist_addthread(thread);
#endif

	if (thread->action != NULL) {
#if defined DEBUG
		printf ("#real Thr=%X hNd=%X lpP=%X %s start\n",thread,thread->hThread,lpParam , thread->friendlyName);
#endif
		thread->action(thread);
	}
#if defined DEBUG
		printf ("** REAL ExitThread(0)*\n"); 
		printf ("*2 Thread ret4close 0x%Xh\n",GetCurrentThreadId());
		printf ("*3 Thread %X %s \n",thread, thread->friendlyName);
#endif
	thread->isRunning = FALSE;
	if (thread->deletePending) {
		cg_thread_delete(thread);
	}
	//proper friendly thread exit for WINCE
	cg_thread_exit(0);
	//dummy - compiler wants a return statement
	return 0;
}
#elif defined(BTRON)
static VOID BTronTaskProc(W param)
{
	cg_log_debug_l4("Entering...\n");

	CgThread *thread = (CgThread *)param;
	if (thread->action != NULL)
		thread->action(thread);
	ext_tsk();

	cg_log_debug_l4("Leaving...\n");
}
#elif defined(ITRON)
static TASK ITronTaskProc(int param)
{
	cg_log_debug_l4("Entering...\n");

	T_RTSK rtsk;
	CgThread *thread;
	if (ref_tsk(TSK_SELF, &rtsk) != E_OK)
		return;
	thread = (CgThread *)rtsk.exinf;
	if (thread->action != NULL)
		thread->action(thread);
	exd_tsk();

	cg_log_debug_l4("Leaving...\n");
}
#elif defined(TENGINE) && !defined(PROCESS_BASE)
static VOID TEngineTaskProc(INT stacd, VP param)
{
	cg_log_debug_l4("Entering...\n");

	CgThread *thread = (CgThread *)param;
	if (thread->action != NULL)
		thread->action(thread);
	tk_exd_tsk();

	cg_log_debug_l4("Leaving...\n");
}
#elif defined(TENGINE) && defined(PROCESS_BASE)
static VOID TEngineProcessBasedTaskProc(W param)
{
	cg_log_debug_l4("Entering...\n");

	CgThread *thread = (CgThread *)param;
	if (thread->action != NULL)
		thread->action(thread);
	b_ext_tsk();

	cg_log_debug_l4("Leaving...\n");
}
#else

/* Key used to store self reference in (p)thread global storage */
static pthread_key_t cg_thread_self_ref;
static pthread_once_t cg_thread_mykeycreated = PTHREAD_ONCE_INIT;

static void cg_thread_createkey()
{
	pthread_key_create(&cg_thread_self_ref, NULL);
}

CgThread *cg_thread_self()
{
	return (CgThread *)pthread_getspecific(cg_thread_self_ref);
}

static void *PosixThreadProc(void *param)
{
	cg_log_debug_l4("Entering...\n");

	sigset_t set;
	struct sigaction actions;
	CgThread *thread = (CgThread *)param;

	/* SIGQUIT is used in thread deletion routine
	 * to force accept and recvmsg to return during thread
	 * termination process. */
	sigfillset(&set);
	sigdelset(&set, SIGQUIT);
	pthread_sigmask(SIG_SETMASK, &set, NULL);
	
	memset(&actions, 0, sizeof(actions));
	sigemptyset(&actions.sa_mask);
	actions.sa_flags = 0;
	actions.sa_handler = sig_handler;
	sigaction(SIGQUIT, &actions, NULL);

	pthread_once(&cg_thread_mykeycreated, cg_thread_createkey);
	pthread_setspecific(cg_thread_self_ref, param);

	if (thread->action != NULL) 
		thread->action(thread);
	
	cg_log_debug_l4("Leaving...\n");
	
	return 0;
}
#endif

/****************************************
* cg_thread_new
****************************************/

CgThread *cg_thread_new()
{
	CgThread *thread;

	cg_log_debug_l4("Entering...\n");

	thread = (CgThread *)malloc(sizeof(CgThread));

	cg_log_debug_s("Creating thread data into %p\n", thread);

	if ( NULL != thread )
	{
		cg_list_node_init((CgList *)thread);
		
		thread->runnableFlag = FALSE;
		thread->action = NULL;
		thread->userData = NULL;
	}

#if defined (WINCE)
	thread->hThread = NULL;
	//WINCE trial result: default sleep value to keep system load down
	thread->sleep = CG_THREAD_MIN_SLEEP;
	thread->isRunning = FALSE;
	thread->deletePending = FALSE;
#if defined DEBUG
	strcpy(thread->friendlyName,"-");
#endif //DEBUG
#endif //WINCE
	cg_log_debug_l4("Leaving...\n");

	return thread;
}

/****************************************
* cg_thread_delete
****************************************/

BOOL cg_thread_delete(CgThread *thread)
{
#if defined WINCE
	BOOL stop = FALSE;

	cg_log_debug_l4("Entering...\n");

	if ((thread->hThread==NULL) ||
		((thread->isRunning) && (stop = cg_thread_stop(thread) == TRUE)) ||	
		(thread->isRunning == FALSE)) {

#if defined DEBUG
		printf("***** Delete entered handle=%x isRunning=%d stopResult=%d \n",thread->hThread, thread->isRunning, stop);
		printf("***** Delete and free for Thread %X %s\n",thread, thread->friendlyName);
#endif				

		if (thread->hThread!=NULL) CloseHandle(thread->hThread);
		cg_list_remove((CgList *)thread);
#if defined DEBUG_MEM
		memdiags_tlist_removethread(thread);
#endif
		free(thread);
		return TRUE;
	}

#if defined DEBUG
	printf("***** Stop failed for Thread %X %s - marking thread for selfDelete\n",thread, thread->friendlyName);
#endif
	//setting this will cause the real thread exit to call delete() again
	thread->deletePending = TRUE;

	cg_log_debug_l4("Leaving...\n");

	return FALSE;
} //WINCE
#else //all except WINCE

	cg_log_debug_l4("Entering...\n");

	if (thread->runnableFlag == TRUE) 
		cg_thread_stop(thread);

	cg_thread_remove(thread);
	
	free(thread);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}
#endif

/****************************************
* cg_thread_start
****************************************/

BOOL cg_thread_start(CgThread *thread)
{
	cg_log_debug_l4("Entering...\n");

	/**** Thanks for Visa Smolander (09/11/2005) ****/
	thread->runnableFlag = TRUE;

#if defined(WIN32) && !defined(WINCE) && !defined(ITRON)
	thread->hThread = CreateThread(NULL, 0, Win32ThreadProc, (LPVOID)thread, 0, &thread->threadID);
#elif defined (WINCE)
	{

	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	thread->deletePending = FALSE;
	thread->hThread = CreateThread(&saAttr, 0, Win32ThreadProc, (LPVOID)thread, 0, &thread->threadID);
	}
#elif defined(BTRON)
	P_STATE pstate;
	prc_sts(0, &pstate, NULL);
	thread->taskID = cre_tsk(BTronTaskProc, pstate.priority, (W)thread);
	if (thread->taskID < 0) {
		thread->runnableFlag = FALSE;
		return FALSE;
	}
#elif defined(ITRON)
	T_CTSK ctsk = {TA_HLNG,  (VP_INT)thread, ITronTaskProc, 6, 512, NULL, NULL};
	thread->taskID = acre_tsk(&ctsk);
	if (thread->taskID < 0) {
		thread->runnableFlag = FALSE;
		return FALSE;
	}
	if (sta_tsk(thread->taskID, 0) != E_OK) {
		thread->runnableFlag = FALSE;
		del_tsk(thread->taskID);
		return FALSE;
	}
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	T_CTSK ctsk = {(VP)thread, TA_HLNG, TEngineTaskProc,10, 2048};
	thread->taskID = tk_cre_tsk(&ctsk);
	if (thread->taskID < E_OK) {
		thread->runnableFlag = FALSE;
		return FALSE;
	}
	if (tk_sta_tsk(thread->taskID, 0) < E_OK) {
		thread->runnableFlag = FALSE;
		tk_del_tsk(thread->taskID);
		return FALSE;
	}
#elif defined(TENGINE) && defined(PROCESS_BASE)
	P_STATE pstate;
	b_prc_sts(0, &pstate, NULL);
	thread->taskID = b_cre_tsk(TEngineProcessBasedTaskProc, pstate.priority, (W)thread);
	if (thread->taskID < 0) {
		thread->runnableFlag = FALSE;
		return FALSE;
	}
#else
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	/* MODIFICATION Fabrice Fontaine Orange 24/04/2007
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE); */
	/* Bug correction : Threads used to answer UPnP requests are created */
	/* in joinable state but the main thread doesn't call pthread_join on them. */
	/* So, they are kept alive until the end of the program. By creating them */
	/* in detached state, they are correctly clean up */
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
#ifdef STACK_SIZE
	/* Optimization : not we can set STACK_SIZE attribute at compilation time */
	/* to specify thread stack size */
    pthread_attr_setstacksize (&thread_attr,STACK_SIZE);
#endif
	/* MODIFICATION END Fabrice Fontaine Orange 24/04/2007 */
	if (pthread_create(&thread->pThread, &thread_attr, PosixThreadProc, thread) != 0) {
		thread->runnableFlag = FALSE;
		pthread_attr_destroy(&thread_attr);
		return FALSE;
	}
	pthread_attr_destroy(&thread_attr);
#endif
	
	cg_log_debug_l4("Leaving...\n");
	
	return TRUE;
}

/****************************************
* cg_thread_stop
****************************************/

BOOL cg_thread_stop(CgThread *thread)
{
	return cg_thread_stop_with_cond(thread, NULL);
}

BOOL cg_thread_stop_with_cond(CgThread *thread, CgCond *cond)
{
#if defined (WINCE)
	int i, j;
	BOOL result;
	DWORD dwExitCode;
#endif

	cg_log_debug_l4("Entering...\n");

	cg_log_debug_s("Stopping thread %p\n", thread);

	if (thread->runnableFlag == TRUE) {
		thread->runnableFlag = FALSE;
		if (cond != NULL) {
			cg_cond_signal(cond);
		}
#if defined(WIN32) && !defined (WINCE) && !defined(ITRON)
		TerminateThread(thread->hThread, 0);
		WaitForSingleObject(thread->hThread, INFINITE);
		//tb: this will create a deadlock if the thread is on a blocking socket
#elif defined (WINCE)
		// Theo Beisch: while the above code apparently works under WIN32 (NT/XP) 
		// TerminateThread as brute force is not recommended by M$
		// (see API) and actually crashes WCE
		// WINCE provides no safe means of terminating a thread, 
		// so we can only mark the cg_thread (context) for later deletion and 
		// do the delete(thread) cleanup on return of the Win32ThreadProc.
		// Accordingly we simulate the OK exit here as a "look ahead" (what a hack ;-) )
		for (i=0; i<CG_THREAD_SHUTDOWN_ATTEMPTS; ++i){
#if defined (DEBUG)
			printf("# thread stop mainloop %X %s %d. try\n",thread,thread->friendlyName,i+1);
#endif
			j=0;
			if (result = GetExitCodeThread(thread->hThread,&dwExitCode)) {
				if (dwExitCode != STILL_ACTIVE) {
#if defined (DEBUG)
					printf("Thread %X %s ended graceful: xCode=%d\n",thread,thread->friendlyName,dwExitCode);
#endif
					return TRUE;
				} 
			}
			cg_wait(CG_THREAD_MIN_SLEEP);
		}
		// ok - if everything up to here failed
#if defined DEBUG
		if (dwExitCode){
			printf ("Thread %X - %s has not yet terminated - exit code %x \n",thread,thread->friendlyName,dwExitCode);
		}
#endif

		if (dwExitCode)
			return FALSE;

		return TRUE;
		//end WINCE
#elif defined(BTRON)
		ter_tsk(thread->taskID);
#elif defined(ITRON)
		ter_tsk(thread->taskID);
		del_tsk(thread->taskID);
#elif defined(TENGINE) && !defined(PROCESS_BASE)
		tk_ter_tsk(thread->taskID);
		tk_del_tsk(thread->taskID);
#elif defined(TENGINE) && defined(PROCESS_BASE)
		b_ter_tsk(thread->taskID);
#else
		cg_log_debug_s("Killing thread %p\n", thread);
		#if  defined(TARGET_OS_MAC) || defined(TARGET_OS_IPHONE)
		pthread_kill(thread->pThread, 0);
		#else
		 pthread_kill(thread->pThread, SIGQUIT);
		#endif
		/* MODIFICATION Fabrice Fontaine Orange 24/04/2007
		cg_log_debug_s("Thread %p signalled, joining.\n", thread);
		pthread_join(thread->pThread, NULL);
		cg_log_debug_s("Thread %p joined.\n", thread); */
		/* Now we wait one second for thread termination instead of using pthread_join */
		cg_sleep(CG_THREAD_MIN_SLEEP);
		/* MODIFICATION END Fabrice Fontaine Orange 24/04/2007 */
#endif
	}

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_thread_sleep
****************************************/
// Theo Beisch
// Added this to improve external thread control 
// by having a finer timer tick granularity

#if defined (WINCE)
void cg_thread_sleep(CgThread *thread) {
	CgSysTime until;
#if defined DEBUG_MEM
	printf("###### Going to sleep - elapsed since last sleep = %d\n",memdiags_getelapsedtime(thread->hThread));
#endif
	until = cg_getcurrentsystemtime() + (thread->sleep)/1000;
	while ((cg_getcurrentsystemtime()<until) && thread->runnableFlag){
		cg_wait(0);
	}
}
#endif

/****************************************
* cg_thread_exit (friendly exit) 
****************************************/
// Theo Beisch
// to be called from the thread's loop only

#if defined (WINCE)
VOID cg_thread_exit(DWORD exitCode) {
	ExitThread(exitCode);	
}
#endif

/****************************************
* cg_thread_restart
****************************************/

BOOL cg_thread_restart(CgThread *thread)
{
	cg_log_debug_l4("Entering...\n");

	cg_thread_stop(thread);
	cg_thread_start(thread);
	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_thread_isrunnable
****************************************/

BOOL cg_thread_isrunnable(CgThread *thread)
{
	cg_log_debug_l4("Entering...\n");

	return thread->runnableFlag;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_thread_setaction
****************************************/

void cg_thread_setaction(CgThread *thread, CG_THREAD_FUNC func)
{
	cg_log_debug_l4("Entering...\n");

	thread->action = func;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_thread_setuserdata
****************************************/

void cg_thread_setuserdata(CgThread *thread, void *value)
{
	cg_log_debug_l4("Entering...\n");
	
	thread->userData = value;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_thread_getuserdata
****************************************/

void *cg_thread_getuserdata(CgThread *thread)
{
	cg_log_debug_l4("Entering...\n");

	return thread->userData;

	cg_log_debug_l4("Leaving...\n");
}

/* Private helper functions */

static void sig_handler(int sign)
{
	cg_log_debug_s("Got signal %d.\n", sign);
	return;
}
