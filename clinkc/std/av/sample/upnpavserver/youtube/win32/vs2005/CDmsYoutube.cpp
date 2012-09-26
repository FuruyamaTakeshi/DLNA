// CDmsYoutube.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CDmsYoutube.h"
#include "CDmsYoutubeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCDmsYoutubeApp

BEGIN_MESSAGE_MAP(CCDmsYoutubeApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCDmsYoutubeApp construction

CCDmsYoutubeApp::CCDmsYoutubeApp()
{
	dms = cg_upnp_dms_youtube_new();
	cg_upnp_dms_youtube_start(dms);
}

// CCDmsYoutubeApp destruction

CCDmsYoutubeApp::~CCDmsYoutubeApp()
{
	cg_upnp_dms_youtube_stop(dms);
	cg_upnp_dms_youtube_delete(dms);
}


// The one and only CCDmsYoutubeApp object

CCDmsYoutubeApp theApp;


// CCDmsYoutubeApp initialization

BOOL CCDmsYoutubeApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CCDmsYoutubeDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

	return FALSE;
}
