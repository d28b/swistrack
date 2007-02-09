#include "TCPServerDialog.h"
#define THISCLASS TCPServerDialog

#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/button.h>

THISCLASS::TCPServerDialog(wxWindow *parent):
		wxDialog(parent, -1, "TCP Server Settings", wxDefaultPosition, wxSize(400, 200), wxDEFAULT_DIALOG_STYLE)	{

	wxPanel *p=new wxPanel(this, -1);

	wxBoxSizer *pvs=new wxBoxSizer(wxVERTICAL);
	wxStaticText *title=new wxStaticText(p, -1, "TCP Server Sett");
	wxFont f=title->GetFont();
	f.SetWeight(wxFONTWEIGHT_BOLD);
	title->SetFont(f);
	pvs->Add(title);
	pvs->Add(new wxCheckBox(p, -1, "Enable TCP server."));

	wxBoxSizer *phs=new wxBoxSizer(wxHORIZONTAL);
	phs->Add(new wxStaticText(p, -1, "Port"), 0, wxRIGHT, 10);
	phs->Add(pvs);
	p->SetSizer(phs);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(p, 1, wxEXPAND|wxALL, 10);
	vs->Add(new wxStaticLine(this, -1), 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
	vs->Add(new wxButton(this, wxID_OK, "Close"), 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
	SetSizer(vs);
}
