#if !defined(AFX_FATALDIALOG_H__DF52D853_7AB2_11D2_9A87_009027045022__INCLUDED_)
#define AFX_FATALDIALOG_H__DF52D853_7AB2_11D2_9A87_009027045022__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FatalDialog.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CFatalDialog �_�C�A���O

class CFatalDialog : public CDialog
{
// �R���X�g���N�V����
public:
	CFatalDialog(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CFatalDialog)
	enum { IDD = IDD_FATAL };
	CString	m_errmsg;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CFatalDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CFatalDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_FATALDIALOG_H__DF52D853_7AB2_11D2_9A87_009027045022__INCLUDED_)
