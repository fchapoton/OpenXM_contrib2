// asir32gui.h : ASIR32GUI �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C��
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // ���C�� �V���{��

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiApp:
// ���̃N���X�̓���̒�`�Ɋւ��Ă� asir32gui.cpp �t�@�C�����Q�Ƃ��Ă��������B
//

class CAsir32guiApp : public CWinApp
{
public:
	CAsir32guiApp();
	void CreateNewDoc() { OnFileNew(); }
// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��𐶐����I�[�o�[���C�h���܂��B
	//{{AFX_VIRTUAL(CAsir32guiApp)
	public:
	virtual BOOL OnIdle(LONG);
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����

	//{{AFX_MSG(CAsir32guiApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
#include "asir32gui_io.h"
