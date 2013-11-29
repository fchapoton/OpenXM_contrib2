// winfepDlg.h : �w�b�_�[ �t�@�C��
//

#if !defined(AFX_WINFEPDLG_H__E284CDDE_5B01_4A61_A9D5_43C3E6FC5CFE__INCLUDED_)
#define AFX_WINFEPDLG_H__E284CDDE_5B01_4A61_A9D5_43C3E6FC5CFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CWinfepDlg �_�C�A���O

class CWinfepDlg : public CDialog
{
// �\�z
public:
	CWinfepDlg(CWnd* pParent = NULL);	// �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CWinfepDlg)
	enum { IDD = IDD_WINFEP_DIALOG };
	CEdit	m_currentline;
	CStatic	m_after;
	CStatic	m_before;
	CStatic	m_currentfile;
	//}}AFX_DATA

	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CWinfepDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �̃T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	HICON m_hIcon;

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CWinfepDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSend();
	afx_msg void OnChangeFilename();
	afx_msg void OnFileopen();
	afx_msg void OnAsir();
	afx_msg void OnSkip();
	afx_msg void OnNext();
	afx_msg void OnPrev();
	afx_msg void OnPrev10();
	afx_msg void OnNext10();
	virtual void OnOK();
	afx_msg void OnQuit();
	afx_msg void OnIntr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void show_line(int);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_WINFEPDLG_H__E284CDDE_5B01_4A61_A9D5_43C3E6FC5CFE__INCLUDED_)
