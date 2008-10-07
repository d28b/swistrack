#include "AboutDialog.h"
#define THISCLASS AboutDialog

#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include "bitmaps/icon_gui.xpm"

THISCLASS::AboutDialog(wxWindow *parent):
		wxDialog(parent, -1, wxT("About SwisTrack"), wxDefaultPosition, wxSize(400, 250), wxDEFAULT_DIALOG_STYLE)	{

	wxPanel *p = new wxPanel(this, wxID_ANY);

	wxBoxSizer *pvs = new wxBoxSizer(wxVERTICAL);
	wxStaticText *title = new wxStaticText(p, -1, wxT("SwisTrack 4.1"));
	wxFont f = title->GetFont();
	f.SetWeight(wxFONTWEIGHT_BOLD);
	title->SetFont(f);
	pvs->Add(title);
	pvs->Add(new wxStaticText(p, -1, wxT("An open source, general-purpose multi-agent tracking software.")));
	pvs->Add(new wxStaticText(p, -1, wxT("")));
	pvs->Add(new wxStaticText(p, -1, wxT("Maintained by:")));
	pvs->Add(new wxStaticText(p, -1, wxT("Distributed Intelligent Systems and Algorithms Laboratory (DISAL)")));
	pvs->Add(new wxStaticText(p, -1, wxT("École Polytechnique Fédérale de Lausanne (EPFL)")));
	pvs->Add(new wxStaticText(p, -1, wxT("Lausanne, Switzerland")));
	pvs->Add(new wxStaticText(p, -1, wxT("http://en.wikibooks.org/wiki/Swistrack")));

	wxBoxSizer *phs = new wxBoxSizer(wxHORIZONTAL);
	phs->Add(new wxStaticBitmap(p, -1, wxICON(icon_gui)), 0, wxRIGHT, 10);
	phs->Add(pvs);
	p->SetSizer(phs);

	wxBoxSizer *vs = new wxBoxSizer(wxVERTICAL);
	vs->Add(p, 1, wxEXPAND | wxALL, 10);
	vs->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
	vs->Add(new wxButton(this, wxID_OK, wxT("Close")), 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 10);
	SetSizer(vs);
}
