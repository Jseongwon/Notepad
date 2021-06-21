// Font.cpp
/*
���ϸ�Ī : Font.cpp
��� : �۲� Ŭ������ �����.
�ۼ��� : �����, ������
�ۼ����� : 2021.01.07
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