// CDmsMobilePhotoDoc.cpp : implementation of the CCDmsMobilePhotoDoc class
//

#include "stdafx.h"
#include "CDmsMobilePhoto.h"

#include "CDmsMobilePhotoDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCDmsMobilePhotoDoc

IMPLEMENT_DYNCREATE(CCDmsMobilePhotoDoc, CDocument)

BEGIN_MESSAGE_MAP(CCDmsMobilePhotoDoc, CDocument)
END_MESSAGE_MAP()

// CCDmsMobilePhotoDoc construction/destruction

CCDmsMobilePhotoDoc::CCDmsMobilePhotoDoc()
{
	// TODO: add one-time construction code here

}

CCDmsMobilePhotoDoc::~CCDmsMobilePhotoDoc()
{
}

BOOL CCDmsMobilePhotoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

// CCDmsMobilePhotoDoc serialization

#ifndef _WIN32_WCE_NO_ARCHIVE_SUPPORT
void CCDmsMobilePhotoDoc::Serialize(CArchive& ar)
{
	(ar);
}
#endif // !_WIN32_WCE_NO_ARCHIVE_SUPPORT


// CCDmsMobilePhotoDoc diagnostics

#ifdef _DEBUG
void CCDmsMobilePhotoDoc::AssertValid() const
{
	CDocument::AssertValid();
}
#endif //_DEBUG


// CCDmsMobilePhotoDoc commands

