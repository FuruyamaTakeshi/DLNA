/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: kb_util.h
*
*	Revision:
*
*	05/11/05
*		- first revision
*
******************************************************************/

#ifndef _CG_KB_UTIL_H_
#define _CG_KB_UTIL_H_


void kbinit();
int kbhit();
int kbkey();
void kbexit();

#ifdef  __cplusplus
}
#endif

#endif
