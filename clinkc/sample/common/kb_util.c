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

#if !defined(__MINGW32__)
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#endif

#if defined (__CYGWIN__) && !defined(FIONREAD)
#include <winsock2.h>
#include <windows.h>
#endif

#if !defined(__MINGW32__)
static struct termios orgTermAttr;
#endif

#include "kb_util.h"

void kbinit()
{
#if !defined(__MINGW32__)
	struct termios termAttr;
	tcgetattr(0, &orgTermAttr);
	termAttr = orgTermAttr;
	termAttr.c_lflag &= ~(ICANON | ECHO);
	termAttr.c_cc[VTIME] = 0;
	termAttr.c_cc[VMIN] = 1;
	tcsetattr(0, TCSANOW, &termAttr);
#endif
}

int kbhit()
{
#if !defined(__MINGW32__)
	int ret, n;
	ret = ioctl(0, FIONREAD, &n);
	if (ret != -1)
		return n;
#endif
	return 0;
}

int kbkey()
{
	return 0;
}

void kbexit()
{
#if !defined(__MINGW32__)
	tcsetattr(0, TCSANOW, &orgTermAttr);
#endif
}

