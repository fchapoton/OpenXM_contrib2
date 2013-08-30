// asir32gui.cpp : �A�v���P�[�V�����p�N���X�̋@�\��`���s���܂��B
//

#include "stdafx.h"
#include "asir32gui.h"

#include "MainFrm.h"
#include "asir32guiDoc.h"
#include "Asir32guiView.h"
#include "FatalDialog.h"
#include <winnls.h>
#include <string.h>
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" {
	extern HANDLE hNotify;
	extern HANDLE hNotify_Ack;
	extern int asirgui_kind;
	BOOL Init_IO(char *);
	BOOL get_rootdir(char *,int,char *);
	void read_and_insert();
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiApp

BEGIN_MESSAGE_MAP(CAsir32guiApp, CWinApp)
	//{{AFX_MSG_MAP(CAsir32guiApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// �W���̃t�@�C����{�h�L�������g �R�}���h
	// �W���̈���Z�b�g�A�b�v �R�}���h
//	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiApp �N���X�̍\�z

CAsir32guiApp::CAsir32guiApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
	char errmsg[BUFSIZ];

	if ( Init_IO(errmsg) == FALSE ) {
		CFatalDialog fatalDlg;
		fatalDlg.m_errmsg = errmsg;
		fatalDlg.DoModal();
		exit(0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// �B��� CAsir32guiApp �I�u�W�F�N�g

CAsir32guiApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CAsir32guiApp �N���X�̏�����

BOOL CAsir32guiApp::InitInstance()
{
	// �W���I�ȏ���������
	// ���������̋@�\���g�p�����A���s�t�@�C���̃T�C�Y��������
	// ��������Έȉ��̓���̏��������[�`���̒�����s�K�v�Ȃ���
	// ���폜���Ă��������B

    char lang[BUFSIZ];
    char dll[BUFSIZ];
    char root[BUFSIZ];
    char errmsg[BUFSIZ];
    HINSTANCE hRes;
    get_rootdir(root,sizeof(root),errmsg);
    if(GetLocaleInfo(GetUserDefaultLCID(), LOCALE_SISO639LANGNAME, lang, BUFSIZ)) {
        sprintf(dll, "%s\\bin\\%s.dll", root, lang);
        hRes = LoadLibrary(dll);
        if(hRes) {
            AfxSetResourceHandle(hRes);
        }
    }

#ifdef _AFXDLL
	Enable3dControls();			// ���L DLL �̒��� MFC ���g�p����ꍇ�ɂ͂������R�[�����Ă��������B 
#else
	Enable3dControlsStatic();	// MFC �ƐÓI�Ƀ����N���Ă���ꍇ�ɂ͂������R�[�����Ă��������B
#endif

	LoadStdProfileSettings();  // �W���� INI �t�@�C���̃I�v�V���������[�ނ��܂� (MRU ���܂�)
	// �A�v���P�[�V�����p�̃h�L�������g �e���v���[�g��o�^���܂��B�h�L�������g �e���v���[�g
	// �̓h�L�������g�A�t���[�� �E�B���h�E�ƃr���[���������邽�߂ɋ@�\���܂��B

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		asirgui_kind==ASIRGUI_MAIN?IDR_MAINFRAME:IDR_DEBUGFRAME,
		RUNTIME_CLASS(CAsir32guiDoc),
		RUNTIME_CLASS(CMainFrame),       // ���C�� SDI �t���[�� �E�B���h�E
		RUNTIME_CLASS(CAsir32guiView));
	AddDocTemplate(pDocTemplate);

	// DDE�Afile open �ȂǕW���̃V�F�� �R�}���h�̃R�}���h���C������͂��܂��B
	CCommandLineInfo cmdInfo;
//	ParseCommandLine(cmdInfo);

	// �R�}���h���C���Ńf�B�X�p�b�` �R�}���h���w�肵�܂��B
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// �A�v���P�[�V�����̃o�[�W�������Ŏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

  	// ClassWizard �͉��z�֐��𐶐����I�[�o�[���C�h���܂��B
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �̃T�|�[�g
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// ���b�Z�[�W �n���h���͂���܂���B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// ���b�Z�[�W �n���h��������܂���B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// �_�C�A���O�����s���邽�߂̃A�v���P�[�V���� �R�}���h
void CAsir32guiApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiApp �R�}���h

BOOL CAsir32guiApp::OnIdle(LONG lCount) 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
	
	if ( CWinApp::OnIdle(lCount) )
		return TRUE;
	while ( 1 ) {
		int retcode = ::MsgWaitForMultipleObjects(1,&hNotify,FALSE,-1,QS_ALLINPUT);
		retcode -= WAIT_OBJECT_0;
		if ( retcode == 0 ) {
			ResetEvent(hNotify);
			SetEvent(hNotify_Ack);
			read_and_insert();
		} else
			break;
	}
	return TRUE;
}
