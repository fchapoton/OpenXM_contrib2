// MainFrm.cpp : CMainFrame �N���X�̓���̒�`���s���܂��B
//

#include "stdafx.h"
#include "asir32gui.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_HEAP,OnUpdateHeapSize)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // �X�e�[�^�X ���C�� �C���W�P�[�^
	ID_INDICATOR_KANA,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_HEAP,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame �N���X�̍\�z/����

CMainFrame *theFrame;

CMainFrame::CMainFrame()
{
	// TODO: ���̈ʒu�Ƀ����o�̏����������R�[�h��ǉ����Ă��������B
	theFrame = this;	
}

CMainFrame::~CMainFrame()
{
}

extern "C" {
	extern int asirgui_kind;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if ( asirgui_kind == ASIRGUI_MAIN ) {
		if (!m_wndToolBar.Create(this) ||
			!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{
			TRACE0("Failed to create toolbar\n");
			return -1;      // �쐬�Ɏ��s
		}
	
		if (!m_wndStatusBar.Create(this) ||
			!m_wndStatusBar.SetIndicators(indicators,
		  	sizeof(indicators)/sizeof(UINT)))
		{
			TRACE0("Failed to create status bar\n");
			return -1;      // �쐬�Ɏ��s
		}
	
		// TODO: �����c�[�� �`�b�v�X���K�v�Ȃ��ꍇ�A�������폜���Ă��������B
		m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
			CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	
		// TODO: �c�[�� �o�[���h�b�L���O�\�ɂ��Ȃ��ꍇ�͈ȉ��̂R�s���폜
		//       ���Ă��������B
		m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
		EnableDocking(CBRS_ALIGN_ANY);
		DockControlBar(&m_wndToolBar);
	}
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���̈ʒu�� CREATESTRUCT cs ���C�����āAWindow �N���X��X�^�C����
	//       �C�����Ă��������B
	if ( asirgui_kind == ASIRGUI_MAIN ) {
	   	cs.cx = ::GetSystemMetrics(SM_CXSCREEN)*2 / 3; 
		cs.cy = ::GetSystemMetrics(SM_CYSCREEN)*2 / 3; 
	} else {
	   	cs.cx = ::GetSystemMetrics(SM_CXSCREEN)*2 / 5; 
		cs.cy = ::GetSystemMetrics(SM_CYSCREEN)*1 / 5; 
	}

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame �N���X�̐f�f

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame ���b�Z�[�W �n���h��

extern "C" {
	extern int AsirHeapSize;

void show_window(int on)
{
	if ( on )
		theFrame->ShowWindow(SW_SHOW);
	else
		theFrame->ShowWindow(SW_HIDE);
}
}

void CMainFrame::OnUpdateHeapSize(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString strHeap;
	strHeap.Format("HeapSize %dBytes",AsirHeapSize);
	pCmdUI->SetText(strHeap);
}
