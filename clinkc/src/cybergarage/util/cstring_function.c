/******************************************************************
*
* CyberUtil for C
*
* Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006-2007 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
* File: cstring_function.c
*
* Revision:
*
* 01/25/05
*  - first revision
* 11/11/05
*  - Added cg_longlong2str() and cg_str2longlong().
* 03/20/06 Theo Beisch
*  - WINCE support
*  - added cg_strtrimwhite() - trims all whitespace, not just ' '
* 03/18/07
*  - Changed the following functions to use CgInt64.
*    cg_longlong2str()
* 10/22/07 Aapo Makela
*  - Added NULL check to cg_strtrimwhite()
* 08/04/09
*  - Added cg_float2str() and cg_double2str().
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cybergarage/util/cstring.h>
#include <cybergarage/util/clog.h>

#include <ctype.h>

#if defined (WINCE)
//Theo Beisch - needed for _i64toa_s support - 
// strangely just with the VS2005 ce environment 
// evc4 is only happy without :-(
#include <altcecrt.h>
#endif

#if !defined(NORTiAPI)
#include <string.h>
#endif

/****************************************
* cg_strdup
****************************************/

char *cg_strdup(char *str)
{
#if !defined(HAVE_STRDUP)
 char *cpStrBuf;
#endif

	/* cg_log_debug_l5("Entering...\n"); */

	if (str == NULL)
		return NULL;

#if defined(HAVE_STRDUP)
	return strdup(str);
#else
	cpStrBuf = (char *)malloc(strlen(str)+1);
	if ( NULL != cpStrBuf )
		strcpy(cpStrBuf, str);
	return cpStrBuf;
#endif

	/* cg_log_debug_l5("Leaving...\n"); */
}

/****************************************
* cg_strlen
****************************************/

int cg_strlen(char *str)
{
 cg_log_debug_l5("Entering...\n");

 return (str == NULL) ? 0 : strlen(str);

 cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_strcpy
****************************************/

char *cg_strcpy(char *dest, char *src)
{
 cg_log_debug_l5("Entering...\n");

 return strcpy(dest, src);

 cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_strcat
****************************************/

char *cg_strcat(char *dest, char *src)
{
 cg_log_debug_l5("Entering...\n");

 return strcat(dest, src);

 cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_strcmp
****************************************/

int cg_strcmp(char *str1, char *str2)
{
 cg_log_debug_l5("Entering...\n");

 if (str1 == NULL)
  return -1;
 if (str2 == NULL)
  return 1;
 return strcmp(str1, str2);

 cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_strncmp
****************************************/

int cg_strncmp(char *str1, char *str2, int nchars)
{
 cg_log_debug_l5("Entering...\n");

 if (str1 == NULL)
                return -1;
        if (str2 == NULL)
                return 1;

 return strncmp(str1, str2, nchars);

 cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_strcasecmp
****************************************/

int cg_strcasecmp(char *str1, char *str2)
{
 cg_log_debug_l5("Entering...\n");

	if (str1 == NULL || str2 == NULL) return -1;
#if !defined(WIN32)
	return strcasecmp(str1, str2);
#else
	for (; toupper(*str1) == toupper(*str2); str1++, str2++) {
		if (*str1 == '\0')
			return 0;
	}
	return *str1 - *str2;
#endif

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_streq
****************************************/

BOOL cg_streq(char *str1, char *str2)
{
	cg_log_debug_l5("Entering...\n");

	if (str1 == NULL || str2 == NULL) return FALSE;
	
	return ((cg_strcmp(str1, str2) == 0) ? TRUE : FALSE);

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_strcaseeq
****************************************/

BOOL cg_strcaseeq(char *str1, char *str2)
{
	cg_log_debug_l5("Entering...\n");

	if (str1 == NULL || str2 == NULL) return FALSE;
	
	return ((cg_strcasecmp(str1, str2) == 0) ? TRUE : FALSE);

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_strstr
****************************************/

int cg_strstr(char *haystack, char *needle)
{
	char *strPos;

	cg_log_debug_l5("Entering...\n");

	if (haystack == NULL || needle == NULL)
		return -1;
	strPos = strstr(haystack, needle);
	if (strPos == NULL)
		return -1;
	return (strPos - haystack);

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_strchr
****************************************/

int cg_strchr(char *str, char *chars, int nchars)
{
	int strLen;
	int i, j;
	
	cg_log_debug_l5("Entering...\n");

	if (str == NULL || chars == NULL)
		return -1;
		
	strLen = cg_strlen(str);
	for (i=0; i<strLen; i++) {
		for (j=0; j<nchars; j++) {
			if (str[i] == chars[j])
				return i;
		}		
	}
	
	return -1;

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_strrchr
****************************************/

int cg_strrchr(char *str, char *chars, int nchars)
{
	int strLen;
	int i, j;
	
	cg_log_debug_l5("Entering...\n");

	if (str == NULL || chars == NULL)
		return -1;
		
	strLen = cg_strlen(str);
	for (i=(strLen-1); 0<=i; i--) {
		for (j=0; j<nchars; j++) {
			if (str[i] == chars[j])
				return i;
		}		
	}
	
	return -1;

}

/****************************************
* cg_strtrimwhite
****************************************/

char *cg_strtrimwhite(char *str)
{
	int strLen, i;
	strLen = cg_strlen(str);
	if (strLen == 0) return str;
	for (i=(strLen-1); 0<=i; i--) {
		if (isspace(str[i])) {
			strLen--;
		} 
	}
	for (i=0 ; i<strLen ; i++) {
		if (!isspace(str[i])) break;
	}

	if (i>0) memmove(str,str+i,strLen-i);

	str[strLen] = 0;
	return str;
}

/****************************************
* cg_strtrim
****************************************/

char *cg_strtrim(char *str, char *delim, int ndelim)
{
	cg_log_debug_l5("Entering...\n");

	if (str == NULL || delim == NULL) return NULL;

	cg_strrtrim(str, delim, ndelim);
	return cg_strltrim(str, delim, ndelim);

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_strltrim
****************************************/

char *cg_strltrim(char *str, char *delim, int ndelim)
{
	int strLen, i, j;
	
	cg_log_debug_l5("Entering...\n");

	strLen = cg_strlen(str);
	for (i=0; i<strLen; i++) {
		BOOL hasDelim = FALSE;
		for (j=0; j<ndelim; j++) {
			if (str[i] == delim[j]) {
				hasDelim = TRUE;
				break;
			}
		}
		if (hasDelim == FALSE)
			return (str + i);
	}
	
	return (str + strLen);

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_strrtrim
****************************************/

char *cg_strrtrim(char *str, char *delim, int ndelim)
{
	int strLen, i, j;
	
	cg_log_debug_l5("Entering...\n");

	strLen = cg_strlen(str);
	for (i=(strLen-1); 0<=i; i--) {
		BOOL hasDelim = FALSE;
		for (j=0; j<ndelim; j++) {
			if (str[i] == delim[j]) {
				hasDelim = TRUE;
				str[i] = '\0';
				break;
			}
		}
		if (hasDelim == FALSE)
			break;
	}
	
	return str;

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_strncpy
****************************************/

char *cg_strncpy(char *str1, char *str2, size_t cnt)
{
	cg_log_debug_l5("Entering...\n");

	strncpy(str1, str2, cnt);
	
	cg_log_debug_l5("Leaving...\n");

	return str1;
}

/****************************************
* cg_strncat
****************************************/

char *cg_strncat(char *str1, char *str2, size_t cnt)
{
	int str1Len;
	cg_log_debug_l5("Entering...\n");

	str1Len = cg_strlen(str1);
	
	cg_log_debug_l5("Leaving...\n");
	
	return cg_strncpy((str1 + str1Len), str2, cnt);

}

/****************************************
* cg_int2str
****************************************/

char *cg_int2str(int value, char *buf, int bufSize)
{
 cg_log_debug_l5("Entering...\n");

#if defined(HAVE_SNPRINTF)
 snprintf(buf, bufSize, "%d", value);
#else
 sprintf(buf, "%d", value);
#endif

 cg_log_debug_l5("Leaving...\n");

 return buf;
}

/****************************************
* cg_long2str
****************************************/

char *cg_long2str(long value, char *buf, int bufSize)
{
 cg_log_debug_l5("Entering...\n");

#if defined(HAVE_SNPRINTF)
 snprintf(buf, bufSize, "%ld", value);
#else
 sprintf(buf, "%ld", value);
#endif

 cg_log_debug_l5("Leaving...\n");

 return buf;
}

/****************************************
* cg_longlong2str
****************************************/

#if defined(__USE_ISOC99) || defined(HAVE_LONGLONG)
char *cg_longlong2str(CgInt64 value, char *buf, int bufSize)
{
#if defined(HAVE_SNPRINTF)
 snprintf(buf, bufSize, "%lld", value);
#else
 sprintf(buf, "%lld", value);
#endif
 return buf;
}
#elif (defined(WIN32) && !defined(WINCE))
char *cg_longlong2str(CgInt64 value, char *buf, int bufSize)
{
 return _i64toa(value, buf, 10);
}
#elif (defined (WIN32) && defined (WINCE))
//theo beisch CE supports only safe version of _i64toa
char *cg_longlong2str(CgInt64 value, char *buf, int bufSize)
{
 _i64toa_s(value, buf, bufSize, 10);
 return buf;
}
#endif

/****************************************
* cg_float2str
****************************************/

char *cg_float2str(float value, char *buf, int bufSize)
{
 cg_log_debug_l5("Entering...\n");

#if defined(HAVE_SNPRINTF)
 snprintf(buf, bufSize, "%f", value);
#else
 sprintf(buf, "%f", value);
#endif

 cg_log_debug_l5("Leaving...\n");

 return buf;
}

/****************************************
* cg_double2str
****************************************/

char *cg_double2str(double value, char *buf, int bufSize)
{
 cg_log_debug_l5("Entering...\n");

#if defined(HAVE_SNPRINTF)
 snprintf(buf, bufSize, "%lf", value);
#else
 sprintf(buf, "%lf", value);
#endif

 cg_log_debug_l5("Leaving...\n");

 return buf;
}

