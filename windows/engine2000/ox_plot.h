// ox_plot.h : OX_PLOT �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C��
//

#if !defined(AFX_OX_PLOT_H__59DD85A9_D89D_4970_A079_E159432A107F__INCLUDED_)
#define AFX_OX_PLOT_H__59DD85A9_D89D_4970_A079_E159432A107F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // ���C�� �V���{��

/////////////////////////////////////////////////////////////////////////////
// COx_plotApp:
// ���̃N���X�̓���̒�`�Ɋւ��Ă� ox_plot.cpp �t�@�C�����Q�Ƃ��Ă��������B
//

class COx_plotApp : public CWinApp
{
public:
	COx_plotApp();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(COx_plotApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����

public:
	//{{AFX_MSG(COx_plotApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppExit();
	//}}AFX_MSG
	afx_msg void OnFilePrintSetup();
	DECLARE_MESSAGE_MAP()
};

extern COx_plotApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_OX_PLOT_H__59DD85A9_D89D_4970_A079_E159432A107F__INCLUDED_)
