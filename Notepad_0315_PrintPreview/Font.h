// Font.h
#ifndef _FONT_H
#define _FONT_H
#include <afxwin.h>

class Font {
public:
	Font();
	Font(LOGFONT logFont, COLORREF color, LONG pointSize);
	Font(const Font& source);
	~Font();
	Font& operator =(const Font& source);
	LOGFONT GetLogFont() const;
	COLORREF GetColorRef() const;
	LONG GetPointSize() const;
	LONG GetPixel() const;
	LOGFONT FindPrintingLogFont(CDC* printerDC);

private:
	LOGFONT logFont;
	COLORREF color;
	LONG pointSize;
};

inline LOGFONT Font::GetLogFont() const {
	return this->logFont;
}

inline COLORREF Font::GetColorRef() const {
	return this->color;
}

inline LONG Font::GetPointSize() const {
	return this->pointSize;
}

inline LONG Font::GetPixel() const {
	LONG pixel = this->pointSize * 96 / 72;

	return pixel;
}

#endif // _FONT_H