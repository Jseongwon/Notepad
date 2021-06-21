// PageSetupDialog.cpp
/*
파일명칭 : PageSetupDialog.cpp
기능 : 페이지 설정 대화상자 클래스를 만든다.
작성자 : 김승현, 정성원
작성일자 : 2021.01.29
*/
#include "PageSetupDialog.h"
#include "NotepadForm.h"
#include "Font.h"
#include <dlgs.h>
#include <afxdisp.h>
#include "Printer.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(PageSetupDialog, CPageSetupDialog)
	ON_BN_CLICKED(IDOK, OnOK)
END_MESSAGE_MAP()

PageSetupDialog::PageSetupDialog(DWORD dwFlags, LONG m_bOrientation, CWnd *pParentWnd)
	: CPageSetupDialog(dwFlags, pParentWnd) {
	this->m_psd.Flags |= PSD_ENABLEPAGESETUPTEMPLATE | PSD_MARGINS | PSD_INHUNDREDTHSOFMILLIMETERS;
	this->m_psd.hInstance = AfxGetResourceHandle();
	this->m_psd.lpPageSetupTemplateName = MAKEINTRESOURCE(PageSetupDialog::IDD);
	this->m_bOrientation = m_bOrientation;
}

BOOL PageSetupDialog::OnInitDialog() {
	CPageSetupDialog::OnInitDialog();

	CString header = AfxGetApp()->GetProfileString("NotepadSection", "Header", "");
	CString footer = AfxGetApp()->GetProfileString("NotepadSection", "Footer", "");

	this->GetDlgItem(IDC_EDIT_HEADER)->SetWindowText(header);
	this->GetDlgItem(IDC_EDIT_FOOTER)->SetWindowText(footer);

	return TRUE;
}

INT_PTR PageSetupDialog::DoModal() {
	LONG left = AfxGetApp()->GetProfileInt("NotepadSection", "LEFT", 20);
	LONG top = AfxGetApp()->GetProfileInt("NotepadSection", "TOP", 20);
	LONG right = AfxGetApp()->GetProfileInt("NotepadSection", "RIGHT", 20);
	LONG bottom = AfxGetApp()->GetProfileInt("NotepadSection", "BOTTOM", 20);

	DWORD prevFlags;

	LPDEVMODE devmode = (LPDEVMODE)::GlobalLock(this->m_psd.hDevMode);
	if (devmode == 0) {
		prevFlags = this->m_psd.Flags;
		this->m_psd.Flags |= PSD_RETURNDEFAULT;
		CPageSetupDialog::DoModal();

		devmode = (LPDEVMODE)::GlobalLock(this->m_psd.hDevMode);
		this->m_psd.Flags = prevFlags;
	}
	RECT rect = { left * 100, top * 100, right * 100, bottom * 100 };
	this->m_psd.rtMargin = rect;

	devmode->dmFields |= DM_ORIENTATION | DM_PAPERSIZE;
	devmode->dmPaperSize = AfxGetApp()->GetProfileInt("NotepadSection", "PaperSize", DMPAPER_A4);

	if (this->m_bOrientation == 2) {
		devmode->dmOrientation = DMORIENT_LANDSCAPE;
	}

	::GlobalUnlock(devmode);

	return CPageSetupDialog::DoModal();
}

void PageSetupDialog::OnOK() { // 여기서 써줘야함.
	CString header;
	CString footer;

	this->GetDlgItem(IDC_EDIT_HEADER)->GetWindowText(header);
	this->GetDlgItem(IDC_EDIT_FOOTER)->GetWindowText(footer);

	this->m_psd.rtMargin.left /= 100;
	this->m_psd.rtMargin.top /= 100;
	this->m_psd.rtMargin.right /= 100;
	this->m_psd.rtMargin.bottom /= 100;

	AfxGetApp()->WriteProfileInt("NotepadSection", "LEFT", this->m_psd.rtMargin.left);
	AfxGetApp()->WriteProfileInt("NotepadSection", "TOP", this->m_psd.rtMargin.top);
	AfxGetApp()->WriteProfileInt("NotepadSection", "RIGHT", this->m_psd.rtMargin.right);
	AfxGetApp()->WriteProfileInt("NotepadSection", "BOTTOM", this->m_psd.rtMargin.bottom);
	AfxGetApp()->WriteProfileString("NotepadSection", "Header", header);
	AfxGetApp()->WriteProfileString("NotepadSection", "Footer", footer);

	LPDEVMODE devmode = (LPDEVMODE)::GlobalLock(this->m_psd.hDevMode);

	devmode = (LPDEVMODE)::GlobalLock(this->m_psd.hDevMode);
	if(devmode->dmOrientation == 1) {
		this->m_bOrientation = 1;
	}
	else if (devmode->dmOrientation == 2) {
		this->m_bOrientation = 2;
	}
	
	if (devmode->dmOrientation != 0) {
		AfxGetApp()->WriteProfileInt("NotepadSection", "Orientation", this->m_bOrientation);
		AfxGetApp()->WriteProfileInt("NotepadSection", "PaperSize", devmode->dmPaperSize);
	}

	::GlobalUnlock(devmode);

	CPageSetupDialog::OnOK();
}

CString PageSetupDialog::GetHeader() {
	CString header = AfxGetApp()->GetProfileString("NotepadSection", "Header", "");

	return header;
}

CString PageSetupDialog::GetFooter() {
	CString footer = AfxGetApp()->GetProfileString("NotepadSection", "Footer", "");

	return footer;
}