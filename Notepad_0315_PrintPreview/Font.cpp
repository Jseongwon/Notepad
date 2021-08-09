// Font.cpp
/*
파일명칭 : Font.cpp
기능 : 글꼴 클래스를 만든다.
작성자 : 김승현, 정성원
작성일자 : 2021.01.07
*/
#include "Font.h"

Font::Font() {
	memset(&this->logFont, 0, sizeof(LOGFONT));
	memset(&this->color, 0, sizeof(COLORREF));
	this->pointSize = 0;
}

Font::Font(LOGFONT logFont, COLORREF color, LONG pointSize) {
	this->logFont = logFont;
	this->logFont.lfCharSet = HANGUL_CHARSET;
	this->color = color;
	this->pointSize = pointSize;
}

Font::Font(const Font& source) {
	this->logFont = source.logFont;
	this->color = source.color;
	this->pointSize = pointSize;
}

Font::~Font() {
}

Font& Font::operator =(const Font& source) {
	this->logFont = source.logFont;
	this->color = source.color;
	this->pointSize = pointSize;

	return *this;
}

LOGFONT Font::FindPrintingLogFont(CDC* printerDC) {
	LOGFONT logFont;
	bool isSame = false;

	// 1. 공식에 의한 로그폰트를 구한다. -MulDiv(pointSize, printerDC->GetDeviceCaps(LOGPIXELSY), 72);
	logFont = this->logFont;
	logFont.lfHeight = -MulDiv(this->pointSize, printerDC->GetDeviceCaps(LOGPIXELSY), 72);

	// 2. 인쇄할 때 불일치하는지 확인한다.
	CRect rRect;
	CRect printRect(0, 0, printerDC->GetDeviceCaps(HORZRES), printerDC->GetDeviceCaps(VERTRES));

	HFONT hFont = CreateFontIndirect(&logFont);
	HFONT oldFont = (HFONT)printerDC->SelectObject(hFont);

	CSize iSize = printerDC->GetTextExtent("i");

	int iCount = printRect.Width() / iSize.cx;
	int compareICount = 0;

	char characterI[9999];
	int nHeight = iSize.cy;
	int i = 0;
	while (rRect.right < printRect.Width() - iSize.cy && nHeight == iSize.cy) { // 인쇄의 가로 길이 - 한 글자의 너비(딱 맞게 세면 한 글자를 더 세게 되므로 빼야됨)
		rRect.right = printRect.right;
		characterI[i] = 'i';
		characterI[i + 1] = '\0';
		i++;

		compareICount++;

		// 검사할 문자열, 문자 개수, 구할 Rect, Flag => 반환값 높이
		nHeight = printerDC->DrawText(characterI, i, rRect, DT_CALCRECT);
	}
	if (compareICount == iCount) {
		isSame = true;
	}

	// 3. 불일치하면
	if (isSame != true) {
		// 3.1. 한 치수 작게 로그폰트를 구한다.
		logFont.lfHeight = -MulDiv(this->pointSize - 1, printerDC->GetDeviceCaps(LOGPIXELSY), 72);
	}

	// 4. 일치하면
	else {
		// 4.1. 한 치수 크게 로그폰트를 구한다.
		logFont.lfHeight = -MulDiv(this->pointSize + 1, printerDC->GetDeviceCaps(LOGPIXELSY), 72);
	}

	// 5. 일치하면 인쇄할 때 불일치하는지 확인한다.
	printerDC->SelectObject(oldFont);
	DeleteObject(hFont);

	hFont = CreateFontIndirect(&logFont);
	oldFont = (HFONT)printerDC->SelectObject(hFont);

	iSize = printerDC->GetTextExtent("i");
	iCount = printRect.Width() / iSize.cx;
	rRect.right = 0;

	compareICount = 0;
	i = 0;
	while (rRect.right < printRect.Width() - iSize.cy && nHeight == iSize.cy && isSame == true) { // 인쇄의 가로 길이 - 한 글자의 너비(딱 맞게 세면 한 글자를 더 세게 되므로 빼야됨)
		rRect.right = printRect.right;
		characterI[i] = 'i';
		characterI[i + 1] = '\0';
		i++;

		compareICount++;

		// 검사할 문자열, 문자 개수, 구할 Rect, Flag => 반환값 높이
		nHeight = printerDC->DrawText(characterI, i, rRect, DT_CALCRECT);
	}
	if (compareICount == iCount) {
		isSame = true;
	}

	// 6. 불일치하면
	if (isSame != true) {
		// 6.1. 한 치수 작게 로그폰트를 구한다.
		logFont.lfHeight = -MulDiv(this->pointSize - 1, printerDC->GetDeviceCaps(LOGPIXELSY), 72);
	}

	printerDC->SelectObject(oldFont);
	DeleteObject(hFont);

	return logFont;
}