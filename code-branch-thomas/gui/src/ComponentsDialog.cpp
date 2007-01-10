#include "ComponentsDialog.h"
#define THISCLASS ComponentsDialog

#include <wx/sizer.h>
#include "ComponentTreeItem.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
  EVT_BUTTON  (eID_ButtonAdd, THISCLASS::OnButtonAdd)
  EVT_BUTTON  (eID_ButtonCancel, THISCLASS::OnButtonCancel)
END_EVENT_TABLE()

THISCLASS::ComponentsDialog(wxWindow* parent, SwisTrackCore *stc):
		wxDialog(parent, -1, "", wxDefaultPosition, wxSize(200, 400), wxDEFAULT_DIALOG_STYLE), mSwisTrackCore(stc), mSelectedComponent(0) {


	// Create tree control
	mTree=new wxTreeCtrl(this, -1);
	mTree->SetWindowStyle(wxTR_HAS_BUTTONS);

	// Add the root element
	wxTreeItemId rootitem=mTree->AddRoot("");

	// Add an item for each component
	SwisTrackCore::tComponents::iterator it=mSwisTrackCore->mComponents.begin();
	wxTreeItemId curcategoryitem=0;
	ComponentCategory *curcategory;
	while (it!=mSwisTrackCore->mComponents.end()) {
		ComponentCategory *category=&((*it)->mCategory);
		if ((! curcategoryitem) || (category!=curcategory)) {
			curcategoryitem=mTree->AppendItem(rootitem, category->mDisplayName.c_str());
			curcategory=category;
		}

		wxTreeItemId item=mTree->AppendItem(curcategoryitem, (*it)->mDisplayName.c_str(), -1, -1, new ComponentTreeItem(*it));
	}

	// Create Buttons
	mButtonAdd=new wxButton(this, eID_ButtonAdd, "+", wxDefaultPosition, wxDefaultSize);
	mButtonCancel=new wxButton(this, eID_ButtonCancel, "-", wxDefaultPosition, wxDefaultSize);

	// Layout the components in the panel
	wxBoxSizer *vs=new wxBoxSizer(wxHORIZONTAL);
	hs->AddStretchSpacer(1);
	hs->Add(mButtonAdd, 0, 0, 0);
	hs->Add(mButtonCancel, 0, 0, 0);
	hs->AddStretchSpacer(1);

	wxBoxSizer *hs=new wxBoxSizer(wxVERTICAL);
	vs->Add(mTree, 1, wxEXPAND, 0);
	vs->Add(hs, 0, 0, 0);

	SetSizer(vs);
}

void THISCLASS::OnButtonAdd(wxCommandEvent& event) {
	mSelectedComponent=0;
	wxTreeItemId sel=mTree->GetSelection();
	if (wxTreeItemId::IsOk(sel)) {
		mSelectedComponent=mTree->GetItemData(sel)->Component();
	}
	
	if (! mSelectedComponent) {return;}
	this->Hide();
}

void THISCLASS::OnButtonCancel(wxCommandEvent& event) {
	mSelectedComponent=0;
	this->Hide();
}
