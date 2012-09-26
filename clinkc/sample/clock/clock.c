/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: clock.c
*
*	Revision:
*       06/07/05
*               - first release.
*
************************************************************/

#include "clock.h"

#include <stdio.h>
#include <time.h>

/****************************************
* toMonthString
****************************************/

static char *MONTH_STRING[] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec",
};

static char *toMonthString(int value)
{
	if (0 <= value && value < 12)
		return MONTH_STRING[value];
	return "";
}
	
/****************************************
* toWeekString
****************************************/

static char *WEEK_STRING[] = {
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
};

static char *toWeekString(int value)
{
	if (0 <= value && value < 7)
		return WEEK_STRING[value];
	return "";
}

/****************************************
* GetSystemTimeString
****************************************/

char *GetSystemTimeString(CgSysTime currTime, char *buf)
{
	struct tm *localTime;
			
	localTime = localtime(&currTime);
	sprintf(buf, "%s, %s %d, %02d, %02d:%02d:%02d",
		toWeekString(localTime->tm_wday),
		toMonthString(localTime->tm_mon),
		localTime->tm_mday,
		localTime->tm_year + 1900,
		localTime->tm_hour,
		localTime->tm_min,
		localTime->tm_sec);
	
	return buf;
}

/****************************************
* GetDateString
****************************************/

char *GetDateString(CgSysTime currTime, char *buf)
{
	struct tm *localTime;
			
	localTime = localtime(&currTime);

	sprintf(buf, "%s, %s %d, %02d",
		toWeekString(localTime->tm_wday),
		toMonthString(localTime->tm_mon),
		localTime->tm_mday,
		localTime->tm_year + 1900);

	return buf;
}

/****************************************
* GetTimeString
****************************************/

char *GetTimeString(CgSysTime currTime, char *buf)
{
	struct tm *localTime;
			
	localTime = localtime(&currTime);

	sprintf(buf, "%02d%s%02d",
		localTime->tm_hour,
		((localTime->tm_sec % 2) == 0) ? ":" : " ",
		localTime->tm_min);

	return buf;
}

/****************************************
* GetSecond
****************************************/

int GetSecond(CgSysTime currTime)
{
	struct tm *localTime;
			
	localTime = localtime(&currTime);

	return localTime->tm_sec;
}
