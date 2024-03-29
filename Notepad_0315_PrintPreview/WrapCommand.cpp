// WrapCommand.cpp
/*
파일명칭 : WrapCommand.cpp
기능 : 자동개행 명령 클래스를 만든다.
작성자 : 정성원
작성일자 : 2020.10.14
*/
#include "WrapCommand.h"
#include "NotepadForm.h"
#include "Glyph.h"
#include "GlyphFactory.h"
#include "Matrix.h"

WrapCommand::WrapCommand(NotepadForm *notepadForm) {
	this->notepadForm = notepadForm;
}

WrapCommand::~WrapCommand() {
}

void WrapCommand::Execute() {
	GlyphFactory glyphFactory;
	Glyph *line;
	Glyph *dummyLine;

	RECT rect;

	Long splitCount;
	Long height = this->notepadForm->matrix->GetHeight();
	Long current = 0;
	Long noteIndex = 0;
	Long noteCurrent = this->notepadForm->note->GetCurrent();
	Long lineCurrent = this->notepadForm->current->GetCurrent();
	Long i = 0;

	bool onIsGreaterThanScreen;
	// 1. 잘리는 줄의 개수를 구한다.(2020.01.22)
	onIsGreaterThanScreen = this->notepadForm->matrix->GetOnIsGreaterThanScreen();// 한글자가 화면의 크기를 넘어설때 문제가 발생한다.

	// 2. 잘리는 줄의 개수가 화면 크기보다 많으면 수직 스크롤바가 있는 것으로 지정한다.(2020.01.22)
	if (onIsGreaterThanScreen == true) {
		this->notepadForm->onIsVerticalScrollBar = TRUE;
	}
	else {
		this->notepadForm->onIsVerticalScrollBar = FALSE;
	}

	this->notepadForm->GetClientRect(&rect);
	if (this->notepadForm->onIsVerticalScrollBar == TRUE) {
		rect.right -= GetSystemMetrics(SM_CXVSCROLL);
	}

	// 1. 이전 줄들이 자동개행 될 경우 미리 현재 줄을 구한다.
	while (i < noteCurrent) {
		line = this->notepadForm->note->GetAt(i);
		splitCount = this->notepadForm->matrix->GetSplitCount(line);
		if (splitCount > 0) {
			noteIndex += splitCount;
		}
		i++;
	}
	if (noteIndex > 0) {
		noteCurrent += noteIndex;
	}

	// 2. 자동개행한다.
	i = 0;
	while (i < this->notepadForm->note->GetLength()) {
		line = this->notepadForm->note->GetAt(i);
		current = this->notepadForm->matrix->GetSplitCurrent(line, rect.right);
		if (current > 0) {
			dummyLine = glyphFactory.Create((char*)"\n", false);
			line->Split(dummyLine, current);
			this->notepadForm->note->Add(i + 1, dummyLine);
		}
		else if (current == 0) {
			dummyLine = glyphFactory.Create((char*)"\n", false);
			line->Split(dummyLine, 1);
			this->notepadForm->note->Add(i + 1, dummyLine);
		}
		i++;
	}

	// 3. 종이의 현재 위치와 줄의 현재 위치를 계산한다.
	line = this->notepadForm->note->GetAt(noteCurrent);
	i = noteCurrent;
	while (i < this->notepadForm->note->GetLength() - 1 && lineCurrent > line->GetLength()) {
		noteCurrent++;
		lineCurrent -= line->GetLength();

		line = this->notepadForm->note->GetAt(i + 1);
		i++;
	}

	// 4. 계산된 위치로 이동한다.
	this->notepadForm->note->Move(noteCurrent);
	this->notepadForm->current = this->notepadForm->note->GetAt(noteCurrent);
	this->notepadForm->current->Move(lineCurrent);
}