/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cmd5.h
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifndef _CG_CLINKCAV_MD5_H_
#define _CG_CLINKCAV_DIDL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CG_MD5_STRING_BUF_SIZE ((16*2)+1)

char *cg_str2md5(char *string, char *md5buf);

#ifdef __cplusplus
}
#endif

#endif
