// CDmsMobilePhotoDoc.h : interface of the CCDmsMobilePhotoDoc class
//


#pragma once

class CCDmsMobilePhotoDoc : public CDocument
{
protected: // create from serialization only
	CCDmsMobilePhotoDoc();
	DECLARE_DYNCREATE(CCDmsMobilePhotoDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
#ifndef _WIN32_WCE_NO_ARCHIVE_SUPPORT
	virtual void Serialize(CArchive& ar);
#endif // !_WIN32_WCE_NO_ARCHIVE_SUPPORT

// Implementation
public:
	virtual ~CCDmsMobilePhotoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


