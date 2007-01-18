#include "AboutDialog.h"
#define THISCLASS AboutDialog

THISCLASS::AboutDialog(wxWindow *parent):
		wxDialog(parent, -1, "About SwisTrack", wxDefaultPosition, wxSize(400,150), wxDEFAULT_DIALOG_STYLE)	{

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(new wxStaticText(this, -1, "SWISTrack"));
	vs->Add(new wxStaticText(this, -1, "A software for tracking mobile robots and other moving things."));
	vs->Add(new wxStaticText(this, -1, ""));
	vs->Add(new wxStaticText(this, -1, "(c) 2004-2007 Swarm-Intelligent Systems Group"));
	vs->Add(new wxStaticText(this, -1, "Swiss Federal Institute of Technology (EPFL)"));
	vs->Add(new wxStaticText(this, -1, "Lausanne, Switzerland"));
	vs->Add(new wxStaticText(this, -1, "http://swistrack.sourceforge.net"));

	vs->Add(new wxButton(this, wxID_OK, "Close"));
	SetSizer(vs);
}
