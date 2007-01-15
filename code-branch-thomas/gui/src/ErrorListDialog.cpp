#include "ErrorListDialog.h"
#define THISCLASS ErrorListDialog

#include <wx/sizer.h>
#include "ErrorList.h"

BEGIN_EVENT_TABLE(THISCLASS, wxDialog)
  EVT_BUTTON  (eID_ButtonOK, THISCLASS::OnButtonOK)
END_EVENT_TABLE()

THISCLASS::ErrorListDialog(wxWindow *parent, ErrorList *el, const wxString &title, const wxString &text):
		wxDialog(parent, -1, title, wxDefaultPosition, wxSize(400, 300), wxDEFAULT_DIALOG_STYLE), mErrorList(el) {

	// Create list control
	mList=new wxListCtrl(this, -1);
	mList->SetWindowStyle(wx);
	mList->InsertColumn(0, "Line", wxLIST_FORMAT_RIGHT, 100);
	mList->InsertColumn(1, "Error message", wxLIST_FORMAT_LEFT, 400);

	// Create other controls
	mLabel=new wxLabel(this, -1, text);
	mButtonOK=new wxButton(this, eID_ButtonOK, "OK");

	// Layout the components in the panel
	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->AddStretchSpacer(1);
	hs->Add(mButtonOK, 0, 0, 0);
	hs->AddStretchSpacer(1);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(mLabel, 1, wxEXPAND, 0);
	vs->Add(mList, 1, wxEXPAND, 0);
	vs->Add(hs, 0, 0, 0);

	SetSizer(vs);

	// Update the list contents
	OnUpdate();
}

void THISCLASS::OnUpdate() {
	mList->DeleteAllItems();

	int row=0;
	ErrorList::tList::iterator it=mErrorList->mList.begin();
	while (it!=mErrorList->mList.end()) {
		wxListItem li;

		// Line number
		li.SetId(row);
		li.SetColumn(0);
		if ((*it)->mLineNumber > 0) {
			li.SetText(wxString::Format("%d", (*it)->mLineNumber));
		} else {
			li.SetText("");
		}
		li.SetTextColour(*wxBLACK);
		li.SetData((void*)(*it));
		mList->InsertItem(li);

		// Message
		li.SetColumn(1);
		li.SetText((*it)->mMessage.c_str());
		mList->SetItem(li);

		// Next item
		it++;
		row++;
	}
}

void THISCLASS::OnButtonAdd(wxCommandEvent& event) {
	this->Hide();
}
