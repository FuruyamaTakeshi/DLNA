/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdms_filesys_main.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include "ckeyboard.h"
#include "cdms_filesys.h"

#include <stdio.h>
#include <ctype.h>

#if defined(WIN32)
#include <conio.h>
#include <shlobj.h>
#endif

/****************************************
* PrintKeyMessage
****************************************/

void PrintKeyMessage()
{
	printf("'p' : Print\n");
	printf("'x' : eXit\n");
}

/****************************************
* main
****************************************/

int main(int argc, char *argv[])
{
	CgUpnpMediaServer *dms;
	char *pubDir;
	int programNameIdx;
	int key;
#if defined(WIN32)
	TCHAR myDocumentPath[MAX_PATH];
	#if defined(UNICODE)
	char myDocumentPathUtf8[MAX_PATH];
	#endif
#endif

	pubDir = NULL;

	if (1 < argc) {
		pubDir = argv[argc-1];
	}
	else {
#if defined(WIN32)
		SHGetFolderPath(NULL,CSIDL_PERSONAL,NULL,0,myDocumentPath);
		#if defined(UNICODE)
		WideCharToMultiByte(CP_UTF8, 0, myDocumentPath, -1, myDocumentPathUtf8, (MAX_PATH-1), NULL, NULL);
		pubDir = myDocumentPathUtf8;
		#else
		pubDir = myDocumentPath;
		#endif
#endif
	}

	if (!pubDir) {
#if defined(WIN32)
		programNameIdx = cg_strrchr(argv[0], "\\", 1);
#else
		programNameIdx = cg_strrchr(argv[0], "/", 1);
#endif
		printf("Usage : %s <publication directory>\n", (argv[0] + programNameIdx + 1));
		exit(1);
	}

	dms = cg_upnp_dms_filesys_new();
	cg_upnp_dms_filesys_setpublicationdirectory(dms, pubDir);
	cg_upnp_dms_filesys_start(dms);

#if !defined(WIN32) || defined(__CYGWIN__)
	cg_kb_init();
#endif

	PrintKeyMessage();
	
	do
	{
#if defined(WIN32) && !defined(__CYGWIN__)
		key = _getch();
#else
		key = getchar();
#endif
		key = toupper(key);
		switch (key) {
		case 'X':
			break;
		default:
			PrintKeyMessage();
		}
	} while(key != 'X');

#if !defined(WIN32) || defined(__CYGWIN__)
	cg_kb_exit();
#endif

	cg_upnp_dms_filesys_stop(dms);
	cg_upnp_dms_filesys_delete(dms);

	return 0;
}
