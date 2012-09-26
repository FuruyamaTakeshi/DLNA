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
*	File: cstring_tokenizer.c
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#include <cybergarage/util/cstring.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_string_tokenizer_new
****************************************/

CgStringTokenizer *cg_string_tokenizer_new(char *value, char *delim)
{
	CgStringTokenizer *strToken;

	cg_log_debug_l4("Entering...\n");

	strToken = (CgStringTokenizer *)malloc(sizeof(CgStringTokenizer));

	if ( NULL != strToken )
	{
#if defined(NOUSE_ZERO_COPY)
		strToken->value = cg_strdup(value);
		strToken->delim = cg_strdup(delim);
#else
		strToken->value = value;
		strToken->delim = delim;
#endif
		strToken->delimCnt = cg_strlen(strToken->delim);
		strToken->nextStartPos = 0;
		strToken->lastPos = cg_strlen(value) - 1;
		strToken->currToken = NULL;
		strToken->nextToken = NULL;
		cg_string_tokenizer_nexttoken(strToken);
	}

	return strToken;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_string_tokenizer_delete
****************************************/

void cg_string_tokenizer_delete(CgStringTokenizer *strToken)
{
	cg_log_debug_l4("Entering...\n");

#if defined(NOUSE_ZERO_COPY)
	free(strToken->value);
	free(strToken->delim);
#endif
	free(strToken);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_string_tokenizer_hasmoretoken
****************************************/

BOOL cg_string_tokenizer_hasmoretoken(CgStringTokenizer *strToken)
{
	cg_log_debug_l4("Entering...\n");

	return strToken->hasNextTokens;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_string_tokenizer_nexttoken
****************************************/

char *cg_string_tokenizer_nexttoken(CgStringTokenizer *strToken)
{
	int tokenCnt;
	int i, j;
	
	cg_log_debug_l4("Entering...\n");

	strToken->currToken = strToken->nextToken;
	strToken->nextToken = NULL;
	strToken->hasNextTokens = FALSE;
	strToken->repToken = '\0';
	
	tokenCnt = 0;
	for (i=strToken->nextStartPos; i<=strToken->lastPos; i++) {
		BOOL isDelimChar = FALSE;
		for (j=0; j<strToken->delimCnt; j++) {
			if (strToken->value[i] == strToken->delim[j]) {
				isDelimChar = TRUE;
				if (tokenCnt == 0) {
					strToken->nextStartPos = i + 1;
					break;
				}
				strToken->hasNextTokens = TRUE;
				strToken->repToken = strToken->value[i];
				strToken->value[i] = '\0';
				strToken->nextToken = strToken->value + strToken->nextStartPos;
				strToken->nextStartPos = i + 1;
			}
		}
		if (strToken->hasNextTokens == TRUE)
			break;
		if (isDelimChar == FALSE)
			tokenCnt++;
	}
	
	if (strToken->hasNextTokens == FALSE && 0 < tokenCnt) {
		strToken->hasNextTokens = TRUE;
		strToken->nextToken = strToken->value + strToken->nextStartPos;
		strToken->nextStartPos = strToken->lastPos + 1;
	}

	return strToken->currToken;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_string_tokenizer_nextalltoken
****************************************/

char *cg_string_tokenizer_nextalltoken(CgStringTokenizer *strToken)
{
	int nextTokenLen;

	cg_log_debug_l4("Entering...\n");

	nextTokenLen = cg_strlen(strToken->nextToken);
	strToken->nextToken[nextTokenLen] = strToken->repToken;
	strToken->currToken = strToken->nextToken;
	strToken->nextToken = NULL;
	strToken->hasNextTokens = FALSE;
	return strToken->currToken;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_string_tokenizer_print
****************************************/

void cg_string_tokenizer_print(CgStringTokenizer *strToken)
{
	cg_log_debug_l4("Entering...\n");

cg_log_debug_s(
		"cg_string_tokenizer_print\n"
		"value = %s\n, delim = %s,\n delimCnt = %d,\n nextStartPos = %d,\n lastPos = %d,\n currToken = %s,\n nextToken = %s,\n repToken = %c,\n hasNextTokens = %d\n",
		strToken->value,
		strToken->delim,
		strToken->delimCnt,
		strToken->nextStartPos,
		strToken->lastPos,
		strToken->currToken,
		strToken->nextToken,
		strToken->repToken,
		strToken->hasNextTokens);

	cg_log_debug_l4("Leaving...\n");
}
