// FatalDialog.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "asir32gui.h"
#include "FatalDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFatalDialog �_�C�A���O


CFatalDialog::CFatalDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFatalDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFatalDialog)
	m_errmsg = _T("");
	//}}AFX_DATA_INIT
}


void CFatalDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFatalDialog)
	DDX_Text(pDX, IDC_NOTFOUND, m_errmsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFatalDialog, CDialog)
	//{{AFX_MSG_MAP(CFatalDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFatalDialog ���b�Z�[�W �n���h��

void CFatalDialog::OnOK() 
{
	// TODO: ���̈ʒu�ɂ��̑��̌��ؗp�̃R�[�h��ǉ����Ă�������
	
	CDialog::OnOK();
}

BOOL CFatalDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	SetDlgItemText(IDC_NOTFOUND,m_errmsg);	

	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}
