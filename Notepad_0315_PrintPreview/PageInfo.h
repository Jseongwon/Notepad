// PageInfo.h
#ifndef _PAGEINFO_H
#define _PAGEINFO_H
#include <afxwin.h>

class PageInfo {
public:
	PageInfo();
	PageInfo(CRect pageRect, CRect margin, CRect writeRect, LOGFONT logFont, LONG orientation = 1);
	PageInfo(const PageInfo& source);
	~PageInfo();
	CRect& GetPageRect() const;
	CRect& GetMargin() const;
	CRect& GetWriteRect() const;
	LOGFONT GetLogFont() const;
	LONG GetOrientation() const;
	PageInfo& operator =(const PageInfo& source);

private:
	CRect pageRect;
	CRect margin;
	CRect writeRect;
	LOGFONT logFont;
	LONG orientation;
};

inline CRect& PageInfo::GetPageRect() const {
	return const_cast<CRect&>(this->pageRect);
}

inline CRect& PageInfo::GetMargin() const {
	return  const_cast<CRect&>(this->margin);
}

inline CRect& PageInfo::GetWriteRect() const {
	return  const_cast<CRect&>(this->writeRect);
}

inline LOGFONT PageInfo::GetLogFont() const {
	return this->logFont;
}

inline LONG PageInfo::GetOrientation() const {
	return this->orientation;
}

#endif // _PAGEINFO_H