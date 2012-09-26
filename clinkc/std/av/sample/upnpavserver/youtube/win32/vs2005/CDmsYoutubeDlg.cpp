// CDmsYoutubeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CDmsYoutube.h"
#include "CDmsYoutubeDlg.h"

#include <cybergarage/util/cthread.h>
#include "cdms_youtube.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CCDmsYoutubeDlg dialog

CCDmsYoutubeDlg::CCDmsYoutubeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCDmsYoutubeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCDmsYoutubeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCDmsYoutubeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_UPDATE, &CCDmsYoutubeDlg::OnBnClickedUpdate)
END_MESSAGE_MAP()

// CCDmsYoutubeDlg message handlers

BOOL CCDmsYoutubeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CCDmsYoutubeApp *app = &theApp;
	CgUpnpMediaServer *dms = app->GetDMS();

	SetWindowText(_T(CG_UPNP_MEDIA_YOUTUBE_APP_NAME) _T(" ") _T(CG_UPNP_MEDIA_YOUTUBE_APP_VER));

	CWnd *updateButton = GetDlgItem(IDC_UPDATE);
	CProgressCtrl  *progressBar = (CProgressCtrl  *)GetDlgItem(IDC_PROGRESS);
	CComboBox *rssUrlList = (CComboBox  *)GetDlgItem(IDC_RSS_URL);

	rssUrlList->AddString(_T(CG_UPNP_MEDIA_YOUTUBE_RSS_URL_MOSTVIEWEDVIDEOS_TODAY));
	rssUrlList->AddString(_T(CG_UPNP_MEDIA_YOUTUBE_RSS_URL_MOSTVIEWEDVIDEOS_WEEK));
	rssUrlList->AddString(_T(CG_UPNP_MEDIA_YOUTUBE_RSS_URL_MOSTVIEWEDVIDEOS_MONTH));
	rssUrlList->AddString(_T(CG_UPNP_MEDIA_YOUTUBE_RSS_URL_MOSTVIEWEDVIDEOS_ALLTIME));
	rssUrlList->AddString(_T(CG_UPNP_MEDIA_YOUTUBE_RSS_URL_RECENTLYADDED));
	rssUrlList->AddString(_T(CG_UPNP_MEDIA_YOUTUBE_RSS_URL_RECENTLYFRATURED));
	rssUrlList->AddString(_T(CG_UPNP_MEDIA_YOUTUBE_RSS_URL_TOPFAVORITES));
	rssUrlList->AddString(_T(CG_UPNP_MEDIA_YOUTUBE_RSS_URL_TOPRATED));
	rssUrlList->SetCurSel(0);

	updateButton->EnableWindow(FALSE);
	progressBar->EnableWindow(FALSE);
	rssUrlList->EnableWindow(FALSE);
	SetDlgItemText(IDC_MESSAGE, _T(""));

	UpdateContents();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCDmsYoutubeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCDmsYoutubeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCDmsYoutubeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void UpdateProgressBar(int size, int cnt, char *msg, void *userData)
{
	CCDmsYoutubeDlg *dlg = (CCDmsYoutubeDlg *)userData;

	CProgressCtrl  *progressBar = (CProgressCtrl  *)dlg->GetDlgItem(IDC_PROGRESS);
	progressBar->SetRange(0, size);
	progressBar->SetPos(cnt);
	
	CString msgStr(msg);
	dlg->SetDlgItemText(IDC_MESSAGE, msgStr);
}

void UpdateContents(CgThread *thread)
{
	CCDmsYoutubeDlg *dlg = (CCDmsYoutubeDlg *)cg_thread_getuserdata(thread);
	CCDmsYoutubeApp *app = &theApp;
	CgUpnpMediaServer *dms = app->GetDMS();

	CWnd *updateButton = dlg->GetDlgItem(IDC_UPDATE);
	CProgressCtrl  *progressBar = (CProgressCtrl  *)dlg->GetDlgItem(IDC_PROGRESS);
	CComboBox *rssUrlList = (CComboBox  *)dlg->GetDlgItem(IDC_RSS_URL);

	updateButton->EnableWindow(FALSE);
	rssUrlList->EnableWindow(FALSE);
	progressBar->EnableWindow(TRUE);
	dlg->SetDlgItemText(IDC_MESSAGE, _T(""));
	cg_upnp_dms_youtube_setupdatelistener(dms, UpdateProgressBar);
	cg_upnp_dms_youtube_setupdatelistenerdata(dms, dlg);
	cg_upnp_dms_youtube_update(dms);
	updateButton->EnableWindow(TRUE);
	rssUrlList->EnableWindow(TRUE);
	progressBar->EnableWindow(FALSE);
	dlg->SetDlgItemText(IDC_MESSAGE, _T("Done"));

	cg_thread_stop(thread);
}

void CCDmsYoutubeDlg::UpdateContents()
{
	CCDmsYoutubeApp *app = &theApp;
	CgUpnpMediaServer *dms = app->GetDMS();

	CComboBox *rssUrlList = (CComboBox  *)GetDlgItem(IDC_RSS_URL);
	int selListIdx = rssUrlList->GetCurSel();
	if (selListIdx == CB_ERR)
		return;

	CString rssURL;
	rssUrlList->GetLBText(selListIdx, rssURL);
	LPCTSTR rssURLStr = (LPCTSTR)rssURL;
	#if defined(UNICODE)
	char mCharBuf[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, rssURLStr, -1, mCharBuf, (MAX_PATH-1), NULL, NULL);
	cg_upnp_dms_youtube_setrssurl(dms, mCharBuf);
	#else
	cg_upnp_dms_youtube_setrssurl(dms, (char *)rssURLStr);
	#endif


	CgThread *thread = cg_thread_new();
	cg_thread_setaction(thread, ::UpdateContents);
	cg_thread_setuserdata(thread, this);
	cg_thread_start(thread);
}

void CCDmsYoutubeDlg::OnBnClickedUpdate()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	UpdateContents();
}
