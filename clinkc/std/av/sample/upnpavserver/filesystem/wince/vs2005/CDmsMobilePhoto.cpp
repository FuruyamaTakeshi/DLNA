// CDmsMobilePhoto.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CDmsMobilePhoto.h"
#include "MainFrm.h"
#include <shlobj.h>

#include "CDmsMobilePhotoDoc.h"
#include "CDmsMobilePhotoView.h"

#include "COptionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCDmsMobilePhotoApp

BEGIN_MESSAGE_MAP(CCDmsMobilePhotoApp, CWinApp)
#ifndef WIN32_PLATFORM_WFSP
	ON_COMMAND(ID_APP_ABOUT, &CCDmsMobilePhotoApp::OnAppAbout)
#endif // !WIN32_PLATFORM_WFSP
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	ON_COMMAND(ID_APP_OPTION, &CCDmsMobilePhotoApp::OnAppOption)
END_MESSAGE_MAP()

// CCDmsMobilePhotoApp construction
CCDmsMobilePhotoApp::CCDmsMobilePhotoApp()
	: CWinApp()
{
	// Initialize Slide Show File
	shideShowRoot = cg_string_new();
	slideShowFileList = cg_filelist_new();
	nextSlideShowFile = NULL;

	// Set Slide Show Root Directory
	TCHAR rootDir[MAX_PATH];
	char rootDirUtf8[MAX_PATH];
	SHGetSpecialFolderPath(NULL, rootDir, CSIDL_MYPICTURES, FALSE);
	WideCharToMultiByte(CP_UTF8, 0, rootDir, -1, rootDirUtf8, (MAX_PATH-1), NULL, NULL);
	SetSlideShowRoot(rootDirUtf8);

	// Initialize Cyber Media Gate
	dms = cg_upnp_dms_filesys_new();
	cg_upnp_dms_filesys_setpublicationdirectory(dms, GetSlideShowRoot());
	CgUpnpDevice *dev = cg_upnp_dms_getdevice(dms);
	cg_upnp_device_setfriendlyname(dev, CDMS_MOBILE_PHOTO_APPNAME);
	cg_upnp_dms_filesys_start(dms);
}

// CCDmsMobilePhotoApp destruction
CCDmsMobilePhotoApp::~CCDmsMobilePhotoApp()
{
	cg_string_delete(shideShowRoot);
	cg_filelist_delete(slideShowFileList);

	cg_upnp_dms_filesys_stop(dms);
	cg_upnp_dms_filesys_delete(dms);
}

// The one and only CCDmsMobilePhotoApp object
CCDmsMobilePhotoApp theApp;

// CCDmsMobilePhotoApp initialization

BOOL CCDmsMobilePhotoApp::InitInstance()
{
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
    // SHInitExtraControls should be called once during your application's initialization to initialize any
    // of the Windows Mobile specific controls such as CAPEDIT and SIPPREF.
    SHInitExtraControls();
#endif // WIN32_PLATFORM_PSPC || WIN32_PLATFORM_WFSP

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
	SetRegistryKey(_T(CDMS_MOBILE_PHOTO_APPNAME));
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CCDmsMobilePhotoDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CCDmsMobilePhotoView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	SetUpdateTimer();

	return TRUE;
}

CgFile *CCDmsMobilePhotoApp::GetNextSlideShowFIle()
{
	if (!nextSlideShowFile) {
		cg_filelist_clear(slideShowFileList);
		CgFile *photoDir = cg_file_new();
		cg_file_setname(photoDir, GetSlideShowRoot());
		cg_file_listfiles(photoDir, slideShowFileList);
		cg_file_delete(photoDir);
		nextSlideShowFile = cg_filelist_gets(slideShowFileList);
	}

	if (!nextSlideShowFile)
		return NULL;

	CgFile *currSlideShowFile = nextSlideShowFile;
	nextSlideShowFile = cg_file_next(nextSlideShowFile);

	return currSlideShowFile;
}

#ifndef WIN32_PLATFORM_WFSP
// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
#ifdef _DEVICE_RESOLUTION_AWARE
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;	// return TRUE unless you set the focus to a control
			// EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
#ifdef _DEVICE_RESOLUTION_AWARE
	ON_WM_SIZE()
#endif
END_MESSAGE_MAP()

#ifdef _DEVICE_RESOLUTION_AWARE
void CAboutDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	DRA::RelayoutDialog(
		AfxGetInstanceHandle(), 
		this->m_hWnd, 
		DRA::GetDisplayMode() != DRA::Portrait ? MAKEINTRESOURCE(IDD_ABOUTBOX_WIDE) : MAKEINTRESOURCE(IDD_ABOUTBOX));
}
#endif

// App command to run the dialog
void CCDmsMobilePhotoApp::OnAppAbout()
{
	KillUpdateTimer();
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
	SetUpdateTimer();
}
#endif // !WIN32_PLATFORM_WFSP

void CCDmsMobilePhotoApp::OnAppOption()
{
	KillUpdateTimer();
	COptionDialog optDlg;
	optDlg.DoModal();
	SetUpdateTimer();
}

