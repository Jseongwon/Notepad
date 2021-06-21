// PreviewCommand.cpp
/*
���ϸ�Ī : PreviewCommand.cpp
��� : �̸����� ��� Ŭ������ �����.
�ۼ��� : �����, ������
�ۼ����� : 2021.01.29
*/
#include "PreviewCommand.h"
#include "PrintPreviewForm.h"
#include "NotepadForm.h"
#include "Printer.h"
#include "PageInfo.h"
#include "PageSetupDialog.h"
#include <afxdlgs.h>
#include <afxstat_.h>
#include <afxdisp.h>

PreviewCommand::PreviewCommand(NotepadForm *notepadForm) {
	this->notepadForm = notepadForm;
}

PreviewCommand::~PreviewCommand() {
}

void PreviewCommand::Execute() {
	LONG orientation = AfxGetApp()->GetProfileInt("NotepadSection", "Orientation", 1);
	PageSetupDialog pageSetupDialog(PSD_INHUNDREDTHSOFMILLIMETERS | PSD_MARGINS |
		PSD_ENABLEPAGEPAINTHOOK, orientation, this->notepadForm);

	pageSetupDialog.m_psd.hDevMode = NULL;
	pageSetupDialog.m_psd.hDevNames = NULL;
	pageSetupDialog.m_psd.Flags |= PSD_RETURNDEFAULT;

	pageSetupDialog.DoModal();
	this->notepadForm->printer->SetDC(pageSetupDialog.CreatePrinterDC());

	GlobalFree(pageSetupDialog.m_psd.hDevMode);
	GlobalFree(pageSetupDialog.m_psd.hDevNames);

	PrintPreviewForm *printPreviewForm = new PrintPreviewForm(this->notepadForm);
	printPreviewForm->Create(NULL, "�̸�����");
	printPreviewForm->ShowWindow(SW_SHOW);
	printPreviewForm->UpdateWindow();
}