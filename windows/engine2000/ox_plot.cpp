// ox_plot.cpp : �A�v���P�[�V�����p�N���X�̋@�\��`���s���܂��B
//

#include "stdafx.h"
#include "ox_plot.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" {
#include "ca.h"
#include "ifplot.h"

	extern HANDLE hStreamNotify;
	extern HANDLE hStreamNotify_Ack;
	extern DWORD MainThread;
	void Init_IO();
}
/////////////////////////////////////////////////////////////////////////////
// COx_plotApp

BEGIN_MESSAGE_MAP(COx_plotApp, CWinApp)
	//{{AFX_MSG_MAP(COx_plotApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COx_plotApp �N���X�̍\�z

COx_plotApp::COx_plotApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
	Init_IO();
}

/////////////////////////////////////////////////////////////////////////////
// �B��� COx_plotApp �I�u�W�F�N�g

COx_plotApp theApp;

/////////////////////////////////////////////////////////////////////////////
// COx_plotApp �N���X�̏�����

BOOL COx_plotApp::InitInstance()
{
	AfxEnableControlContainer();

	// �W���I�ȏ���������
	// ���������̋@�\���g�p�����A���s�t�@�C���̃T�C�Y��������
	// ��������Έȉ��̓���̏��������[�`���̒�����s�K�v�Ȃ���
	// ���폜���Ă��������B

#ifdef _AFXDLL
	Enable3dControls();		// ���L DLL �̒��� MFC ���g�p����ꍇ�ɂ͂������Ăяo���Ă��������B
#else
	Enable3dControlsStatic();	// MFC �ƐÓI�Ƀ����N���Ă���ꍇ�ɂ͂������Ăяo���Ă��������B
#endif

	// �ݒ肪�ۑ�����鉺�̃��W�X�g�� �L�[��ύX���܂��B
	// TODO: ���̕�������A��Ж��܂��͏����ȂǓK�؂Ȃ��̂�
	// �ύX���Ă��������B
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	// ���C�� �E�C���h�E���쐬����Ƃ��A���̃R�[�h�͐V�����t���[�� �E�C���h�E �I�u�W�F�N�g���쐬���A
	// ������A�v���P�[�V�����̃��C�� �E�C���h�E�ɃZ�b�g���܂�

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// �t���[�������\�[�X���烍�[�h���č쐬���܂�

	pFrame->m_pWindowName = "dummy window";
	pFrame->m_cansize.cx = 1;
	pFrame->m_cansize.cy = 1;
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, 
		NULL);
	MainThread = m_nThreadID;

	// ���C�� �E�B���h�E�����������ꂽ�̂ŁA�\���ƍX�V���s���܂��B
//	pFrame->ShowWindow(SW_SHOW);
//	pFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// COx_plotApp message handlers





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

	// ClassWizard ���z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �̃T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
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
		// ���b�Z�[�W �n���h���͂���܂���B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// �_�C�A���O�����s���邽�߂̃A�v���P�[�V���� �R�}���h
void COx_plotApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// COx_plotApp ���b�Z�[�W �n���h��

BOOL COx_plotApp::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
	CMainFrame *pFrame;
	struct canvas *can;

	switch ( pMsg->message ) {
	  case WM_APP: // copy to canvas 
	    can = canvas[pMsg->wParam];
	    if ( !can->window ) {
	      pFrame = new CMainFrame;
	      /* XXX */
	      pFrame->m_pWindowName = 
		(char *)malloc(MAX(strlen(can->wname),strlen("ox_plot"))+10);
	      sprintf(pFrame->m_pWindowName,"%s : %d",
		strlen(can->wname)?can->wname:"ox_plot",can->index);

	      pFrame->m_cansize.cx = can->width;
	      pFrame->m_cansize.cy = can->height;
	      pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);
	      can->window = (void *)pFrame;
	      pFrame->m_wndView.can = can;
	      pFrame->ShowWindow(SW_SHOW);
	      pFrame->UpdateWindow();
	      pFrame->BringWindowToTop();
	      can->hwnd = pFrame->m_wndView.m_hWnd;
	    } else
	      pFrame = (CMainFrame *)can->window;
	    pFrame->RedrawWindow();
	    break;
	  case WM_APP+1: // popup
	    can = canvas[pMsg->wParam];
	    pFrame = (CMainFrame *)can->window;
	    pFrame->ShowWindow(SW_SHOW);
	    break;
	  case WM_APP+2: // popdown
	    can = canvas[pMsg->wParam];
	    pFrame = (CMainFrame *)can->window;
	    pFrame->ShowWindow(SW_HIDE);
	    closed_canvas[pMsg->wParam] = can;
	    canvas[pMsg->wParam] = 0;
	    break;
	  default:
	    break;
	}
	return CWinApp::PreTranslateMessage(pMsg);
}

void COx_plotApp::OnAppExit() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
}

BOOL COx_plotApp::OnIdle(LONG lCount) 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
	
	return CWinApp::OnIdle(lCount);
}

void COx_plotApp::OnFilePrintSetup()
{
	// XXX: the following function is protected.
	CWinApp::OnFilePrintSetup();
}
