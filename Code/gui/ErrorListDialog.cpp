#include "ErrorListDialog.h"
#define THISCLASS ErrorListDialog

#include <wx/sizer.h>
#include "ErrorList.h"

BEGIN_EVENT_TABLE(THISCLASS, wxDialog)
	EVT_BUTTON  (eID_ButtonOK, THISCLASS::OnButtonOK)
END_EVENT_TABLE()

THISCLASS::ErrorListDialog(wxWindow *parent, ErrorList *el, const wxString &title, const wxString &text):
		wxDialog(parent, -1, title, wxDefaultPosition, wxSize(500, 300), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER), mErrorList(el) {

	// Create list control
	mList = new wxListCtrl(this, wxID_ANY);
	mList->SetWindowStyle(wxLC_REPORT | wxLC_HRULES | wxLC_SINGLE_SEL);
	mList->InsertColumn(0, wxT("Line"), wxLIST_FORMAT_RIGHT, 50);
	mList->InsertColumn(1, wxT("Error message"), wxLIST_FORMAT_LEFT, 400);

	// Create other controls
	mLabel = new wxStaticText(this, wxID_ANY, text);
	mButtonOK = new wxButton(this, eID_ButtonOK, wxT("OK"));

	// Layout the components in the panel
	wxBoxSizer *hs = new wxBoxSizer(wxHORIZONTAL);
	hs->AddStretchSpacer(1);
	hs->Add(mButtonOK, 0, wxALL, 4);
	hs->AddStretchSpacer(1);

	wxBoxSizer *vs = new wxBoxSizer(wxVERTICAL);
	vs->Add(mLabel, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 8);
	vs->Add(mList, 1, wxEXPAND | wxALL, 8);
	vs->Add(hs, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 8);

	SetSizer(vs);

	// Update the list contents
	OnUpdate();
}

void THISCLASS::OnUpdate() {
	mList->DeleteAllItems();

	int row = 0;
	ErrorList::tList::iterator it = mErrorList->mList.begin();
	while (it != mErrorList->mList.end()) {
		wxListItem li;

		// Line number
		li.SetId(row);
		li.SetColumn(0);
		if (it->mLineNumber > 0) {
			li.SetText(wxString::Format(wxT("%d"), it->mLineNumber));
		} else {
			li.SetText(wxT(""));
		}
		li.SetTextColour(*wxBLACK);
		mList->InsertItem(li);

		// Message
		li.SetColumn(1);
		li.SetText(it->mMessage);
		mList->SetItem(li);

		// Next item
		it++;
		row++;
	}
}

void THISCLASS::OnButtonOK(wxCommandEvent& event) {
	this->Hide();
}
