// PreviewToolBar.cpp
/*
파일명칭 : PreviewToolBar.cpp
기능 : 미리보기 툴바를 만든다.
작성자 : 김승현, 정성원
작성일자 : 2021.03.04
*/
#include "PreviewToolBar.h"
#include "PrintPreviewForm.h"
#include "NotepadForm.h"
#include "PageSetupDialog.h"
#include "Glyph.h"
#include "File.h"
#include "Printer.h"
#include "PageInfo.h"
#include <afxdlgs.h>
#include <afxpriv.h>

BEGIN_MESSAGE_MAP(PreviewToolBar, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PRINT, OnPrintButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_PAGESETUP, OnPageSetupButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_FIRST, OnFirstButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_PREVIOUS, OnPreviousButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnNextButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_LAST, OnLastButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnCloseButtonClicked)
	ON_EN_KILLFOCUS(IDC_EDIT_CURRENTPAGE, OnCurrentPageEditKillFocus)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

PreviewToolBar::PreviewToolBar(CWnd *pParentWnd)
	: CDialog(PreviewToolBar::IDD, pParentWnd) {
	this->printPreviewForm = (PrintPreviewForm*)pParentWnd;
}

PreviewToolBar::~PreviewToolBar() {
}

BOOL PreviewToolBar::Create(UINT nIDTemplate, CWnd* pParentWnd) {
	CDialog::Create(nIDTemplate, pParentWnd);
	CString pageCurrent;
	CString pageCount;

	CRect clientRect;

	// 1. 페이지 개수를 출력한다.
	pageCount.Format("/ %d", this->printPreviewForm->m_nPages);
	this->GetDlgItem(IDC_STATIC_PAGECOUNT)->SetWindowText(pageCount);

	// 2. 현재 페이지를 출력한다.
	pageCurrent.Format("%d", this->printPreviewForm->m_nCurrentPage);
	this->GetDlgItem(IDC_EDIT_CURRENTPAGE)->SetWindowText(pageCurrent);

	// 3. 툴바의 높이를 구한다.
	this->GetClientRect(&clientRect);

	this->size.cy = clientRect.Height();

	// 3. 미리보기 화면을 갱신한다.
	this->printPreviewForm->Invalidate();

	return FALSE;
}

void PreviewToolBar::OnPrintButtonClicked() {
	CPrintDialog dlg(FALSE, PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_NOSELECTION, this);
	DOCINFO di;
	CDC *cdc;

	Glyph *note;
	Glyph *line;

	Long pageLineCount;
	Long printWidth;
	Long printHeight;
	Long totalHeight;
	Long current;
	Long i = 0;

	bool onIsKorean = false;

	string linestr;

	CFont font;
	LOGFONT logFont;

	RECT margin = { 0, };
	CString header;
	CString footer;
	Long devMode = 1;
	string fileTitle;

	if (dlg.DoModal() == IDOK) {
		//docInfo 작성
		memset(&di, 0, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		fileTitle = this->printPreviewForm->notepadForm->file->GetTitle();
		di.lpszDocName = fileTitle.c_str();

		//dc정보읽기
		cdc = CDC::FromHandle(dlg.GetPrinterDC());

		//가로세로 정보 설정
		devMode = this->printPreviewForm->notepadForm->printer->GetNotepadPageInfo()->GetOrientation();
		if (devMode != 1) {//가로모드로 설정이 되어있으면
			DEVMODE *pDevMode = dlg.GetDevMode();
			pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
			cdc->ResetDCA(pDevMode);
		}

		printWidth = cdc->GetDeviceCaps(HORZRES);
		printHeight = cdc->GetDeviceCaps(VERTRES);

		//폰트 적용
		logFont = this->printPreviewForm->notepadForm->printer->GetPrintLogFont();
		font.CreateFontIndirectA(&logFont);
		TEXTMETRIC metric;
		CFont *def_font = cdc->SelectObject(&font);
		cdc->GetTextMetrics(&metric);

		//페이지 여백설정
		header = AfxGetApp()->GetProfileString("NotepadSection", "Header", "");
		footer = AfxGetApp()->GetProfileString("NotepadSection", "Footer", "");

		CRect writeRect = this->printPreviewForm->notepadForm->printer->GetPrintWriteRect();

		if (header.Compare("") != 0) {
			writeRect.top += (metric.tmHeight);
		}
		if (footer.Compare("") != 0) {
			writeRect.bottom -= (metric.tmHeight);
		}

		CRect headerRect(0, 0, printWidth, printHeight + (metric.tmHeight));
		CRect footerRect(0, printHeight - (metric.tmHeight), printWidth, printHeight);
		cdc->SetMapMode(MM_TEXT);

		//인쇄 시작
		CRect           r;
		CPrintInfo info;
		info.m_rectDraw.SetRect(writeRect.left, writeRect.top, writeRect.right, writeRect.bottom);

		// 도큐먼트 인쇄 시작
		BOOL bPrintingOK = cdc->StartDoc(&di);

		// 페이지를 인쇄하는 루프
		pageLineCount = this->printPreviewForm->notepadForm->printer->GetPageLineCount();
		note = this->printPreviewForm->notepadForm->printer->GetPrintNote();
		i = 0;
		while (i < note->GetLength()) {
			cdc->StartPage();
			current = 0;
			totalHeight = writeRect.top + metric.tmHeight;
			cdc->DrawText(header, &headerRect, DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
			cdc->DrawText(footer, &footerRect, DT_CENTER | DT_SINGLELINE | DT_TOP);
			while (i < note->GetLength() && current < pageLineCount && totalHeight <= writeRect.bottom) {
				line = note->GetAt(i);
				cdc->TextOut(writeRect.left, metric.tmHeight * current + writeRect.top, line->GetString().c_str());

				current++;
				totalHeight += metric.tmHeight;
				i++;
			}
			cdc->EndPage();
		}

		// 도큐먼트 인쇄 종료
		if (bPrintingOK) cdc->EndDoc();
		else cdc->AbortDoc();
		cdc->SelectObject(def_font);

		font.DeleteObject();
		//dc할당해제
		cdc->DeleteDC();

		this->printPreviewForm->PostMessage(WM_CLOSE);
	}
}

void PreviewToolBar::OnPageSetupButtonClicked() {
	LONG orientation = AfxGetApp()->GetProfileInt("NotepadSection", "Orientation", 1);
	PageSetupDialog pageSetupDialog(PSD_INHUNDREDTHSOFMILLIMETERS | PSD_MARGINS |
		PSD_ENABLEPAGEPAINTHOOK, orientation, this->printPreviewForm);

	CString pageCount;
	CString pageCurrent;

	CRect clientRect;

	LONG id;

	id = pageSetupDialog.DoModal();
	if (id == IDOK) {
		this->printPreviewForm->notepadForm->printer->SetDC(pageSetupDialog.CreatePrinterDC());

		this->printPreviewForm->notepadForm->printer->Prepare(this->printPreviewForm);
		this->printPreviewForm->m_nCurrentPage = 1;

		Glyph *note = this->printPreviewForm->notepadForm->printer->GetPrintNote();
		CRect clientRect;

		this->printPreviewForm->m_nPages = note->GetLength() / this->printPreviewForm->notepadForm->printer->GetPageLineCount();

		pageCount.Format("/ %d", this->printPreviewForm->m_nPages);
		this->GetDlgItem(IDC_STATIC_PAGECOUNT)->SetWindowText(pageCount);

		pageCurrent.Format("%d", this->printPreviewForm->m_nCurrentPage);
		this->GetDlgItem(IDC_EDIT_CURRENTPAGE)->SetWindowText(pageCurrent);

		this->printPreviewForm->Invalidate();
	}
	GlobalFree(pageSetupDialog.m_psd.hDevMode);
	GlobalFree(pageSetupDialog.m_psd.hDevNames);
}

void PreviewToolBar::OnFirstButtonClicked() {
	CString pageCurrent;

	// 1. 첫 번째 페이지로 이동한다.
	this->printPreviewForm->m_nCurrentPage = 1;

	// 2. 현재 페이지를 출력한다.
	pageCurrent.Format("%d", this->printPreviewForm->m_nCurrentPage);
	this->GetDlgItem(IDC_EDIT_CURRENTPAGE)->SetWindowText(pageCurrent);

	// 3. 미리보기 화면을 갱신한다.
	this->printPreviewForm->Invalidate();
}

void PreviewToolBar::OnPreviousButtonClicked() {
	CString pageCurrent;

	// 1. 이전 번째 페이지로 이동한다.
	this->printPreviewForm->m_nCurrentPage--;
	if (this->printPreviewForm->m_nCurrentPage < 1) {
		this->printPreviewForm->m_nCurrentPage = 1;
	}

	// 2. 현재 페이지를 출력한다.
	pageCurrent.Format("%d", this->printPreviewForm->m_nCurrentPage);
	this->GetDlgItem(IDC_EDIT_CURRENTPAGE)->SetWindowText(pageCurrent);

	// 3. 미리보기 화면을 갱신한다.
	this->printPreviewForm->Invalidate();
}

void PreviewToolBar::OnNextButtonClicked() {
	CString pageCurrent;

	// 1. 다음 번째 페이지로 이동한다.
	this->printPreviewForm->m_nCurrentPage++;
	if (this->printPreviewForm->m_nCurrentPage > this->printPreviewForm->m_nPages) {
		this->printPreviewForm->m_nCurrentPage = this->printPreviewForm->m_nPages;
	}

	// 2. 현재 페이지를 출력한다.
	pageCurrent.Format("%d", this->printPreviewForm->m_nCurrentPage);
	this->GetDlgItem(IDC_EDIT_CURRENTPAGE)->SetWindowText(pageCurrent);

	// 3. 미리보기 화면을 갱신한다.
	this->printPreviewForm->Invalidate();
}

void PreviewToolBar::OnLastButtonClicked() {
	CString pageCurrent;

	// 1. 마지막 번째 페이지로 이동한다.
	this->printPreviewForm->m_nCurrentPage = this->printPreviewForm->m_nPages;

	// 2. 현재 페이지를 출력한다.
	pageCurrent.Format("%d", this->printPreviewForm->m_nCurrentPage);
	this->GetDlgItem(IDC_EDIT_CURRENTPAGE)->SetWindowText(pageCurrent);

	// 3. 미리보기 화면을 갱신한다.
	this->printPreviewForm->Invalidate();
}

void PreviewToolBar::OnCloseButtonClicked() {
	this->printPreviewForm->PostMessage(WM_CLOSE);
}

void PreviewToolBar::OnCurrentPageEditKillFocus() {
	CString pageCurrent;
	
	// 1. 현재 페이지를 읽어온다.
	this->GetDlgItem(IDC_EDIT_CURRENTPAGE)->GetWindowText(pageCurrent);
	this->printPreviewForm->m_nCurrentPage = _ttoi((LPCTSTR)pageCurrent);
	if (this->printPreviewForm->m_nCurrentPage > this->printPreviewForm->m_nPages) {
		this->printPreviewForm->m_nCurrentPage = this->printPreviewForm->m_nPages;
	}

	// 2. 현재 페이지를 적는다.
	pageCurrent.Format("%d", this->printPreviewForm->m_nCurrentPage);
	this->GetDlgItem(IDC_EDIT_CURRENTPAGE)->SetWindowText(pageCurrent);

	// 3. 미리보기 화면을 갱신한다.
	this->printPreviewForm->Invalidate();
}

void PreviewToolBar::OnClose() {
	CDialog::OnClose();
}