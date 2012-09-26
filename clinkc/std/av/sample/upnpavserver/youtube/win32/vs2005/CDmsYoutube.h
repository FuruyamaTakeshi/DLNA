// CDmsYoutube.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "cdms_youtube.h"

// CCDmsYoutubeApp:
// See CDmsYoutube.cpp for the implementation of this class
//

class CCDmsYoutubeApp : public CWinApp
{
	CgUpnpMediaServer *dms;

public:
	CCDmsYoutubeApp();
	~CCDmsYoutubeApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

public:

	CgUpnpMediaServer *GetDMS()
	{
		return dms;
	}
};

extern CCDmsYoutubeApp theApp;