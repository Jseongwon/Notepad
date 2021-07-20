// PrintPreviewForm.h
#ifndef _PRINTPREVIEWFORM_H
#define _PRINTPREVIEWFORM_H
#include <afxwin.h>

class PreviewToolBar;
class NotepadForm;
class Glyph;
class PrintPreviewForm : public CFrameWnd {
public:
	PreviewToolBar *previewToolBar;
	NotepadForm *notepadForm;
	BOOL onIsNotepadOrWordMode;
	BOOL onIsWordWrap;
	HICON m_hIcon;
	LONG m_nCurrentPage;
	LONG m_nPages;

	CRect m_cRect;
	double m_dWidthRate;
	double m_dHeightRate;

public:
	PrintPreviewForm(NotepadForm *notepadForm = NULL, BOOL onIsNotepadOrWordMode = TRUE, BOOL onIsWordWrap = FALSE);
	BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	afx_msg virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg virtual void OnPaint();
	afx_msg virtual void OnClose();
	DECLARE_MESSAGE_MAP()
};

#endif // _PRINTPREVIEWFORM_H