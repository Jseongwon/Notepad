// PageSetUpCommand.cpp
/*
파일명칭 : PageSetUpCommand.cpp
기능 : 페이지 설정 명령 클래스를 만든다.
작성자 : 김승현, 정성원
작성일자 : 2021.01.29
*/

#include"PageSetupCommand.h"
#include"NotepadForm.h"
#include"PageSetupDialog.h"
#include"Printer.h"
#include"PageInfo.h"

PageSetupCommand::PageSetupCommand(NotepadForm *notepadForm) {
	this->notepadForm = notepadForm;
}

PageSetupCommand::~PageSetupCommand() {

}

void PageSetupCommand::Execute() {
	LONG orientation = AfxGetApp()->GetProfileInt("NotepadSection", "Orientation", 1);
	PageSetupDialog pageSetupDialog(PSD_INHUNDREDTHSOFMILLIMETERS | PSD_MARGINS |
		PSD_ENABLEPAGEPAINTHOOK, orientation, this->notepadForm);
	
	AfxGetApp()->SelectPrinter(pageSetupDialog.m_psd.hDevNames, pageSetupDialog.m_psd.hDevMode);

	LONG id;
	id = pageSetupDialog.DoModal();
	if (id == IDOK) {
		this->notepadForm->printer->SetDC(pageSetupDialog.CreatePrinterDC());
	}

	GlobalFree(pageSetupDialog.m_psd.hDevMode);
	GlobalFree(pageSetupDialog.m_psd.hDevNames);
}

#if 0

PAGESETUPDLG* psd;
psd = new PAGESETUPDLG;

psd->hDevMode = NULL;
psd->hDevNames = NULL;

ZeroMemory(psd, sizeof(*psd));
psd->lStructSize = sizeof(*psd);
psd->Flags = PSD_RETURNDEFAULT;
PageSetupDlg(psd);
psd->Flags &= ~PSD_RETURNDEFAULT;


psd->Flags &= ~PSD_ENABLEPAGESETUPTEMPLATEHANDLE;
LPDEVMODE devmode = (LPDEVMODE)::GlobalLock(psd->hDevMode);
devmode->dmFields |= DM_ORIENTATION;
devmode->dmOrientation = DMORIENT_LANDSCAPE;

::GlobalUnlock(devmode);
PageSetupDlg(psd);

#endif