// asir32guiDoc.cpp : CAsir32guiDoc �N���X�̓���̒�`���s���܂��B
//

#include "stdafx.h"
#include "asir32gui.h"

#include "asir32guiDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" {
	void flush_log();
	void put_line(char *);
	extern int asirgui_kind;
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiDoc

IMPLEMENT_DYNCREATE(CAsir32guiDoc, CDocument)

BEGIN_MESSAGE_MAP(CAsir32guiDoc, CDocument)
	//{{AFX_MSG_MAP(CAsir32guiDoc)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiDoc �N���X�̍\�z/����

CAsir32guiDoc::CAsir32guiDoc()
{
	// TODO: ���̈ʒu�ɂP�񂾂��\�z�p�̃R�[�h��ǉ����Ă��������B

}

CAsir32guiDoc::~CAsir32guiDoc()
{
}

BOOL CAsir32guiDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���̈ʒu�ɍď�����������ǉ����Ă��������B
	// (SDI �h�L�������g�͂��̃h�L�������g���ė��p���܂��B)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiDoc �V���A���C�[�[�V����

void CAsir32guiDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���̈ʒu�ɕۑ��p�̃R�[�h��ǉ����Ă��������B
	}
	else
	{
		// TODO: ���̈ʒu�ɓǂݍ��ݗp�̃R�[�h��ǉ����Ă��������B
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiDoc �N���X�̐f�f

#ifdef _DEBUG
void CAsir32guiDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAsir32guiDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiDoc �R�}���h

extern "C" {
	void terminate_asir();
}

BOOL CAsir32guiDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
	
	if ( asirgui_kind == ASIRGUI_MESSAGE )
		return FALSE;

	if ( asirgui_kind == ASIRGUI_DEBUG ) {
		put_line("quit");
		return FALSE;
	}

	if( IDNO == ::MessageBox(NULL,"Really quit?","Asir",
		MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2) ) {
		return FALSE;
	}
	flush_log();
	terminate_asir();
	return TRUE;
//	return CDocument::CanCloseFrame(pFrame);
}

BOOL CAsir32guiDoc::SaveModified() 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
	
	return TRUE;
//	return CDocument::SaveModified();
}
