// FontCommand.cpp
/*
파일명칭 : FontCommand.cpp
기능 : 글꼴 명령어 클래스를 만든다.
작성자 : 김승현, 정성원
작성일자 : 2021.02.03
*/
#include "FontCommand.h"
#include <afxdlgs.h>
#include "NotepadForm.h"
#include "Glyph.h"
#include "Font.h"
#include "Matrix.h"
#include "resource.h"
#include <afxtempl.h>
#if _MFC_VER < 0x0700
#include <..\src\afximpl.h>
#else
#include <..\src\mfc\afximpl.h>
#endif

FontCommand::FontCommand(NotepadForm *notepadForm) {
	this->notepadForm = notepadForm;
}

FontCommand::~FontCommand() {
}

void FontCommand::Execute() {
	LOGFONT logFont = this->notepadForm->font->GetLogFont();
	CFontDialog dlg(&logFont, CF_INITTOLOGFONTSTRUCT | CF_EFFECTS, NULL, this->notepadForm);
	Long pointSize;
	LONG noteCurrent;
	LONG id;
	id = (Long)dlg.DoModal();
	if (id == IDOK) {
		dlg.GetCurrentFont(&logFont);
		if (this->notepadForm->font != NULL) {
			delete this->notepadForm->font;
		}
		pointSize = dlg.GetSize() / 10;
		this->notepadForm->font = new Font(logFont, dlg.GetColor(), pointSize);
		AfxGetApp()->WriteProfileInt("NotepadSection", "PointSize", this->notepadForm->font->GetPointSize());
		
		if (this->notepadForm->matrix != 0) {
			delete this->notepadForm->matrix;
		}
		this->notepadForm->matrix = new Matrix(this->notepadForm);
		if (this->notepadForm->onIsLineWrapping == TRUE) {
			this->notepadForm->note->CombineAll();
			noteCurrent = this->notepadForm->note->GetCurrent();
			this->notepadForm->current = this->notepadForm->note->GetAt(noteCurrent);
			this->notepadForm->SendMessage(WM_COMMAND, IDM_FORMAT_WRAP);
		}
	}
}