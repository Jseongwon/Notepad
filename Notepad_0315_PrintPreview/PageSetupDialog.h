//PageSetupDialog.h
#ifndef _PAGESETUPDIALOG_H
#define _PAGESETUPDIALOG_H
#include<afxdlgs.h>
#include"resource.h"
class NotepadForm;
class PageSetupDialog :public CPageSetupDialog {
public:
	enum { IDD = IDD_PAGESETUPFORM };

public:
	PageSetupDialog(DWORD dwFlags, LONG m_bOrientation = 1, CWnd *pParentWnd = NULL);
	virtual INT_PTR DoModal();
	CString GetHeader();
	CString GetFooter();
	LONG GetOrientation();

protected:
	afx_msg virtual BOOL OnInitDialog();
	afx_msg virtual void OnOK();
	DECLARE_MESSAGE_MAP()

private:
	LONG m_bOrientation;
};

inline LONG PageSetupDialog::GetOrientation() {
	return this->m_bOrientation;
}

#endif // !_PAGESETUPDIALOG_H
