// PageInfo.cpp
/*
���ϸ�Ī : PageInfo.cpp
��� : �μ� ���� Ŭ������ �����.
�ۼ��� : �����, ������
�ۼ����� : 2021.01.29
*/
#include "PageInfo.h"

PageInfo::PageInfo() {
	this->pageRect = CRect(0, 0, 0, 0);
	this->margin = CRect(0, 0, 0, 0);
	this->writeRect = CRect(0, 0, 0, 0);
	memset(&this->logFont, 0, sizeof(LOGFONT));
	this->orientation = 1;
}

PageInfo::PageInfo(CRect pageRect, CRect margin, CRect writeRect, LOGFONT logFont, LONG orientation) {
	this->pageRect = pageRect;
	this->margin = margin;
	this->writeRect = writeRect;
	this->logFont = logFont;
	this->orientation = orientation;
}

PageInfo::PageInfo(const PageInfo& source) {
	this->pageRect = source.pageRect;
	this->margin = source.margin;
	this->writeRect = source.writeRect;
	this->logFont = source.logFont;
	this->orientation = source.orientation;
}

PageInfo::~PageInfo() {
}

PageInfo& PageInfo::operator =(const PageInfo& source) {
	this->pageRect = source.pageRect;
	this->margin = source.margin;
	this->writeRect = source.writeRect;
	this->logFont = source.logFont;
	this->orientation = source.orientation;

	return *this;
}