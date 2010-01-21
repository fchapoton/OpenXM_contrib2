// winfepDlg.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "winfep.h"
#include "winfepDlg.h"
#include <process.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HWND asirhwnd;
FILE *infile;
char *filebuf;
char *before,*after;
int nlines, currentline;
char **lineptr;

extern "C" { 
void get_asirhwnd()
{
	char *temp;
	if ( temp = getenv("TEMP") ) {
		int len;
		char *name;
		FILE *fp;
		len = strlen(temp);
		name = (char *)malloc(len+BUFSIZ);
		sprintf(name,"%s\\asirgui.hnd",temp);
		fp = fopen(name,"r");
		if ( fp ) {
			fscanf(fp,"%d",&asirhwnd);
			fclose(fp);
		}
	}
}


}

/////////////////////////////////////////////////////////////////////////////
// �A�v���P�[�V�����̃o�[�W�������Ŏg���Ă��� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂�
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �̃T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	//{{AFX_MSG(CAboutDlg)
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

/////////////////////////////////////////////////////////////////////////////
// CWinfepDlg �_�C�A���O

CWinfepDlg::CWinfepDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWinfepDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWinfepDlg)
	//}}AFX_DATA_INIT
	// ����: LoadIcon �� Win32 �� DestroyIcon �̃T�u�V�[�P���X��v�����܂���B
    get_asirhwnd();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinfepDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWinfepDlg)
	DDX_Control(pDX, IDC_CURRENTLINE, m_currentline);
	DDX_Control(pDX, IDC_AFTER, m_after);
	DDX_Control(pDX, IDC_BEFORE, m_before);
	DDX_Control(pDX, IDC_CURRENTFILE, m_currentfile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWinfepDlg, CDialog)
	//{{AFX_MSG_MAP(CWinfepDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(Send, OnSend)
	ON_BN_CLICKED(IDC_FILEOPEN, OnFileopen)
	ON_BN_CLICKED(IDC_BUTTON3, OnAsir)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_BN_CLICKED(IDC_PREV10, OnPrev10)
	ON_BN_CLICKED(IDC_NEXT10, OnNext10)
	ON_BN_CLICKED(IDC_QUIT, OnQuit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinfepDlg ���b�Z�[�W �n���h��

BOOL CWinfepDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "�o�[�W�������..." ���j���[���ڂ��V�X�e�� ���j���[�֒ǉ����܂��B

	// IDM_ABOUTBOX �̓R�}���h ���j���[�͈̔͂łȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���̃_�C�A���O�p�̃A�C�R����ݒ肵�܂��B�t���[�����[�N�̓A�v���P�[�V�����̃��C��
	// �E�B���h�E���_�C�A���O�łȂ����͎����I�ɐݒ肵�܂���B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R����ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R����ݒ�
	
	// TODO: ���ʂȏ��������s�����͂��̏ꏊ�ɒǉ����Ă��������B
	
	return TRUE;  // TRUE ��Ԃ��ƃR���g���[���ɐݒ肵���t�H�[�J�X�͎����܂���B
}

void CWinfepDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����_�C�A���O�{�b�N�X�ɍŏ����{�^����ǉ�����Ȃ�΁A�A�C�R����`�悷��
// �R�[�h���ȉ��ɋL�q����K�v������܂��BMFC �A�v���P�[�V������ document/view
// ���f�����g���Ă���̂ŁA���̏����̓t���[�����[�N�ɂ�莩���I�ɏ�������܂��B

void CWinfepDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// �N���C�A���g�̋�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R����`�悵�܂��B
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// �V�X�e���́A���[�U�[���ŏ����E�B���h�E���h���b�O���Ă���ԁA
// �J�[�\����\�����邽�߂ɂ������Ăяo���܂��B
HCURSOR CWinfepDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CWinfepDlg::OnSend() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	if ( !infile ) return;
    if ( currentline >= 0 && currentline < nlines ) {
	  CString str;
	  m_currentline.GetWindowText(str);
      LPCSTR line = (LPCSTR)str;
      for ( int i = 0; line[i]; i++ ) {
	if ( line[i] == '\t' )
        	::SendMessage(asirhwnd,WM_CHAR,' ',8);
	else
        	::SendMessage(asirhwnd,WM_CHAR,line[i],1);
      }
      ::SendMessage(asirhwnd,WM_CHAR,'\n',1);
      show_line(++currentline);
    } else
	show_line(currentline);
}

void CWinfepDlg::OnChangeFilename() 
{
	// TODO: ���ꂪ RICHEDIT �R���g���[���̏ꍇ�A�R���g���[���́A lParam �}�X�N
	// ���ł̘_���a�� ENM_CHANGE �t���O�t���� CRichEditCrtl().SetEventMask()
	// ���b�Z�[�W���R���g���[���֑��邽�߂� CDialog::OnInitDialog() �֐����I�[�o�[
	// ���C�h���Ȃ����肱�̒ʒm�𑗂�܂���B
	
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	
	
}

void CWinfepDlg::OnFileopen() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CFileDialog fileDialog(TRUE);
	char *prev;
	struct _stat fs;
	int i,j,n,nl;
	if ( fileDialog.DoModal() == IDOK ) {
		CString pathName = fileDialog.GetPathName();
		if ( infile ) fclose(infile);
		infile = fopen(pathName,"r");
		m_currentfile.SetWindowText(pathName);
		_fstat(_fileno(infile),&fs);
		filebuf = (char *)realloc(filebuf,fs.st_size+1);
		before = (char *)realloc(before,fs.st_size+1);
		after = (char *)realloc(after,fs.st_size+1);
		n = fread(filebuf,1,fs.st_size,infile);
		filebuf[n] = 0;
		for ( nl = 0, i = 0; i < n; i++ )
			if ( filebuf[i] == '\n' ) nl++;
		nlines = nl;
		lineptr = (char **)realloc(lineptr,(nl+1)*sizeof(char *));
		prev = filebuf;
		for ( i = 0, j = 0; i < n; i++ )
			if ( filebuf[i] == '\n' ) {
				filebuf[i] = 0;
				lineptr[j++] = prev;
				prev = filebuf+i+1;
			}
		currentline = 0;
		show_line(0);
	}
}

void CWinfepDlg::OnAsir() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������	CFileDialog fileDialog(TRUE);
	CFileDialog fileDialog(TRUE);
	if ( fileDialog.DoModal() == IDOK ) {
		CString pathName = fileDialog.GetPathName();
		_spawnl(_P_NOWAIT,pathName,pathName,NULL); 
		Sleep(5000);
		get_asirhwnd();
	}
}

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

void CWinfepDlg::show_line(int i)
{
	int j,nafter,nbefore;
	char *cur;

	before[0] = after[0] = 0;
	nbefore = MIN(5,i);
	if ( nbefore < 0 ) nbefore = 0;
	for ( j = 0; j < 5-nbefore; j++ )
		strcat(before,"\n");
	for ( j = i-nbefore; j<i; j++ ) {
		strcat(before,lineptr[j]); strcat(before,"\n");
	} 
	nafter = MIN(5,nlines-i-1);
	if ( nafter < 0 ) nafter = 0;
	for ( j = i+1; j < i+1+nafter; j++ ) {
		strcat(after,lineptr[j]); strcat(after,"\n");
	}
	m_before.SetWindowText(before);
	if ( i < 0 ) cur = "<Beginning of File>";
	else if ( i == nlines ) cur = "<End of File>";
	else cur = lineptr[i];	
	m_currentline.SetWindowText(cur);
	m_after.SetWindowText(after);
}


void CWinfepDlg::OnNext() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	if ( !infile ) return;
    if ( currentline == nlines ) return;
    show_line(++currentline);	
}

void CWinfepDlg::OnPrev() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	if ( !infile ) return;
	if ( currentline < 0 ) return;
	show_line(--currentline);
}

void CWinfepDlg::OnPrev10() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	if ( !infile ) return;
	currentline -= 10;
	if ( currentline < 0 ) currentline = -1;
	show_line(currentline);
}

void CWinfepDlg::OnNext10() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	if ( !infile ) return;
    currentline += 10;
	if ( currentline >= nlines ) currentline = nlines;
	show_line(currentline);
}

void CWinfepDlg::OnOK() 
{
	// TODO: ���̈ʒu�ɂ��̑��̌��ؗp�̃R�[�h��ǉ����Ă�������
    OnSend();
}

void CWinfepDlg::OnQuit() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CDialog::OnOK();
	
}
