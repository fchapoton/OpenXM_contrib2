// asir32guiView.h : CAsir32guiView �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂��B
//
/////////////////////////////////////////////////////////////////////////////

class CAsir32guiView : public CEditView
{
public: // �V���A���C�Y�@�\�݂̂���쐬���܂��B
	CAsir32guiView();
	DECLARE_DYNCREATE(CAsir32guiView);

// �A�g���r���[�g
public:
	CAsir32guiDoc* GetDocument();

// �I�y���[�V����
public:

	int DebugMode;
	int DebugInMain;
	int CurrentPos,EndPos;
	int MaxLineLength;
	int LogStart;
	int Logging;
	FILE *Logfp;
	char Buffer[BUFSIZ*2];

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��𐶐����I�[�o�[���C�h���܂��B
	//{{AFX_VIRTUAL(CAsir32guiView)
	public:
	virtual void OnDraw(CDC* pDC);  // ���̃r���[��`�悷��ۂɃI�[�o�[���C�h����܂��B
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	virtual ~CAsir32guiView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void Paste(void);
	void Insert(char *,BOOL wrap = TRUE);
	void ResetIndex(void) {EndPos = CurrentPos = 0;}
	void PutChar(int);
	void DeleteChar(int);
	void DeleteTop(void);
	void UpdateCursor(int);
	void Beep(void);
	void ReplaceString(char *);
	void UpdateLineLength(int);

protected:

// �������ꂽ���b�Z�[�W �}�b�v�֐�
public:
	//{{AFX_MSG(CAsir32guiView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFileOpen();
	afx_msg void OnFileLog();
	afx_msg void OnAsirhelp();
	afx_msg void OnUpdateFileLog(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnFont();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContribhelp();
	afx_msg void OnUpdateContribhelp(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // asir32guiView.cpp �t�@�C�����f�o�b�O���̎��g�p����܂��B
inline CAsir32guiDoc* CAsir32guiView::GetDocument()
   { return (CAsir32guiDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

