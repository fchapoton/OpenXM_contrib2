// asir32guiView.cpp : CAsir32guiView �N���X�̓���̒�`���s���܂��B
//

#include "stdafx.h"

#include "asir32gui.h"
#include "asir32guiDoc.h"
#include "asir32guiView.h"
#include "FatalDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const WindowHeight       = 200;
const WindowWidth        = 400;
const TextBufferSize     = 32768;

CAsir32guiView *theView;

extern "C"
{
char *prev_hist(void), *next_hist(void), *search_hist(char *p);
void read_input_history(),write_input_history();
void init_input_history();
void add_hist(char *p), reset_traverse(void);
void asir_terminate(void);
void Main(int,char*[]);
void loadfile(char *);
void put_line(char *);
void send_intr(void);
void insert_to_theView(char *);
void get_rootdir(char *,int,char *);
extern int bigfloat, prtime, prresult;
extern char *asir_libdir;
extern int asirgui_kind;

void insert_to_theView(char *str)
{
	(void)theView->Insert(str);
}
void flush_log()
{
	if ( theView->Logging )
		theView->OnFileLog();
}
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiView

IMPLEMENT_DYNCREATE(CAsir32guiView, CEditView)

BEGIN_MESSAGE_MAP(CAsir32guiView, CEditView)
	//{{AFX_MSG_MAP(CAsir32guiView)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_LOG, OnFileLog)
	ON_COMMAND(ID_ASIRHELP, OnAsirhelp)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOG, OnUpdateFileLog)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	//}}AFX_MSG_MAP
	// �W������R�}���h
//	ON_COMMAND(ID_FILE_PRINT, CEditView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_DIRECT, CEditView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiView �N���X�̍\�z/����

CAsir32guiView::CAsir32guiView()
{
	// TODO: ���̏ꏊ�ɍ\�z�p�̃R�[�h��ǉ����Ă��������B
	static int tmpView_created = 0;
	char errmsg[BUFSIZ];

	init_input_history();
	read_input_history();
	DebugMode = 0;
	DebugInMain = 0;
	MaxLineLength = 80;
	LogStart = 0;
	Logging = 0;
	Logfp = NULL;
	ResetIndex();
	theView = this;
}

CAsir32guiView::~CAsir32guiView()
{
	write_input_history();
}

BOOL CAsir32guiView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���̈ʒu�� CREATESTRUCT cs ���C������ Window �N���X�܂��̓X�^�C����
	//       �C�����Ă��������B

	m_dwDefaultStyle = dwStyleDefault & ~ES_AUTOHSCROLL ;
	m_dwDefaultStyle &= ~WS_HSCROLL;
	return CCtrlView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiView �N���X�̕`��

void CAsir32guiView::OnDraw(CDC* pDC)
{
	CAsir32guiDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: ���̏ꏊ�Ƀl�C�e�B�u �f�[�^�p�̕`��R�[�h��ǉ����܂��B
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiView �N���X�̈��

BOOL CAsir32guiView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �f�t�H���g�̈������
	return DoPreparePrinting(pInfo);
}

void CAsir32guiView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ����O�̓��ʂȏ�����������ǉ����Ă��������B
}

void CAsir32guiView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �����̌㏈����ǉ����Ă��������B
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiView �N���X�̐f�f

#ifdef _DEBUG
void CAsir32guiView::AssertValid() const
{
	CEditView::AssertValid();
}

void CAsir32guiView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CAsir32guiDoc* CAsir32guiView::GetDocument() // ��f�o�b�O �o�[�W�����̓C�����C���ł��B
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAsir32guiDoc)));
	return (CAsir32guiDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiView �N���X�̃��b�Z�[�W �n���h��

void CAsir32guiView::PutChar(int c)
{
  int i;
  char buf[2];

  for ( i = EndPos-1; i >= CurrentPos; i-- )
    Buffer[i+1] = Buffer[i];
  Buffer[CurrentPos] = (char)c;
  buf[0] = (char)c; buf[1] = 0;
  Insert(buf,FALSE);
  CurrentPos++;
  EndPos++;
}

void CAsir32guiView::DeleteChar(int count) {
  int i,s,e;
  char null = 0;

  if ( count > (EndPos-CurrentPos) )
    count = EndPos - CurrentPos;
  if ( count == 0 )
    Beep();
  else {
    EndPos -= count;
    for ( i = CurrentPos; i < EndPos; i++ )
      Buffer[i] = Buffer[i+count];
    GetEditCtrl().GetSel(s,e);
    GetEditCtrl().SetSel(s,s+count);
    GetEditCtrl().ReplaceSel(&null);
  }
}

void CAsir32guiView::Insert(char *buf,BOOL wrap)
{
	char t[BUFSIZ*4];
	char *p,*q,c;
	int len;
	int i;

	if ( wrap ) {
		i = GetEditCtrl().LineLength(-1);
		for ( p = t, q = buf; (c = *p++ = *q++) != 0; )
			if ( c == '\r' ) {
				*p++ = *q++; i = 0;
			} else if ( (*q != '\r') && (++i == MaxLineLength) ) {
				*p++ = '\r'; *p++ = '\r'; *p++ = '\n'; i = 0;
			}
		p = t;
	} else
		p = buf;
	len = GetWindowTextLength();
	if ( strlen(p) + len >= TextBufferSize )
		DeleteTop();
	GetEditCtrl().ReplaceSel(p);
}

void CAsir32guiView::DeleteTop()
{
  const char *p,*q;
  const char *buf;
  char null = 0;
  int len,len0,len1;

  buf = LockBuffer();
  len0 = len = strlen(buf);
  for ( p = buf; len >= TextBufferSize/2; ) {
    q = strchr(p,'\n');
    if ( !q ) {
       len = 0; break;
    } else {
      q++; len -= (q-p); p = q;
    }
  }
  if ( Logging ) {
    len1 = (len0 - len - LogStart);
    if ( len1 > 0 ) {
      fwrite(buf+LogStart,1,len1,Logfp);
      LogStart = 0;
    } else
      LogStart = -len1;
  }
  UnlockBuffer();
  if ( q ) {
	GetEditCtrl().SetSel(0,len);
	GetEditCtrl().ReplaceSel(&null);
  }
  UpdateCursor(EndPos);
}

void CAsir32guiView::UpdateCursor(int pos)
{
  int s;
  int len;

  if ( pos < 0 || pos > EndPos )
    Beep();
  else {
    len = GetWindowTextLength();
    CurrentPos = pos;
    s = len-EndPos+CurrentPos;
    GetEditCtrl().SetSel(s,s);
  }
}

void CAsir32guiView::ReplaceString(char *str)
{
  UpdateCursor(0); DeleteChar(EndPos);
  Insert(str,FALSE);
  strcpy(Buffer,str);
  EndPos = CurrentPos = strlen(str);
}

void CAsir32guiView::Beep(void) {
  ::MessageBeep(0xffffffff);
}

void CAsir32guiView::Paste(void) {
	char buf[2*BUFSIZ];
	const char *src;
	char c;
	int i,j,l;
	HGLOBAL hClip;

	if ( asirgui_kind == ASIRGUI_MESSAGE ) {
		Beep(); return;
	}

	if ( OpenClipboard() == FALSE ) {
    	Beep(); return;
    }
    hClip = GetClipboardData(CF_TEXT);
    src = (const char *)::GlobalLock(hClip);
    if ( !src || (l = strlen(src)) >= 2*BUFSIZ ) {
    	::CloseClipboard();
    	Beep(); return;
    }
	for ( i = j = 0; i < l; i++ )
		if ( (c = src[i]) != '\n' && c != '\r' )
			buf[j++] = c;
	buf[j] = 0;
    ::GlobalUnlock(hClip);
    ::CloseClipboard();
    GetEditCtrl().ReplaceSel(buf);
	strncpy(Buffer+EndPos,buf,j);
	EndPos += j; CurrentPos = EndPos;
}

#define CTRL(c) (c)&0x1f

void CAsir32guiView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	char *exp_result;

	switch ( nChar ) {
		case VK_LEFT:
			UpdateCursor(CurrentPos-1); return;
		case VK_RIGHT:
			UpdateCursor(CurrentPos+1); return;
		case VK_DELETE:
			DeleteChar(1); return;
		case VK_UP:
			if ( (exp_result = prev_hist()) != 0 )
				ReplaceString(exp_result);
			else
				Beep();
			return;
		case VK_DOWN:
			if ( (exp_result = next_hist()) != 0 )
				ReplaceString(exp_result);
			else
				Beep();
			return;
		default:
			break;
	}
	CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAsir32guiView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	char *exp_result;
	
	if ( asirgui_kind == ASIRGUI_MESSAGE ) {
		Beep(); return;
	}

	switch ( nChar ) {
	case CTRL('M'): case CTRL('J'):
		UpdateCursor(EndPos);
		Buffer[EndPos] = 0;
		reset_traverse();
		if ( Buffer[0] == '!' ) {
		exp_result = search_hist(Buffer+1);
		if ( exp_result )
			ReplaceString(exp_result);
		else {
			UpdateCursor(0); DeleteChar(EndPos); Beep();
		}
		return;
		} else {
			add_hist(Buffer);
			put_line(Buffer);
			ResetIndex();
			nChar = CTRL('M');
		}
		break;
	case CTRL('N'):
		if ( (exp_result = next_hist()) != 0 )
		ReplaceString(exp_result);
		else
		Beep();
		return;
	case CTRL('P'):
		if ( (exp_result = prev_hist()) != 0 )
		ReplaceString(exp_result);
		else
		Beep();
		return;
	case CTRL('A'):
		UpdateCursor(0); return;
	case CTRL('E'):
		UpdateCursor(EndPos); return;
	case CTRL('F'):
		UpdateCursor(CurrentPos+1); return;
	case CTRL('B'):
		UpdateCursor(CurrentPos-1); return;
	case CTRL('K'):
		DeleteChar(EndPos-CurrentPos); return;
	case CTRL('U'):
		UpdateCursor(0); DeleteChar(EndPos); return;
	case CTRL('D'):
		DeleteChar(1); return;
	case CTRL('H'):
		if ( CurrentPos > 0 ) {
		UpdateCursor(CurrentPos-1); DeleteChar(1);
		} else
		Beep();
		return;
	case CTRL('C'):
		send_intr();
		/* wake up the engine */
		put_line("");
		return;
	default :
		if ( nChar >= 0x20 && nChar < 0x7f )
			PutChar(nChar);
		else
			Beep();
		return;
	}
	CEditView::OnChar(nChar, nRepCnt, nFlags);
}

void CAsir32guiView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
    GetEditCtrl().Copy(); UpdateCursor(EndPos);
	CEditView::OnLButtonUp(nFlags, point);
}

void CAsir32guiView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
    Paste();
	
//	CEditView::OnRButtonDown(nFlags, point);
}

void CAsir32guiView::OnFileOpen() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	char cmd[BUFSIZ*2]; // XXX
	char *p;

	CFileDialog	fileDialog(TRUE);
	if ( fileDialog.DoModal() == IDOK ) {
		CString pathName = fileDialog.GetPathName();
		sprintf(cmd,"load(\"%s\");",pathName);
		for ( p = cmd; *p; p++ )
			if ( *p == '\\' )
				*p = '/';
		put_line(cmd);
	}
}

void CAsir32guiView::OnAsirhelp() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	char root[BUFSIZ],errmsg[BUFSIZ],helpfile[BUFSIZ];

	get_rootdir(root,sizeof(root),errmsg);
	sprintf(helpfile,"%s\\bin\\asirhelp.chm",root);
	HtmlHelp(NULL, helpfile, HH_DISPLAY_TOPIC, 0);
}

void CAsir32guiView::OnFileLog() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	if ( !Logging ) {
		CFileDialog	fileDialog(TRUE);
		if ( fileDialog.DoModal() == IDOK ) {
			CString logfile = fileDialog.GetPathName();
			Logfp = fopen(logfile,"ab");
			if ( Logfp ) {
				LogStart = GetWindowTextLength();
				Logging = 1;
			}
		}
	} else {
		const char *buf;
		buf = LockBuffer();
		fwrite(buf+LogStart,1,strlen(buf)-LogStart,Logfp);
		UnlockBuffer();
		fclose(Logfp);
		Logging = 0;
	}
}

void CAsir32guiView::OnUpdateFileLog(CCmdUI* pCmdUI) 
{
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
	if ( Logging ) 
		pCmdUI->SetCheck(1);	
	else
		pCmdUI->SetCheck(0);	
}

void CAsir32guiView::OnEditPaste() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
    Paste();
}
