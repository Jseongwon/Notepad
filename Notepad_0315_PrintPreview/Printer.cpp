// Printer.cpp
/*
파일명칭 : Printer.cpp
기능 : 인쇄기 클래스를 만든다.
작성자 : 김승현, 정성원
작성일자 : 2020.08.21
*/
#include "Printer.h"
#include "PageInfo.h"
#include "NotepadForm.h"
#include "Glyph.h"
#include "GlyphFactory.h"
#include "Font.h"
#include "Matrix.h"
#if _MFC_VER < 0x0700
#include <..\src\afximpl.h>
#else
#include <..\src\mfc\afximpl.h>
#endif
#include "PrintPreviewForm.h"
#include "PreviewToolBar.h"

Printer::Printer(NotepadForm *notepadForm) {
	this->notepadForm = notepadForm;
	this->printerDC = 0;
	this->notepadPage = 0;
	this->previewPage = 0;
}

Printer::~Printer() {
	if (this->printerDC != 0) {
		DeleteDC(this->printerDC);
	}
	if (this->note != 0) {
		delete this->note;
	}
	if (this->notepadPage != 0) {
		delete this->notepadPage;
	}
	if (this->previewPage != 0) {
		delete this->previewPage;
	}
}

bool Printer::Prepare(CWnd *pParentWnd, CDC* dc) {
	CSize printSize;

	CDC *printerDC = CDC::FromHandle(this->printerDC);

	HFONT hFont;
	HFONT oldFont;

	string str;
	const char(*text);

	// 1. 인쇄 용지 크기를 구한다.
	printSize.cx = printerDC->GetDeviceCaps(PHYSICALWIDTH);
	printSize.cy = printerDC->GetDeviceCaps(PHYSICALHEIGHT);

	return true;
}

void Printer::Prepare(CWnd *pParentWnd) {
	CRect printRect;
	CRect previewRect;
	CRect printMargin;
	CRect previewMargin;
	CRect printWriteRect;
	CRect previewWriteRect;
	CRect previewScreenRect;

	CSize cSize;

	CFont printFont;
	CFont previewFont;

	CSize printSize;
	CSize previewSize;

	CDC *dc = pParentWnd->GetDC();
	CDC *printerDC = CDC::FromHandle(this->printerDC);

	HFONT hFont;
	HFONT oldFont;

	LOGFONT notepadFont;
	LOGFONT printLogFont;
	LOGFONT previewLogFont;

	TEXTMETRIC metric;

	GlyphFactory glyphFactory;
	Glyph *line;
	Glyph *dummyLine;

	string str;
	const char(*text);

	LONG paperWidth;
	LONG paperHeight;
	LONG rate;
	LONG basePPi = 1440;
	LONG pointSize = this->notepadForm->font->GetPointSize();
	LONG orientation = AfxGetApp()->GetProfileInt("NotepadSection", "Orientation", 1);
	LONG ppi = afxData.cyPixelsPerInch;
	LONG plus = 1;
	LONG aveCharWidth;
	LONG rowCount;
	LONG columnCount;
	LONG totalWidth;
	LONG current = 0;
	LONG j;
	LONG i = 0;
	LONG magnification = 100;

	DOUBLE inch = 25.4;

	if (ppi == 120) {
		magnification = 125;
	}
	else if (ppi == 144) {
		magnification = 150;
	}
	else if (ppi == 168) {
		magnification = 175;
	}

	// 1. 인쇄 용지 크기를 구한다.
	printSize.cx = printerDC->GetDeviceCaps(PHYSICALWIDTH);
	printSize.cy = printerDC->GetDeviceCaps(PHYSICALHEIGHT);

	printRect.left = 0;
	printRect.top = 0;
	printRect.right = printSize.cx;
	printRect.bottom = printSize.cy;

	paperWidth = printerDC->GetDeviceCaps(HORZSIZE);
	paperHeight = printerDC->GetDeviceCaps(VERTSIZE);

	// 2. 인쇄 화면의 폰트를 구한다.
	notepadFont = this->notepadForm->font->GetLogFont();

	printLogFont = notepadFont;
	printLogFont.lfHeight = -MulDiv(pointSize, printerDC->GetDeviceCaps(LOGPIXELSY), 72);

	// 3. 인쇄 화면의 줄 개수와 열 개수를 구한다.
	hFont = CreateFontIndirect(&printLogFont);
	oldFont = (HFONT)dc->SelectObject(hFont);

	dc->GetTextMetrics(&metric);

	aveCharWidth = (LONG)(metric.tmAveCharWidth / 2);

	rowCount = printRect.Height() / metric.tmHeight;
	columnCount = (LONG)floor(printRect.Width() / aveCharWidth * 1.0 + 0.5);

	printRect.SetRect(0, 0, columnCount * aveCharWidth, rowCount * metric.tmHeight);
	printRect.SetRect(printSize.cx / 2 - printRect.Width() / 2, printSize.cy / 2 - printRect.Height() / 2,
		printSize.cx / 2 + printRect.Width() / 2, printSize.cy / 2 + printRect.Height() / 2);

	// 4. 여백을 구한다.
	rate = (LONG)floor(printSize.cy / paperHeight * 1.0 + 0.5);
	printMargin.left = AfxGetApp()->GetProfileInt("NotepadSection", "LEFT", 20) * rate;
	printMargin.top = AfxGetApp()->GetProfileInt("NotepadSection", "TOP", 20) * rate;
	printMargin.right = AfxGetApp()->GetProfileInt("NotepadSection", "RIGHT", 20) * rate;
	printMargin.bottom = AfxGetApp()->GetProfileInt("NotepadSection", "BOTTOM", 20) * rate;

	// 5. 인쇄 용지의 적을 범위를 구한다.
	printWriteRect = printRect;
	printWriteRect.left += printMargin.left + 3;
	printWriteRect.top += printMargin.top + 3;
	printWriteRect.right -= printMargin.right + 3;
	printWriteRect.bottom -= printMargin.bottom + 3;

	dc->SelectObject(oldFont);
	DeleteObject(hFont);

	// 6. 미리보기 화면의 폰트를 구한다.
	previewLogFont = notepadFont;
	previewLogFont.lfHeight = -MulDiv(pointSize, ppi, 72);

	hFont = CreateFontIndirect(&previewLogFont);
	oldFont = (HFONT)dc->SelectObject(hFont);

	dc->GetTextMetrics(&metric);

	// 7. 미리보기 용지 크기를 구한다.
	rate = (LONG)floor(previewSize.cy / paperHeight * 1.0 + 0.5);

	aveCharWidth = (LONG)(metric.tmAveCharWidth / 2);

	previewSize.cx = aveCharWidth * columnCount;// * magnification / 100;
	previewSize.cy = metric.tmHeight * rowCount;// * magnification / 100;

	pParentWnd->GetClientRect(&previewScreenRect);

	previewRect.SetRect(0, 0, previewSize.cx, previewSize.cy);
	
	if (previewSize.cx < previewSize.cy) {
		previewRect.OffsetRect(previewScreenRect.CenterPoint().x - previewRect.Size().cx / 2, 2);
	}
	else {
		previewRect.OffsetRect(2, 2);
	}

	// 8. 미리보기 여백을 구한다.
	previewMargin.left = AfxGetApp()->GetProfileInt("NotepadSection", "LEFT", 20) * rate;
	previewMargin.top = AfxGetApp()->GetProfileInt("NotepadSection", "TOP", 20) * rate;
	previewMargin.right = AfxGetApp()->GetProfileInt("NotepadSection", "RIGHT", 20) * rate;
	previewMargin.bottom = AfxGetApp()->GetProfileInt("NotepadSection", "BOTTOM", 20) * rate;

	// 9. 미리보기의 적을 범위를 구한다.
	previewWriteRect = previewRect;
	previewWriteRect.left += previewMargin.left + 3;
	previewWriteRect.top += previewMargin.top + 3;
	previewWriteRect.right -= previewMargin.right + 3;
	previewWriteRect.bottom -= previewMargin.bottom + 3;

	dc->SelectObject(oldFont);
	DeleteObject(hFont);

	// 10. 페이지 정보들을 만든다.
	if (this->notepadPage != 0) {
		delete this->notepadPage;
	}
	this->notepadPage = new PageInfo(printRect, printMargin, printWriteRect, printLogFont, orientation);

	if (this->previewPage != 0) {
		delete this->previewPage;
	}
	this->previewPage = new PageInfo(previewRect, previewMargin, previewWriteRect, previewLogFont, orientation);

	// 11. 미리보기 노트를 만든다.
	hFont = CreateFontIndirect(&printLogFont);
	oldFont = (HFONT)dc->SelectObject(hFont);

	if (this->note != 0) {
		delete this->note;
	}

	this->note = glyphFactory.Create(0);
	line = glyphFactory.Create((char*)"\n");
	this->note->Add(line);
	this->note->First();

	this->note->Paste(this->notepadForm->note->GetString());
	this->note->First();

	// 12. 자동개행한다.
	while (i < this->note->GetLength()) {
		line = this->note->GetAt(i);
		totalWidth = 0;
		j = 0;
		while (j < line->GetLength() && totalWidth <= printWriteRect.Width()) {
			str = line->GetAt(j)->GetString();
			text = str.c_str();

			cSize = dc->GetTextExtent(text);
			totalWidth += cSize.cx;

			current = j;
			j++;
		}
		if (totalWidth <= printWriteRect.Width()) {
			current = -1;
		}
		if (current > 0) {
			dummyLine = glyphFactory.Create((char*)"\n", false);
			line->Split(dummyLine, current);
			this->note->Add(i + 1, dummyLine);
		}
		i++;
	}

	dc->GetTextMetrics(&metric);

	pageLineCount = printWriteRect.Height() / metric.tmHeight;

	SelectObject(*dc, oldFont);
	DeleteObject(hFont);

	pParentWnd->ReleaseDC(dc);
}

CRect Printer::GetPrintPageRect() {
	return this->notepadPage->GetPageRect();
}

CRect Printer::GetPreviewPageRect() {
	return this->previewPage->GetPageRect();
}

CRect Printer::GetPrintPageMargin() {
	return this->notepadPage->GetMargin();
}

CRect Printer::GetPreviewPageMargin() {
	return this->previewPage->GetMargin();
}

CRect Printer::GetPrintWriteRect() {
	return this->notepadPage->GetWriteRect();
}

CRect Printer::GetPreviewWriteRect() {
	return this->previewPage->GetWriteRect();
}

LOGFONT Printer::GetPrintLogFont() {
	return this->notepadPage->GetLogFont();
}

LOGFONT Printer::GetPreviewLogFont() { // 폰트는 제대로 구했고 여백이 문제. 여백을 참고해서 구하자.
	return this->previewPage->GetLogFont();
}

void Printer::SetDC(HDC printerDC) {
	if (this->printerDC != 0) {
		DeleteDC(this->printerDC);
	}
	this->printerDC = printerDC;
}

void Printer::SetPageLineCount(LONG pageLineCount) {
	this->pageLineCount = pageLineCount;
}

#if 0

CSize ratio;


int nNum;
int nDen;

// 수직 치수를 기준으로 비율을 사용할 것인지 확인
nNum = previewScreenRect.Height();
nDen = previewSize.cy;

// 조정된 너비가 더 큰 경우 너비를 기준으로 비율 측정
if (MulDiv(previewScreenRect.Width(), nNum, nDen) > previewSize.cx) {
	nNum = previewScreenRect.Width();
	nDen = previewSize.cx;
}

ratio.cx = nNum;
ratio.cy = nDen;

// 비율로 미리보기 용지 크기 줄인다.
previewSize.cx = MulDiv(previewSize.cx, ratio.cx, ratio.cy);
previewSize.cy = MulDiv(previewSize.cy, ratio.cx, ratio.cy);

//LONG previewPPI = MulDiv(ppi, ratio.cx, ratio.cy);

// 비율로 여백을 줄인다.
previewMargin.left = MulDiv(previewMargin.left, ratio.cx, ratio.cy);
previewMargin.top = MulDiv(previewMargin.top, ratio.cx, ratio.cy);
previewMargin.right = MulDiv(previewMargin.right, ratio.cx, ratio.cy);
previewMargin.bottom = MulDiv(previewMargin.bottom, ratio.cx, ratio.cy);




// 용지를 중앙으로 이동
previewRect.OffsetRect((previewScreenRect.Width() - previewRect.Size().cx) / 2 - 1,
(previewScreenRect.Height() - previewRect.Size().cy) / 2 - 1 + 45); // 45는 툴바의 크기


#endif

#if 0


void Printer::CreateNotePageInfo(NotepadForm *notepadForm) {
	CRect printRect;
	CRect printMargin;
	CRect printWriteRect;

	LOGFONT notepadFont;
	LOGFONT printerLogFont;

	HFONT hFont;
	HFONT oldFont;

	TEXTMETRIC metric;

	LONG rowCount = 0;
	LONG columnCount = 0;
	LONG letterWidth;
	LONG letterHeight;

	int nNum;
	int nDen;

	CDC *dc = CDC::FromHandle(this->printerDC);

	// 1. 인쇄 용지 크기를 구한다.
	printRect.left = 0;
	printRect.top = 0;
	printRect.right = dc->GetDeviceCaps(HORZRES);
	printRect.bottom = dc->GetDeviceCaps(VERTRES);

	dc->DPtoLP(&printRect);

	// 2. 인쇄 용지의 폰트를 구한다.
	notepadFont = this->notepadForm->font->GetLogFont();
	printerLogFont = notepadFont;
	printerLogFont.lfHeight = -MulDiv(printerLogFont.lfHeight, GetDeviceCaps(this->printerDC, LOGPIXELSY), 72);
	printerLogFont.lfWidth = 0;

	// 3. 글자 크기를 구한다.
	hFont = CreateFontIndirect(&printerLogFont);
	oldFont = (HFONT)dc->SelectObject(hFont);
	dc->SetTextColor(this->notepadForm->font->GetColorRef());
	dc->GetTextMetrics(&metric);

	// 4. 줄의 개수와 칸의 개수를 구한다.
	columnCount = printRect.right / metric.tmAveCharWidth;
	rowCount = printRect.bottom / metric.tmHeight;

	// 5. 여백을 구한다.
	printMargin.left = AfxGetApp()->GetProfileInt("NotepadSection", "LEFT", 20) * 100;
	printMargin.top = AfxGetApp()->GetProfileInt("NotepadSection", "TOP", 20) * 100;
	printMargin.right = AfxGetApp()->GetProfileInt("NotepadSection", "RIGHT", 20) * 100;
	printMargin.bottom = AfxGetApp()->GetProfileInt("NotepadSection", "BOTTOM", 20) * 100;

	// 6. 인쇄 용지의 적을 범위를 구한다.
	printWriteRect = printRect;
	printWriteRect.left += printMargin.left;
	printWriteRect.top += printMargin.top;
	printWriteRect.right -= printMargin.right;
	printWriteRect.bottom -= printMargin.bottom;

	if (this->notepadPage != 0) {
		delete this->notepadPage;
	}
	this->notepadPage = new PageInfo(printRect, printMargin, printWriteRect, printerLogFont);
}
/*
1. 인쇄 용지 크기를 구한다.
dc에서 구한다.
2. 줄의 개수와 칸의 개수를 구한다.
GetTextMetric
3. 여백을 구한다.
4. 미리보기 용지 크기를 구한다.
5. 글자 크기를 구한다.
6. 글자 크기로 폰트를 설정한다.
7. 미리보기 용지의 여백을 구한다.
*/
void Printer::CreatePreviewPageInfo(CWnd *previewForm) {// 여기에서 만들때 인쇄, 미리보기를 모두 설정한다.
	CRect previewRect;
	CRect previewMargin;
	CRect previewWriteRect;
	CRect previewScreenRect;

	CRect printRect = this->notepadPage->GetPageRect();
	CRect printMargin = this->notepadPage->GetMargin();

	HFONT hFont;
	HFONT oldFont;

	CSize previewSize;

	LOGFONT notepadFont;
	LOGFONT printerLogFont;
	LOGFONT previewLogFont;

	TEXTMETRIC metric;

	CDC *dc = CDC::FromHandle(this->printerDC);

	LONG rowCount = 0;
	LONG columnCount = 0;
	LONG letterWidth;
	LONG letterHeight;

	int nNum;
	int nDen;

	notepadFont = this->notepadForm->font->GetLogFont();
	printerLogFont = notepadFont;
	printerLogFont.lfHeight = -MulDiv(printerLogFont.lfHeight, GetDeviceCaps(this->printerDC, LOGPIXELSY), 72);
	printerLogFont.lfWidth = 0;


	// 3. 글자 크기를 구한다.
	hFont = CreateFontIndirect(&printerLogFont);
	oldFont = (HFONT)dc->SelectObject(hFont);
	dc->SetTextColor(this->notepadForm->font->GetColorRef());
	dc->GetTextMetrics(&metric);

	// 4. 줄의 개수와 칸의 개수를 구한다.
	columnCount = printRect.right / metric.tmAveCharWidth;
	rowCount = printRect.bottom / metric.tmHeight;

	// 7. 미리보기 용지 크기를 구한다.
	previewRect = printRect;
	dc->DPtoLP(&previewRect);

	previewMargin = printMargin;
	dc->DPtoLP(&previewMargin);

	previewForm->GetClientRect(&previewScreenRect);

	previewSize.cx = MulDiv(printRect.right, ppi, dc->GetDeviceCaps(LOGPIXELSX));
	previewSize.cy = MulDiv(printRect.bottom, ppi, dc->GetDeviceCaps(LOGPIXELSY));

	previewMargin.left = MulDiv(printMargin.left, ppi, dc->GetDeviceCaps(LOGPIXELSX));
	previewMargin.top = MulDiv(printMargin.top, ppi, dc->GetDeviceCaps(LOGPIXELSY));
	previewMargin.right = MulDiv(printMargin.right, ppi, dc->GetDeviceCaps(LOGPIXELSX));
	previewMargin.bottom = MulDiv(printMargin.bottom, ppi, dc->GetDeviceCaps(LOGPIXELSY));

	// 수직 치수를 기준으로 비율을 사용할 것인지 확인
	nNum = previewScreenRect.bottom;
	nDen = previewSize.cy;

	// 조정된 너비가 더 큰 경우 너비를 기준으로 비율 측정
	if (MulDiv(previewScreenRect.right, nNum, nDen) > previewSize.cx) {
		nNum = previewScreenRect.right;
		nDen = previewSize.cx;
	}

	this->ratio.cx = nNum;
	this->ratio.cy = nDen;

	// 비율로 미리보기 용지 크기 줄인다.
	previewSize.cx = MulDiv(previewSize.cx, this->ratio.cx, this->ratio.cy);
	previewSize.cy = MulDiv(previewSize.cy, this->ratio.cx, this->ratio.cy);

	// 비율로 여백을 줄인다.
	previewMargin.left = MulDiv(previewMargin.left, this->ratio.cx, this->ratio.cy);
	previewMargin.top = MulDiv(previewMargin.top, this->ratio.cx, this->ratio.cy);
	previewMargin.right = MulDiv(previewMargin.right, this->ratio.cx, this->ratio.cy);
	previewMargin.bottom = MulDiv(previewMargin.bottom, this->ratio.cx, this->ratio.cy);

	// 미리보기 용지 크기를 구한다.
	previewRect.SetRect(3, 3, previewSize.cx - 3, previewSize.cy - 3);

	// 용지를 중앙으로 이동
	previewRect.OffsetRect((previewScreenRect.right - previewRect.Size().cx) / 2 - 1,
		(previewScreenRect.bottom - previewRect.Size().cy) / 2 - 1);

	// 8. 미리보기의 적을 범위를 구한다. 구한 용지에서 여백빼기.
	previewWriteRect = previewRect;
	previewRect.left += previewMargin.left;
	previewRect.top += previewMargin.top;
	previewRect.right -= previewMargin.right;
	previewRect.bottom -= previewMargin.bottom;

	// 9. 글자 크기를 구한다.
	letterWidth = previewSize.cx / columnCount;
	letterHeight = previewSize.cy / rowCount;

	// 10. 미리보기 용지의 폰트를 구한다.
	CFont font;
	font.CreateFontA(letterHeight, letterWidth, notepadFont.lfEscapement, notepadFont.lfOrientation, notepadFont.lfWeight,
		notepadFont.lfItalic, notepadFont.lfUnderline, notepadFont.lfStrikeOut, notepadFont.lfCharSet, notepadFont.lfOutPrecision,
		notepadFont.lfClipPrecision, notepadFont.lfQuality, notepadFont.lfPitchAndFamily, notepadFont.lfFaceName);

	font.GetLogFont(&previewLogFont);

	dc->SelectObject(oldFont);
	DeleteObject(hFont);
}


#endif