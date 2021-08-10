// PrintPreviewForm.cpp
/*
파일명칭 : PrintPreviewForm.cpp
기능 : 인쇄 미리보기 윈도우를 만든다.
작성자 : 김승현, 정성원
작성일자 : 2020.08.07
*/
#include "PrintPreviewForm.h"
#include "PreviewToolBar.h"
#include "NotepadForm.h"
#include "PageSetupDialog.h"
#include "Glyph.h"
#include "GlyphFactory.h"
#include "Matrix.h"
#include "Font.h"
#include "Printer.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(PrintPreviewForm, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

PrintPreviewForm::PrintPreviewForm(NotepadForm *notepadForm, BOOL onIsNotepadOrWordMode, BOOL onIsWordWrap) {
	this->previewToolBar = NULL;
	this->notepadForm = notepadForm;
	this->onIsNotepadOrWordMode = onIsNotepadOrWordMode;
	this->onIsWordWrap = onIsWordWrap;
	this->m_nCurrentPage = -1;
}

BOOL PrintPreviewForm::PreCreateWindow(CREATESTRUCT& cs) {
	CFrameWnd::PreCreateWindow(cs);

	//cs.style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

	return TRUE;
}

int PrintPreviewForm::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	CFrameWnd::OnCreate(lpCreateStruct);
	
	this->m_hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_MAINFRAME));
	this->SetIcon(this->m_hIcon, ICON_SMALL);

	this->notepadForm->printer->Prepare(this);
	this->m_nCurrentPage = 1;

	Glyph *note = this->notepadForm->printer->GetPrintNote();

	this->m_nPages = note->GetLength() / this->notepadForm->printer->GetPageLineCount() + 1;

	this->GetClientRect(&this->m_cRect);
	this->m_dHeightRate = 1.0;
	this->m_dWidthRate = 1.0;

	this->previewToolBar = new PreviewToolBar(this);
	this->previewToolBar->Create(PreviewToolBar::IDD, this);
	this->previewToolBar->ShowWindow(SW_SHOW);
	this->previewToolBar->SetWindowPos(NULL, 0, 0, this->m_cRect.right, this->previewToolBar->size.cy, SWP_DRAWFRAME);

	this->SetFocus();

	Invalidate();

	return 0;
}

void PrintPreviewForm::OnSize(UINT nType, int cx, int cy) {
	CFrameWnd::OnSize(nType, cx, cy);
	CRect clientRect;
	this->GetClientRect(&clientRect);

	this->m_dWidthRate = this->m_cRect.Width() / clientRect.Width();
	this->m_dHeightRate = this->m_cRect.Height() / clientRect.Width();

	this->previewToolBar->SetWindowPos(NULL, 0, 0, clientRect.right, this->previewToolBar->size.cy, SWP_DRAWFRAME);

	this->Invalidate();
}

void PrintPreviewForm::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (nChar == 'P' || nChar == 'p') {
		this->previewToolBar->SendMessage(WM_COMMAND, IDC_BUTTON_PRINT);
	}
	else if (nChar == 'U' || nChar == 'u') {
		this->previewToolBar->SendMessage(WM_COMMAND, IDC_BUTTON_PAGESETUP);
	}
	else if (nChar == VK_UP) {
		this->previewToolBar->SendMessage(WM_COMMAND, IDC_BUTTON_FIRST);
	}
	else if (nChar == VK_LEFT) {
		this->previewToolBar->SendMessage(WM_COMMAND, IDC_BUTTON_PREVIOUS);
	}
	else if (nChar == VK_RIGHT) {
		this->previewToolBar->SendMessage(WM_COMMAND, IDC_BUTTON_NEXT);
	}
	else if (nChar == VK_DOWN) {
		this->previewToolBar->SendMessage(WM_COMMAND, IDC_BUTTON_LAST);
	}
	else if ((nChar == 'C' || nChar == 'c') || nChar == VK_ESCAPE) {
		this->previewToolBar->SendMessage(WM_COMMAND, IDC_BUTTON_CLOSE);
	}
}

void PrintPreviewForm::OnPaint() {
	CPaintDC paintDC(this);
	CDC *m_dcPrint = CDC::FromHandle(this->notepadForm->printer->GetHDC());
	CDC tempDC;

	HBITMAP hbmp;
	HBITMAP oldBMP;

	HFONT hFont;
	HFONT oldFont;

	CPen rectPen;
	CPen shadowPen;

	CRect screenRect;
	CRect printRect = this->notepadForm->printer->GetPrintPageRect();
	CRect fillRect;

	LOGFONT printLogFont = this->notepadForm->printer->GetPrintLogFont();

	Glyph *note;
	Glyph *line;

	LONG pageCount = 1;
	LONG totalHeight;
	LONG pageLineCount;
	LONG current = 0;
	LONG i = 0;

	this->GetClientRect(&screenRect);

	tempDC.CreateCompatibleDC(&paintDC);
	hbmp = ::CreateCompatibleBitmap(paintDC, printRect.right, printRect.bottom);
	oldBMP = (HBITMAP)tempDC.SelectObject(hbmp);

	fillRect.right = printRect.right;
	fillRect.bottom = printRect.bottom;

	// SWE 12, 12 , SVE 5, 5를 설정하면 600dpi의 100% 화면으로 볼 수 있다.

	tempDC.FillRect(&fillRect, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));

	hFont = CreateFontIndirect(&printLogFont);
	oldFont = (HFONT)tempDC.SelectObject(hFont);

	CString header = AfxGetApp()->GetProfileString("NotepadSection", "Header", "");
	CString footer = AfxGetApp()->GetProfileString("NotepadSection", "Footer", "");

	TEXTMETRIC metric;
	tempDC.GetTextMetrics(&metric);

	tempDC.SetBkColor(RGB(255, 255, 255));
	tempDC.SetTextColor(RGB(0, 0, 0));

	CRect headerRect = { 0, };
	CRect footerRect = { 0, };
	CRect writeRect = this->notepadForm->printer->GetPrintWriteRect();
	pageLineCount = this->notepadForm->printer->GetPageLineCount();
	if (header.Compare("") != 0) {
		writeRect.top += (metric.tmHeight);
		headerRect.SetRect(printRect.left, printRect.top, printRect.right, writeRect.top);
		pageLineCount--;
	}
	if (footer.Compare("") != 0) {
		writeRect.bottom -= (metric.tmHeight);
		footerRect.SetRect(printRect.left, writeRect.bottom, printRect.right, printRect.bottom);
		pageLineCount--;
	}

	note = this->notepadForm->printer->GetPrintNote();

	i = (this->m_nCurrentPage - 1) * pageLineCount;
	totalHeight = writeRect.top;
	tempDC.DrawText(header, &headerRect, DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
	tempDC.DrawText(footer, &footerRect, DT_CENTER | DT_SINGLELINE | DT_TOP);
	while (i < note->GetLength() && current < pageLineCount && totalHeight <= writeRect.bottom) {
		line = note->GetAt(i);
		tempDC.TextOut(writeRect.left, metric.tmHeight * current + writeRect.top, line->GetString().c_str());

		current++;
		totalHeight += metric.tmHeight;
		i++;
	}
	// 열었을 때의 크기를 적어두고 페인트할 때 현재의 크기로 비율을 구한 뒤 곱해준다.
	// ratioX = OriginWidth() / rect.Width()
	// ratioY = OriginHeight() / rect.Height();
	double rateWidth = m_dcPrint->GetDeviceCaps(HORZSIZE) / 25.4;
	double rateHeight = m_dcPrint->GetDeviceCaps(VERTSIZE) / 25.4;

	Long startXPos = (Long)(screenRect.CenterPoint().x - fillRect.Width() / rateWidth / 2);
	Long startYPos = (Long)((screenRect.CenterPoint().y + this->previewToolBar->size.cy / 2) - fillRect.Height() / rateHeight / 2);// +this->previewToolBar->size.cy;

	paintDC.SelectStockObject(HOLLOW_BRUSH);
	paintDC.SelectObject(&rectPen);
	paintDC.Rectangle(&screenRect);

	paintDC.SelectObject(&shadowPen);

	paintDC.FillRect(&screenRect, CBrush::FromHandle((HBRUSH)GetStockObject(GRAY_BRUSH)));

	paintDC.SetStretchBltMode(HALFTONE); // 가로모드 너비 5 높이 8
	paintDC.StretchBlt(startXPos, startYPos, (Long)(fillRect.Width() / rateWidth), (Long)(fillRect.Height() / rateHeight), &tempDC,
		0, 0, fillRect.Width(), fillRect.Height(), SRCCOPY);

	rectPen.CreatePen(PS_SOLID, 2, GetSysColor(COLOR_WINDOWFRAME));
	shadowPen.CreatePen(PS_SOLID, 3, GetSysColor(COLOR_BTNSHADOW)); // 그림자를 줌.

	SelectObject(tempDC, oldFont);
	DeleteObject(hFont);

	tempDC.SelectObject(oldBMP);
	::DeleteObject(hbmp);
}

void PrintPreviewForm::OnClose() {
	if (this->previewToolBar != 0) {
		this->previewToolBar->OnClose();
	}
	if (this->m_hIcon != 0) {
		DestroyIcon(this->m_hIcon);
	}
	CFrameWnd::OnClose();
}

#if 0
// 3차 테스트
rateWidth = (DOUBLE)printRect.right / (DOUBLE)screenRect.right;
rateHeight = (DOUBLE)printRect.bottom / (DOUBLE)screenRect.bottom;

if (rateWidth > rateHeight) {
	rate = rateHeight;
	offSetPoint.x = (LONG)(printRect.right - screenRect.right * rate);
}
else {
	rate = rateWidth;
	offSetPoint.y = (LONG)(printRect.bottom - screenRect.bottom * rate);
}

imageRect.right = screenRect.right * rate;
imageRect.bottom = screenRect.bottom * rate;

tempDC.SetMapMode(MM_ANISOTROPIC);
tempDC.SetViewportOrg(offSetPoint);
tempDC.SetWindowExt(screenRect.Width(), screenRect.Height());
tempDC.SetViewportExt(imageRect.Width(), imageRect.Height());

#endif

#if 0
// 2차 테스트
CPaintDC paintDC(this);
CDC *m_dcPrint = CDC::FromHandle(this->notepadForm->printer->GetHDC());
CDC tempDC;

HBITMAP hbmp;
HBITMAP oldBMP;

HFONT hFont;
HFONT oldFont;

CPen rectPen;
CPen shadowPen;

CRect screenRect;
CRect printRect = this->notepadForm->printer->GetPrintPageRect();
CRect fillRect;
CRect imageRect;

LOGFONT printLogFont = this->notepadForm->printer->GetPrintLogFont();

Glyph *note;
Glyph *line;

LONG pageCount = 1;
LONG totalHeight;
LONG pageLineCount;
LONG current = 0;
LONG i = 0;

this->GetClientRect(&screenRect);

tempDC.CreateCompatibleDC(&paintDC);

hbmp = ::CreateCompatibleBitmap(paintDC, printRect.right, printRect.bottom);
oldBMP = (HBITMAP)tempDC.SelectObject(hbmp);

fillRect.right = printRect.right + printRect.left;
fillRect.bottom = printRect.bottom + printRect.top;

tempDC.SetMapMode(MM_ANISOTROPIC);
tempDC.SetWindowExt(fillRect.Width(), fillRect.Height());
tempDC.SetViewportExt(fillRect.Width(), fillRect.Height());

tempDC.FillRect(&fillRect, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));

rectPen.CreatePen(PS_SOLID, 2, GetSysColor(COLOR_WINDOWFRAME));
shadowPen.CreatePen(PS_SOLID, 3, GetSysColor(COLOR_BTNSHADOW)); // 그림자를 줌.

printLogFont.lfWeight = FW_THIN;
hFont = CreateFontIndirect(&printLogFont);
oldFont = (HFONT)tempDC.SelectObject(hFont);

CString header = AfxGetApp()->GetProfileString("NotepadSection", "Header", "");
CString footer = AfxGetApp()->GetProfileString("NotepadSection", "Footer", "");

TEXTMETRIC metric;
tempDC.GetTextMetrics(&metric);

tempDC.SetBkColor(RGB(255, 255, 255));
tempDC.SetTextColor(RGB(0, 0, 0));

CRect headerRect = { 0, };
CRect footerRect = { 0, };
CRect writeRect = this->notepadForm->printer->GetPrintWriteRect();
if (header.Compare("") != 0) {
	writeRect.top += (metric.tmHeight);
	headerRect.SetRect(printRect.left, printRect.top, printRect.right, writeRect.top);
}
if (footer.Compare("") != 0) {
	writeRect.bottom -= (metric.tmHeight);
	footerRect.SetRect(printRect.left, writeRect.bottom, printRect.right, printRect.bottom);
}

note = this->notepadForm->printer->GetPrintNote();
pageLineCount = this->notepadForm->printer->GetPageLineCount();

i = (this->m_nCurrentPage - 1) * pageLineCount;
totalHeight = writeRect.top + metric.tmHeight;
tempDC.DrawText(header, &headerRect, DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
tempDC.DrawText(footer, &footerRect, DT_CENTER | DT_SINGLELINE | DT_TOP);
while (i < note->GetLength() && current < pageLineCount && totalHeight <= writeRect.bottom) {
	line = note->GetAt(i);
	tempDC.TextOut(writeRect.left, metric.tmHeight * current + writeRect.top, line->GetString().c_str());

	current++;
	totalHeight += metric.tmHeight;
	i++;
}

int nNum;
int nDen;

// 수직 치수를 기준으로 비율을 사용할 것인지 확인
if (printRect.Width() < printRect.Height()) {
	nNum = screenRect.Height();
	nDen = printRect.Height();

	// 너비와 높이를 비율로 구하고, 가운데 이동을 너비 높이로 이동시켜준다.
	imageRect.right = screenRect.right / 2;
	imageRect.bottom = MulDiv(screenRect.bottom, nNum, nDen);

	imageRect.SetRect(screenRect.CenterPoint().x - imageRect.Width() / 2, this->previewToolBar->size.cy + 5,
		screenRect.CenterPoint().x + imageRect.Width() / 2, screenRect.bottom - 5);
}
else {
	// 너비와 높이를 비율로 구하고, 가운데 이동을 너비 높이로 이동시켜준다.
	imageRect.right = screenRect.right - 10;
	imageRect.bottom = screenRect.bottom;

	imageRect.SetRect(screenRect.CenterPoint().x - imageRect.Width() / 2, screenRect.CenterPoint().y - imageRect.Height() / 2 + this->previewToolBar->size.cy + 5,
		screenRect.CenterPoint().x + imageRect.Width() / 2, screenRect.CenterPoint().y + imageRect.Height() / 2 - 5);
}

paintDC.SelectStockObject(HOLLOW_BRUSH);
paintDC.SelectObject(&rectPen);
paintDC.Rectangle(&screenRect);

paintDC.SelectObject(&shadowPen);

paintDC.FillRect(&screenRect, CBrush::FromHandle((HBRUSH)GetStockObject(GRAY_BRUSH)));

paintDC.StretchBlt(imageRect.left, imageRect.top, imageRect.Width(), imageRect.Height(), &tempDC,
	0, 0, fillRect.Width(), fillRect.Height(), SRCCOPY);

SelectObject(tempDC, oldFont);
DeleteObject(hFont);

tempDC.SelectObject(oldBMP);
::DeleteObject(hbmp);

#endif

#if 0
// 1차 테스트
CPaintDC paintDC(this);
CDC tempDC;
CDC *m_dcPrint = CDC::FromHandle(this->notepadForm->printer->GetHDC());

CPoint ViewportOrg = paintDC.GetViewportOrg();

CRect imageRect;
CRect previewRect;
CRect rectScreen;
CRect pageRect;
CRect rectFill;

CPen rectPen;
CPen shadowPen;

HBITMAP hbmp;
HBITMAP oldBMP;

CString text;

Glyph *note;
Glyph *line;

Long pageCount = 1;
Long totalHeight;
Long pageLineCount;
Long current = 0;
Long i = 0;

this->GetClientRect(&rectScreen);

pageRect = this->notepadForm->printer->GetPreviewPageRect();

previewRect.left = 0;
previewRect.top = 0;
previewRect.right = pageRect.right + pageRect.left;
previewRect.bottom = pageRect.bottom + pageRect.top;
//previewRect = pageRect;

tempDC.CreateCompatibleDC(&paintDC);

hbmp = ::CreateCompatibleBitmap(paintDC, previewRect.Width(), previewRect.Height());
oldBMP = (HBITMAP)tempDC.SelectObject(hbmp);

tempDC.FillRect(&previewRect, CBrush::FromHandle((HBRUSH)GetStockObject(GRAY_BRUSH)));

rectPen.CreatePen(PS_SOLID, 2, GetSysColor(COLOR_WINDOWFRAME));
shadowPen.CreatePen(PS_SOLID, 3, GetSysColor(COLOR_BTNSHADOW)); // 그림자를 줌.

tempDC.SetMapMode(MM_TEXT);   // Page Rectangle is in screen device coords
tempDC.SetViewportOrg(ViewportOrg);
tempDC.SetWindowOrg(0, 0);

tempDC.SelectObject(&shadowPen);

tempDC.MoveTo(pageRect.right + 1, pageRect.top + 3);
tempDC.LineTo(pageRect.right + 1, pageRect.bottom + 1);
tempDC.MoveTo(pageRect.left + 3, pageRect.bottom + 1);
tempDC.LineTo(pageRect.right + 1, pageRect.bottom + 1);

// erase background to white (most paper is white)
rectFill = pageRect;
rectFill.left += 1;
rectFill.top += 1;
rectFill.right -= 2;
rectFill.bottom -= 2;

CBrush br(RGB(240, 240, 240));
tempDC.FillRect(&pageRect, &br);
br.DeleteObject();

CBrush brush(RGB(255, 255, 255));
tempDC.FillRect(&rectFill, &brush);
brush.DeleteObject();

// 인쇄 미리보기 To do
HFONT hFont;
HFONT oldFont;
LOGFONT logFont = this->notepadForm->printer->GetPreviewLogFont(); // this->notepadForm->font->GetLogFont();
hFont = CreateFontIndirect(&logFont);
oldFont = (HFONT)tempDC.SelectObject(hFont);

CString header = AfxGetApp()->GetProfileString("NotepadSection", "Header", "");
CString footer = AfxGetApp()->GetProfileString("NotepadSection", "Footer", "");

TEXTMETRIC metric;
tempDC.GetTextMetrics(&metric);

tempDC.SetBkColor(RGB(255, 255, 255));
tempDC.SetTextColor(RGB(0, 0, 0));

CRect headerRect = { 0, };
CRect footerRect = { 0, };
CRect writeRect = this->notepadForm->printer->GetPreviewWriteRect();
if (header.Compare("") != 0) {
	writeRect.top += (metric.tmHeight);
	headerRect.SetRect(pageRect.left, pageRect.top, pageRect.right, writeRect.top);
}
if (footer.Compare("") != 0) {
	writeRect.bottom -= (metric.tmHeight);
	footerRect.SetRect(pageRect.left, writeRect.bottom, pageRect.right, pageRect.bottom);
}

note = this->notepadForm->printer->GetPrintNote();
pageLineCount = this->notepadForm->printer->GetPageLineCount();

i = (this->m_nCurrentPage - 1) * pageLineCount;
totalHeight = writeRect.top + metric.tmHeight;
tempDC.DrawText(header, &headerRect, DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
tempDC.DrawText(footer, &footerRect, DT_CENTER | DT_SINGLELINE | DT_TOP);
while (i < note->GetLength() && current < pageLineCount && totalHeight <= writeRect.bottom) {
	line = note->GetAt(i);
	tempDC.TextOut(writeRect.left, metric.tmHeight * current + writeRect.top, line->GetString().c_str());

	current++;
	totalHeight += metric.tmHeight;
	i++;
}

int nNum;
int nDen;

// 수직 치수를 기준으로 비율을 사용할 것인지 확인
if (pageRect.Width() < pageRect.Height()) {
	nNum = rectScreen.Height();
	nDen = previewRect.Height();

	// 너비와 높이를 비율로 구하고, 가운데 이동을 너비 높이로 이동시켜준다.
	imageRect.right = MulDiv(rectScreen.right, nNum, nDen);
	imageRect.bottom = MulDiv(rectScreen.bottom, nNum, nDen);

	imageRect.SetRect(rectScreen.CenterPoint().x - imageRect.Width() / 2, this->previewToolBar->size.cy + 5,
		rectScreen.CenterPoint().x + imageRect.Width() / 2, rectScreen.bottom - 5);
}
else {
	nNum = rectScreen.Width();
	nDen = previewRect.Width();
	if (nNum > nDen) {
		nNum = nDen;
		nDen = rectScreen.Width();
	}

	// 너비와 높이를 비율로 구하고, 가운데 이동을 너비 높이로 이동시켜준다.
	imageRect.right = MulDiv(rectScreen.right, nNum, nDen);
	imageRect.bottom = rectScreen.bottom;

	imageRect.SetRect(rectScreen.CenterPoint().x - imageRect.Width() / 2, rectScreen.CenterPoint().y - imageRect.Height() / 2 + this->previewToolBar->size.cy + 5,
		rectScreen.CenterPoint().x + imageRect.Width() / 2, rectScreen.CenterPoint().y + imageRect.Height() / 2 - 5);
}

paintDC.SelectStockObject(HOLLOW_BRUSH);
paintDC.SelectObject(&rectPen);
paintDC.Rectangle(&rectScreen);

paintDC.SelectObject(&shadowPen);

paintDC.FillRect(&rectScreen, CBrush::FromHandle((HBRUSH)GetStockObject(GRAY_BRUSH)));

//paintDC.BitBlt(0, this->previewToolBar->size.cy + 5, previewRect.Width(), previewRect.Height(), &tempDC, 0, 0, SRCCOPY);
paintDC.StretchBlt(imageRect.left, imageRect.top, imageRect.Width(), imageRect.Height(), &tempDC,
	0, 0, previewRect.Width(), previewRect.Height(), SRCCOPY);

SelectObject(tempDC, oldFont);
DeleteObject(hFont);

tempDC.SelectObject(oldBMP);
::DeleteObject(hbmp);

#endif