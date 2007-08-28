#include "AboutDialog.h"
#define THISCLASS AboutDialog

#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/button.h>

THISCLASS::AboutDialog(wxWindow *parent):
		wxDialog(parent, -1, "About SwisTrack", wxDefaultPosition, wxSize(400, 200), wxDEFAULT_DIALOG_STYLE)	{

	wxPanel *p=new wxPanel(this, wxID_ANY);

	wxBoxSizer *pvs=new wxBoxSizer(wxVERTICAL);
	wxStaticText *title=new wxStaticText(p, -1, "SWISTrack 4.0");
	wxFont f=title->GetFont();
	f.SetWeight(wxFONTWEIGHT_BOLD);
	title->SetFont(f);
	pvs->Add(title);
	pvs->Add(new wxStaticText(p, -1, "A software for tracking mobile robots and other moving things."));
	pvs->Add(new wxStaticText(p, -1, ""));
	pvs->Add(new wxStaticText(p, -1, "(c) 2004-2007 Swarm-Intelligent Systems Group"));
	pvs->Add(new wxStaticText(p, -1, "Swiss Federal Institute of Technology (EPFL)"));
	pvs->Add(new wxStaticText(p, -1, "Lausanne, Switzerland"));
	pvs->Add(new wxStaticText(p, -1, "http://swistrack.sourceforge.net"));

	wxBoxSizer *phs=new wxBoxSizer(wxHORIZONTAL);
	phs->Add(new wxStaticBitmap(p, -1, wxICON(icon_gui)), 0, wxRIGHT, 10);
	phs->Add(pvs);
	p->SetSizer(phs);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(p, 1, wxEXPAND|wxALL, 10);
	vs->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
	vs->Add(new wxButton(this, wxID_OK, "Close"), 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
	SetSizer(vs);
}
