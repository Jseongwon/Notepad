// PrintCommand.cpp
/*
���ϸ�Ī : PrintCommand.cpp
��� : �μ� ��� Ŭ������ �����.
�ۼ��� : �����, ������
�ۼ����� : 2021.01.29
*/
#include"PrintCommand.h"
#include"NotepadForm.h"
#include"PrintPreviewForm.h"
#include"Glyph.h"
#include"Matrix.h"
#include"Font.h"
#include"Printer.h"
#include"PageInfo.h"
#include<afxdlgs.h>
#include"File.h"
#include<afxpriv.h>

PrintCommand::PrintCommand(NotepadForm *notepadForm) {
	this->notepadForm = notepadForm;
}

PrintCommand::~PrintCommand() {

}

void PrintCommand::Execute() {
	CPrintDialog dlg(FALSE);
	DOCINFO di;
	
	CDC *cdc;

	HDC hDC;

	Glyph *note;
	Glyph *line;

	CRect printRect;

	Long pageLineCount;
	Long totalHeight;
	Long current;
	Long i = 0;

	CFont font;
	LOGFONT logFont;

	RECT margin = { 0, };

	CString header;
	CString footer;
	CString fileTitle;
	Long orientation = 1;

	if (dlg.DoModal() == IDOK) {
		//dc�����б�
		hDC = dlg.GetPrinterDC();
		cdc = CDC::FromHandle(hDC);

		//���μ��� ���� ����
		orientation = AfxGetApp()->GetProfileInt("NotepadSection", "Orientation", 1);
		if (orientation != 1) {//���θ��� ������ �Ǿ�������
			DEVMODE *pDevMode = dlg.GetDevMode();
			pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
			pDevMode->dmPaperSize = DMPAPER_TABLOID_EXTRA;
			cdc->ResetDCA(pDevMode);
		}

		this->notepadForm->printer->SetDC(hDC);

		// previewForm ���� �� ����
		PrintPreviewForm *printPreviewForm = new PrintPreviewForm(this->notepadForm);
		printPreviewForm->Create(NULL, "�̸�����");
		printPreviewForm->SendMessage(WM_CLOSE);

		//docInfo �ۼ�
		memset(&di, 0, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		fileTitle = this->notepadForm->file->GetTitle().c_str();
		di.lpszDocName = (LPCTSTR)fileTitle;

		//��Ʈ ����
		//logFont = this->notepadForm->font->FindPrintingLogFont(cdc);
		logFont = this->notepadForm->printer->GetPrintLogFont();
		font.CreateFontIndirectA(&logFont);
		TEXTMETRIC metric;
		CFont *def_font = cdc->SelectObject(&font);
		cdc->GetTextMetrics(&metric);

		//������ ���鼳��
		header = AfxGetApp()->GetProfileString("NotepadSection", "Header", "");
		footer = AfxGetApp()->GetProfileString("NotepadSection", "Footer", "");

		CRect writeRect = this->notepadForm->printer->GetPrintWriteRect();
		pageLineCount = this->notepadForm->printer->GetPageLineCount();
		if (header.Compare("") != 0) {
			writeRect.top += (metric.tmHeight);
			pageLineCount--;
		}
		if (footer.Compare("") != 0) {
			writeRect.bottom -= (metric.tmHeight);
			pageLineCount--;
		}

		CRect headerRect(0, 0, printRect.Width(), printRect.Height() + (metric.tmHeight));
		CRect footerRect(0, printRect.Height() - (metric.tmHeight), printRect.Width(), printRect.Height());
		cdc->SetMapMode(MM_TEXT);

		//�μ� ����
		CRect           r;
		CPrintInfo info;
		info.m_rectDraw.SetRect(writeRect.left, writeRect.top, writeRect.right, writeRect.bottom);

		// ��ť��Ʈ �μ� ����
		BOOL bPrintingOK = cdc->StartDoc(&di);

		// �������� �μ��ϴ� ����
		note = this->notepadForm->printer->GetPrintNote();
		i = 0;
		while (i < note->GetLength()) {
			cdc->StartPage();
			current = 0;
			totalHeight = writeRect.top;
			cdc->DrawText(header, &headerRect, DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
			cdc->DrawText(footer, &footerRect, DT_CENTER | DT_SINGLELINE | DT_TOP);
			while (i < note->GetLength() && current < pageLineCount && totalHeight <= writeRect.bottom) {
				line = note->GetAt(i);
				cdc->TextOut(writeRect.left, metric.tmHeight * current + writeRect.top, line->GetString().c_str());

				current++;
				totalHeight += metric.tmHeight;
				i++;
			}
			cdc->EndPage();
		}

		// ��ť��Ʈ �μ� ����
		if (bPrintingOK) cdc->EndDoc();
		else cdc->AbortDoc();
		cdc->SelectObject(def_font);

		font.DeleteObject();
		//dc�Ҵ�����
		cdc->DeleteDC();
	}
}

#if 0

for (UINT page = info.GetMinPage(); bPrintingOK && totalDone < lengthToGo; page++) {
	// ������ �μ� ����
	cdc->StartPage();
	info.m_nCurPage = page;
	// calc how much text fits on one page
	r = info.m_rectDraw;
	r.bottom = r.top;
	j = 0;
	while (r.bottom < info.m_rectDraw.bottom && totalDone + j < lengthToGo) {
		r.right = info.m_rectDraw.right;

		if ((startAt[j] & 0x80) == 0) {//�����̸�
			j = j + 1;
			onIsKorean = false;
		}
		else if ((startAt[j] & 0x80) != 0) {//�ѱ��̸�
			j = j + 2;
			onIsKorean = true;
		}

		nHeight = cdc->DrawText(startAt, j, r, DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_EXPANDTABS);
	}

	// go one back to assure correct height
	if (r.bottom >= info.m_rectDraw.bottom && onIsKorean == true) {
		j -= 2;
	}
	else if (r.bottom >= info.m_rectDraw.bottom) {
		j--;
	}

	//�Ӹ��� ������ �μ�
	cdc->DrawText(header, &headerRect, DT_CENTER);
	cdc->DrawText(footer, &footerRect, DT_CENTER);

	// print that text
	cdc->DrawText(startAt, j, info.m_rectDraw, DT_WORDBREAK | DT_EDITCONTROL | DT_EXPANDTABS);

	// go to next page


	startAt += j;
	totalDone += j;

	// ������ �μ� �Ϸ�
	bPrintingOK = (cdc->EndPage() > 0);
}
noteString.ReleaseBuffer();

#endif