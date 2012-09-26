// CDmsMobilePhotoView.cpp : implementation of the CCDmsMobilePhotoView class
//

#include "stdafx.h"
#include "CDmsMobilePhoto.h"

#include "CDmsMobilePhotoDoc.h"
#include "CDmsMobilePhotoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCDmsMobilePhotoView

IMPLEMENT_DYNCREATE(CCDmsMobilePhotoView, CView)

BEGIN_MESSAGE_MAP(CCDmsMobilePhotoView, CView)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CCDmsMobilePhotoView construction/destruction

CCDmsMobilePhotoView::CCDmsMobilePhotoView()
{
	// TODO: add construction code here

}

CCDmsMobilePhotoView::~CCDmsMobilePhotoView()
{
}

BOOL CCDmsMobilePhotoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}


// CCDmsMobilePhotoView drawing
void CCDmsMobilePhotoView::OnDraw(CDC* pDC)
{
	CCDmsMobilePhotoApp *app = &theApp;
	CCDmsMobilePhotoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CgFile *nextSlideShowFile = app->GetNextSlideShowFIle();
	if (!nextSlideShowFile)
		return;

	CgString *findDirStr = cg_string_new();
	cg_string_addvalue(findDirStr, app->GetSlideShowRoot());
	cg_string_addvalue(findDirStr, "\\");
	cg_string_addvalue(findDirStr, cg_file_getname(nextSlideShowFile));

	// TODO: add draw code for native data here
	#if defined(UNICODE)
	TCHAR wCharBuf[MAX_PATH];
	MultiByteToWideChar(CP_UTF8, 0, cg_string_getvalue(findDirStr), -1, wCharBuf, (MAX_PATH-1));
	HBITMAP hbitmap = SHLoadImageFile(wCharBuf);
	#else
	HBITMAP hbitmap = SHLoadImageFile(cg_string_getvalue(findDirStr));
	#endif

	if (hbitmap) {
		CBitmap bitmap;
		bitmap.Attach(hbitmap);
		BITMAP bmpInfo;
		bitmap.GetBitmap(&bmpInfo);

		RECT winRect;
		GetClientRect(&winRect);
		float xScale = (float)winRect.right/ (float)bmpInfo.bmWidth;
		float yScale = (float)winRect.bottom / (float)bmpInfo.bmHeight;
		float bltScale = (xScale > yScale) ? xScale : yScale;

		CDC bitmapDC;
		bitmapDC.CreateCompatibleDC(pDC);
		CBitmap* pOldBitmap = bitmapDC.SelectObject(&bitmap);

		float xOffset = ((float)bmpInfo.bmWidth - (float)winRect.right/bltScale )/2.0f;
		float yOffset = ((float)bmpInfo.bmHeight - (float)winRect.bottom/bltScale)/2.0f;

		pDC->StretchBlt(
			0,
			0,
			winRect.right,
			winRect.bottom,
			&bitmapDC,
			(int)xOffset,
			(int)yOffset,
			(int)((float)bmpInfo.bmWidth - (xOffset*2.0f)),
			(int)((float)bmpInfo.bmHeight - (yOffset*2.0f)),
			SRCCOPY);

		bitmapDC.SelectObject(pOldBitmap);
		bitmap.DeleteObject();
	}

	cg_string_delete(findDirStr);
}

// CCDmsMobilePhotoView diagnostics

#ifdef _DEBUG
void CCDmsMobilePhotoView::AssertValid() const
{
	CView::AssertValid();
}

CCDmsMobilePhotoDoc* CCDmsMobilePhotoView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCDmsMobilePhotoDoc)));
	return (CCDmsMobilePhotoDoc*)m_pDocument;
}
#endif //_DEBUG


// CCDmsMobilePhotoView message handlers

BOOL CCDmsMobilePhotoView::OnEraseBkgnd(CDC* pDC)
{
	//return CView::OnEraseBkgnd(pDC);
	return TRUE;
}
