// Printer.h
#ifndef _PRINTER_H
#define _PRINTER_H
#include <afxwin.h>

class NotepadForm;
class Glyph;
class CWnd;
class PrintPreviewForm;
class PageInfo;
class Printer {
public:
	Printer(NotepadForm *notepadForm = 0);
	~Printer();
	bool Prepare(CWnd *pParentWnd, CDC* dc);
	void Prepare(CWnd *pParentWnd);
	Glyph* GetPrintNote() const;
	CRect GetPrintPageRect();
	CRect GetPreviewPageRect();
	CRect GetPrintPageMargin();
	CRect GetPreviewPageMargin();
	CRect GetPrintWriteRect();
	CRect GetPreviewWriteRect();
	LOGFONT GetPrintLogFont();
	LOGFONT GetPreviewLogFont();
	PageInfo* GetNotepadPageInfo() const;
	PageInfo* GetPreviewPageInfo() const;
	LONG GetPageLineCount() const;
	void SetPageLineCount(LONG pageLineCount);
	HDC GetHDC();
	void SetDC(HDC printerDC);

private:
	NotepadForm *notepadForm;
	Glyph *note;
	HDC printerDC;
	PageInfo *notepadPage;
	PageInfo *previewPage;
	LONG pageLineCount;
};

inline HDC Printer::GetHDC() {
	return this->printerDC;
}

inline Glyph* Printer::GetPrintNote() const {
	return const_cast<Glyph*>(this->note);
}

inline PageInfo* Printer::GetNotepadPageInfo() const {
	return const_cast<PageInfo*>(this->notepadPage);
}

inline PageInfo* Printer::GetPreviewPageInfo() const {
	return const_cast<PageInfo*>(this->previewPage);
}

inline LONG Printer::GetPageLineCount() const {
	return this->pageLineCount;
}

#endif // _PRINTER_H