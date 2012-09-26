// CDmsYoutubeDlg.h : header file
//

#pragma once


// CCDmsYoutubeDlg dialog
class CCDmsYoutubeDlg : public CDialog
{
// Construction
public:
	CCDmsYoutubeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CDMSYOUTUBE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void UpdateContents();
public:
	afx_msg void OnBnClickedUpdate();
};
