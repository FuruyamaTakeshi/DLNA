// CDmsMobilePhoto.h : main header file for the CDmsMobilePhoto application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif 
#ifdef SMARTPHONE2003_UI_MODEL
#include "resourcesp.h"
#endif

// CCDmsMobilePhotoApp:
// See CDmsMobilePhoto.cpp for the implementation of this class
//

#include "cdms_filesys.h"
#include <cybergarage/io/cfile.h>

#define CDMS_MOBILE_PHOTO_TIMER (10 * 1000)
#define CDMS_MOBILE_PHOTO_APPNAME "Cyber Media Gate for Windows Mobile"

class CCDmsMobilePhotoApp : public CWinApp
{
	CgUpnpMediaServer *dms;

	CgString *shideShowRoot;
	CgFileList *slideShowFileList;
	CgFile* nextSlideShowFile;

public:
	CCDmsMobilePhotoApp();
	~CCDmsMobilePhotoApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
public:
#ifndef WIN32_PLATFORM_WFSP
	afx_msg void OnAppAbout();
#endif // !WIN32_PLATFORM_WFSP

	afx_msg void OnAppOption();

	DECLARE_MESSAGE_MAP()

public:

	void SetSlideShowRoot(char *dir)
	{
		cg_string_setvalue(shideShowRoot, dir);
	}

	char *GetSlideShowRoot()
	{
		return cg_string_getvalue(shideShowRoot);
	}

	CgUpnpMediaServer *GetDMS()
	{
		return dms;
	}

	CgFile *GetNextSlideShowFIle();

	int GetUpdateInterval()
	{
		return CDMS_MOBILE_PHOTO_TIMER;
	}

	void SetUpdateTimer()
	{
		m_pMainWnd->SetTimer(CDMS_MOBILE_PHOTO_TIMER, GetUpdateInterval(), 0);
	}

	void KillUpdateTimer()
	{
		m_pMainWnd->KillTimer(CDMS_MOBILE_PHOTO_TIMER);
	}

};

extern CCDmsMobilePhotoApp theApp;
