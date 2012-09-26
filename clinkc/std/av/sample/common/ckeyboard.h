/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: ckeyboad.h
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNPAV_KEYBOAD_H_
#define _CG_UPNPAV_KEYBOAD_H_

#ifdef  __cplusplus
extern "C" {
#endif

void cg_kb_init();
int cg_kb_hit();
int cg_kb_key();
void cg_kb_exit();

#ifdef  __cplusplus
}
#endif

#endif
