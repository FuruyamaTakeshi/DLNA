// COptionDialog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "CDmsMobilePhoto.h"
#include "COptionDialog.h"


// COptionDialog ダイアログ

IMPLEMENT_DYNAMIC(COptionDialog, CDialog)

COptionDialog::COptionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDialog::IDD, pParent)
{

}

COptionDialog::~COptionDialog()
{
}

void COptionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionDialog, CDialog)
END_MESSAGE_MAP()


// COptionDialog メッセージ ハンドラ
