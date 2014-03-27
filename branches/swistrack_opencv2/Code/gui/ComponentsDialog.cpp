#include "ComponentsDialog.h"
#define THISCLASS ComponentsDialog

#include <wx/sizer.h>
#include "ComponentTreeItem.h"

BEGIN_EVENT_TABLE(THISCLASS, wxDialog)
	EVT_BUTTON (eID_ButtonAdd, THISCLASS::OnButtonAdd)
	EVT_BUTTON (eID_ButtonCancel, THISCLASS::OnButtonCancel)
END_EVENT_TABLE()

THISCLASS::ComponentsDialog(wxWindow* parent, SwisTrackCore *stc):
		wxDialog(parent, -1, wxT("Add Component"), wxDefaultPosition, wxSize(400, 400), wxDEFAULT_DIALOG_STYLE), mSwisTrackCore(stc), mSelectedComponent(0) {


	// Create tree control
	mTree = new wxTreeCtrl(this, wxID_ANY);
	mTree->SetWindowStyle(wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT);

	// Add the root element
	wxTreeItemId rootitem = mTree->AddRoot(wxT(""));
	wxTreeItemId rootitem2 = mTree->GetRootItem();

	// Add an item for each component
	SwisTrackCore::tComponentList::iterator it = mSwisTrackCore->mAvailableComponents.begin();
	wxTreeItemId curcategoryitem;
	std::map<ComponentCategory *, wxTreeItemId> mCategoryMap;
	ComponentCategory *curcategory = 0;
	while (it != mSwisTrackCore->mAvailableComponents.end()) {
		ComponentCategory *category = (*it)->mCategory;
		if (category) {
			if ((! curcategoryitem) || (category != curcategory)) {
			  curcategory = category;
			  if (mCategoryMap.find(curcategory) != mCategoryMap.end()) {
			    curcategoryitem = mCategoryMap[curcategory];
			  } else {
				curcategoryitem = mTree->AppendItem(rootitem, category->mDisplayName);
				mCategoryMap[category] = curcategoryitem;
			  }
			  mTree->Expand(curcategoryitem);
			}

			wxTreeItemId item = mTree->AppendItem(curcategoryitem, (*it)->mDisplayName, -1, -1, new ComponentTreeItem(*it));
		}
		it++;
	}

	// Create Buttons
	mButtonAdd = new wxButton(this, eID_ButtonAdd, wxT("Add"), wxDefaultPosition, wxDefaultSize);
	mButtonCancel = new wxButton(this, eID_ButtonCancel, wxT("Cancel"), wxDefaultPosition, wxDefaultSize);
	mButtonAdd->SetDefault();

	// Layout the components in the panel
	wxBoxSizer *hs = new wxBoxSizer(wxHORIZONTAL);
	hs->AddStretchSpacer(1);
	hs->Add(mButtonAdd, 0, wxALL, 4);
	hs->Add(mButtonCancel, 0, wxALL, 4);
	hs->AddStretchSpacer(1);

	wxBoxSizer *vs = new wxBoxSizer(wxVERTICAL);
	vs->Add(mTree, 1, wxEXPAND | wxALL, 8);
	vs->Add(hs, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

	SetSizer(vs);
}

void THISCLASS::OnButtonAdd(wxCommandEvent& event) {
	mSelectedComponent = 0;
	wxTreeItemId sel = mTree->GetSelection();
	if (sel.IsOk()) {
		ComponentTreeItem *cti = static_cast<ComponentTreeItem*>(mTree->GetItemData(sel));
		if (cti) {
			mSelectedComponent = cti->mComponent;
		}
	}

	if (! mSelectedComponent) {
		return;
	}
	this->Hide();
}

void THISCLASS::OnButtonCancel(wxCommandEvent& event) {
	mSelectedComponent = 0;
	this->Hide();
}
