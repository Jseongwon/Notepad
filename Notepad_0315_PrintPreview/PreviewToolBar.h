// PreviewToolBar.h
#ifndef _PREVIEWTOOLBAR_H
#define _PREVIEWTOOLBAR_H
#include <afxwin.h>
#include "resource.h"

class PrintPreviewForm;
class PreviewToolBar : public CDialog {
	friend PrintPreviewForm;

public:
	enum { IDD = IDD_PREVIEW_TOOLBAR };

public:
	PrintPreviewForm *printPreviewForm;
	CSize size;

protected:
	PreviewToolBar(CWnd *pParentWnd);
	~PreviewToolBar();
	virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	afx_msg void OnPrintButtonClicked();
	afx_msg void OnPageSetupButtonClicked();
	afx_msg void OnFirstButtonClicked();
	afx_msg void OnPreviousButtonClicked();
	afx_msg void OnNextButtonClicked();
	afx_msg void OnLastButtonClicked();
	afx_msg void OnCloseButtonClicked();
	afx_msg void OnCurrentPageEditKillFocus();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
};

#endif // _PREVIEWTOOLBAR_H