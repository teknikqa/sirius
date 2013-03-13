// SiriusDoc.cpp : implementation of the CSiriusDoc class
//

#include "stdafx.h"
#include "Sirius.h"

#include "SiriusDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSiriusDoc

IMPLEMENT_DYNCREATE(CSiriusDoc, CDocument)

BEGIN_MESSAGE_MAP(CSiriusDoc, CDocument)
	//{{AFX_MSG_MAP(CSiriusDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSiriusDoc construction/destruction

CSiriusDoc::CSiriusDoc()
{
	// TODO: add one-time construction code here

}

CSiriusDoc::~CSiriusDoc()
{
}

BOOL CSiriusDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSiriusDoc serialization

void CSiriusDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSiriusDoc diagnostics

#ifdef _DEBUG
void CSiriusDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSiriusDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSiriusDoc commands
