// ChildView.h : CChildView �N���X�̃C���^�[�t�F�C�X�̒�`�����܂��B
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__1BE7F53D_38AA_4247_893C_9C2D236C7B1F__INCLUDED_)
#define AFX_CHILDVIEW_H__1BE7F53D_38AA_4247_893C_9C2D236C7B1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CChildView �E�B���h�E

class CChildView : public CWnd
{
// �R���X�g���N�^
public:
	CChildView();

// �A�g���r���[�g
public:
	struct canvas *can;

// �I�y���[�V����
public:
	void DestroyCanvas();
	void PrintAxis(CDC &);

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CChildView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	virtual ~CChildView();
	void OnPrint(CDC &);

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnOptNoaxis();
	afx_msg void OnUpdateOptNoaxis(CCmdUI* pCmdUI);
	afx_msg void OnOptPrecise();
	afx_msg void OnUpdateOptPrecise(CCmdUI* pCmdUI);
	afx_msg void OnOptWide();
	afx_msg void OnUpdateOptWide(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ ���V������`��������ꍇ�ɂ́A���̍s�����O�ɒǉ����܂��B

#endif // !defined(AFX_CHILDVIEW_H__1BE7F53D_38AA_4247_893C_9C2D236C7B1F__INCLUDED_)
