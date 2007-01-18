#include "CanvasPanel.h"
#define THISCLASS CanvasPanel

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
    EVT_LEFT_DOWN(THISCLASS::OnMouseLeftDown)
END_EVENT_TABLE()

THISCLASS::CanvasPanel(SwisTrack *st): wxPanel(st, -1, wxDefaultPosition, wxSize(100, 100)), mSwisTrack(st) {
	// Create the canvas
	mCanvas=new Canvas(this);
	
	// Create the combo box
	mTitle=new wxComboBox(this, eID_ComboBox);
	mTitle->SetStyle(wxCB_READONLY);
	mTitle->PushEventHandler(this);

	// Layout the components in the panel
	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(mCanvas, 0, wxALL|wxSHAPED|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 0);
	vs->Add(mComboBox, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT_HORIZONTAL, 0);
	SetSizer(vs);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(mTree, 1, wxEXPAND, 0);
	vs->Add(hs, 0, 0, 0);

	SetSizer(vs);
}

~THISCLASS::CanvasPanel() {
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
	wxMessageDialog dlg(this, "Test", "Test", wxOK);
	dlg.ShowModal();
}
