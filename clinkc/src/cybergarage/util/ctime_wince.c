/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license with patent exclusion,
*       see file COPYING.
*
*	File: ctime_wince.cpp
*
*	Revision:
*
*	01/17/06
*		- first revision
*
******************************************************************/

#if defined(WINCE)
#include <windows.h>
#include <time.h>
#endif

/****************************************
* time
****************************************/

#if defined(WINCE)

time_t time( time_t *timep)
{ 
	static time_t localt = 0; 
	SYSTEMTIME sysTime; 
	FILETIME fileTime; 
	ULARGE_INTEGER fileTime64; 
  
	if (timep == NULL )
		timep = &localt; 
  
	GetSystemTime(&sysTime ); 
	if (SystemTimeToFileTime(&sysTime, &fileTime) ) { 
		memcpy(&fileTime64, &fileTime, sizeof(FILETIME)); 
		fileTime64.QuadPart -= 0x19db1ded53e8000; 
		fileTime64.QuadPart /= 10000000; 
		*timep = (time_t)fileTime64.QuadPart; 
	} 
  
	return *timep; 
} 

#endif

/****************************************
* time
****************************************/

#if defined(WINCE)

struct tm *localtime(const time_t *timep)
{
	FILETIME fileTime; 
	ULARGE_INTEGER fileTime64; 
	SYSTEMTIME sysTime; 
	static struct tm localtm; 
	
	fileTime64.QuadPart = *timep * 10000000;
	fileTime64.QuadPart += 0x19db1ded53e8000;
	memcpy(&fileTime, &fileTime64, sizeof(FILETIME)); 

	if (FileTimeToSystemTime(&fileTime, &sysTime)) {
		localtm.tm_sec = sysTime.wSecond;
		localtm.tm_min = sysTime.wMinute;
		localtm.tm_hour = sysTime.wHour;
		localtm.tm_mday = sysTime.wDay;
		localtm.tm_mon = sysTime.wMonth - 1;
		localtm.tm_year = sysTime.wYear - 1900;
		localtm.tm_wday = sysTime.wDayOfWeek;
		localtm.tm_yday = 0;
		localtm.tm_isdst = 0;
	}

	return &localtm;
}

#endif
