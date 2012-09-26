// CDmsMobilePhotoView.h : interface of the CCDmsMobilePhotoView class
//


#pragma once

class CCDmsMobilePhotoView : public CView
{
protected: // create from serialization only
	CCDmsMobilePhotoView();
	DECLARE_DYNCREATE(CCDmsMobilePhotoView)

// Attributes
public:
	CCDmsMobilePhotoDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:

// Implementation
public:
	virtual ~CCDmsMobilePhotoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // debug version in CDmsMobilePhotoView.cpp
inline CCDmsMobilePhotoDoc* CCDmsMobilePhotoView::GetDocument() const
   { return reinterpret_cast<CCDmsMobilePhotoDoc*>(m_pDocument); }
#endif

