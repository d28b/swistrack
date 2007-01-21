#include "ComponentListPanel.h"
#define THISCLASS ComponentListPanel

#include <wx/sizer.h>
#include <algorithm>

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
  EVT_BUTTON (eID_ButtonAdd, THISCLASS::OnButtonAdd)
  EVT_BUTTON (eID_ButtonRemove, THISCLASS::OnButtonRemove)
  EVT_BUTTON (eID_ButtonUp, THISCLASS::OnButtonUp)
  EVT_BUTTON (eID_ButtonDown, THISCLASS::OnButtonDown)
  EVT_LIST_ITEM_SELECTED (eID_List, THISCLASS::OnListItemSelected)
END_EVENT_TABLE()

THISCLASS::ComponentListPanel(wxWindow* parent, SwisTrackCore *stc):
		wxPanel(parent, -1),
		SwisTrackCoreInterface(),
		mSwisTrackCore(stc), mSelectedComponent(0), mComponentsDialog(0) {

	// Add SwisTrackCoreInterface
	mSwisTrackCore->AddInterface(this);

	// Create List
	mList=new wxListCtrl(this, eID_List);
	mList->SetWindowStyle(wxLC_REPORT|wxLC_HRULES|wxLC_SINGLE_SEL);

	// Add column for the component name
	int col=0;
	mList->InsertColumn(col++, "Component", wxLIST_FORMAT_LEFT, 200);

	// Add columns for data structures
	SwisTrackCore::tDataStructures::iterator it=mSwisTrackCore->mDataStructures.begin();
	while (it!=mSwisTrackCore->mDataStructures.end()) {
		mList->InsertColumn(col++, (*it)->mDisplayName.c_str(), wxLIST_FORMAT_CENTER, 50);
		it++;
	}

	// Add column for error messages
	mColumnMessages=col;
	mList->InsertColumn(col++, "Messages", wxLIST_FORMAT_LEFT, 200);

	// Create Buttons
	mButtonAdd=new wxButton(this, eID_ButtonAdd, "+", wxDefaultPosition, wxSize(25, 25));
	mButtonRemove=new wxButton(this, eID_ButtonRemove, "-", wxDefaultPosition, wxSize(25, 25));
	mButtonUp=new wxButton(this, eID_ButtonUp, "Up", wxDefaultPosition, wxSize(25, 25));
	mButtonDown=new wxButton(this, eID_ButtonDown, "Down", wxDefaultPosition, wxSize(25, 25));

	// Layout the components in the panel
	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(mButtonAdd, 0, 0, 0);
	vs->Add(mButtonRemove, 0, 0, 0);
	vs->AddStretchSpacer(1);
	vs->Add(mButtonUp, 0, 0, 0);
	vs->Add(mButtonDown, 0, 0, 0);

	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(mList, 1, wxEXPAND, 0);
	hs->Add(vs, 0, 0, 0);

	SetSizer(hs);

	// Update list
	OnUpdate();
}

void THISCLASS::Update() {
	// Empty the list
	mList->DeleteAllItems();

	// Add all components
	int row=0;
	SwisTrackCore::tComponentList::iterator it=mSwisTrackCore->mDeployedComponents.begin();
	while (it!=mSwisTrackCore->mDeployedComponents.end()) {
		int col=0;
		wxListItem li;

		// Name
		li.SetId(row);
		li.SetColumn(col++);
		li.SetText((*it)->mDisplayName.c_str());
		li.SetTextColour(*wxBLACK);
		li.SetData((void*)(*it));
		if (mSelectedComponent==(*it)) {
			li.SetState(wxLIST_STATE_SELECTED);
		}
		mList->InsertItem(li);

		// Data structures
		SwisTrackCore::tDataStructures::iterator itds=mSwisTrackCore->mDataStructures.begin();
		while (itds!=mSwisTrackCore->mDataStructures.end()) {
			li.SetColumn(col++);
			
			bool read=(*it)->HasDataStructureRead(*itds);
			bool write=(*it)->HasDataStructureWrite(*itds);
			if (read && write) {
				li.SetText("E");
				mList->SetItem(li);
			} else if (read) {
				li.SetText("R");
				mList->SetItem(li);
			} else if (write) {
				li.SetText("W");
				mList->SetItem(li);
			}

			itds++;
		}

		// Next item
		it++;
		row++;
	}

	// Set optimal column widths of all DataStructure columns
	int cc=mList->GetColumnCount();
	for (int i=1; i<cc-1; i++) {
		mList->SetColumnWidth(i, wxLIST_AUTOSIZE);
	}

	// Update the status messages
	UpdateStatus();
}

void THISCLASS::UpdateStatus() {
	long item=-1;
	while (1) {
		item=mList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_DONTCARE);
		if (item==-1) {break;}

		// Get the corresponding component
		Component *c=(Component*)mList->GetItemData(item);

		// Update the color
		if (c->mStatusHasError) {
			mList->SetItemBackgroundColour(wxColour(255, 225, 225));
		} else if ((*it)->mStatusHasWarning) {
			mList->SetItemBackgroundColour(wxColour(225, 225, 225));
		} else {
			mList->SetItemBackgroundColour(*wxWHITE);
		}

		// Status messages
		if (((*it)->mStatusHasError) || ((*it)->mStatusHasWarning)) {
			wxListItem li;
			li.SetId(item);
			li.SetColumn(mMessageColumn);

			wxString str;
			Component::tStatusItemList::iterator itsl=(*it)->mStatus.begin();
			while (itsl!=(*it)->mStatus.end()) {
				if (itsl->mType==StatusItem::sTypeError) {
					str+=itsl->mMessage.c_str();
					str+=" ";
				} else if (itsl->mType==StatusItem::sTypeWarning) {
					str+=itsl->mMessage.c_str();
					str+=" ";
				}

				itsl++;
			}
			li.SetText(str);

			mList->SetItem(li);
		}
	}
}

void THISCLASS::OnAfterStart(bool seriousmode) {
	UpdateStatus();

	if (seriousmode) {
		mButtonAdd->SetEnabled(false);
		mButtonRemove->SetEnabled(false);
		mButtonUp->SetEnabled(false);
		mButtonDown->SetEnabled(false);
	}
}

void THISCLASS::OnAfterStop() {
	UpdateStatus();
	mButtonAdd->SetEnabled(true);
	mButtonRemove->SetEnabled(true);
	mButtonUp->SetEnabled(true);
	mButtonDown->SetEnabled(true);
}

void THISCLASS::OnAfterStep() {
	UpdateStatus();
}

void THISCLASS::OnAfterEdit() {
	Update();
}

void THISCLASS::OnButtonAdd(wxCommandEvent& event) {
	// Make sure we are allowed to edit
	if (mSwisTrackCore->EditBegin()) {return;}

	// Create the dialog if necessary
	if (mComponentsDialog==0) {
		mComponentsDialog=new ComponentsDialog(this->GetParent(), mSwisTrackCore);
	}

	// Show dialog, returning if the user selected "Cancel"
	mComponentsDialog->mSelectedComponent=0;
	mComponentsDialog->ShowModal();
	if (! mComponentsDialog->mSelectedComponent) {return;}

	// Add the new component to the list
	// TODO: use the group information to add the component in the right place
	mSwisTrackCore->mDeployedComponents.push_back(mComponentsDialog->mSelectedComponent->Create());

	// Update the list
	mSwisTrackCore->EditEnd();
	OnUpdate();
}

void THISCLASS::OnButtonRemove(wxCommandEvent& event) {
	// Find the corresponding component in the list and delete it
	SwisTrackCore::tComponentList::iterator it=find(mSwisTrackCore->mDeployedComponents.begin(), mSwisTrackCore->mDeployedComponents.end(), mSelectedComponent);
	if (it==mSwisTrackCore->mDeployedComponents.end()) {return;}
	mSwisTrackCore->mDeployedComponents.erase(it);
	delete mSelectedComponent;

	// Update the list
	OnUpdate();
}

void THISCLASS::OnButtonUp(wxCommandEvent& event) {
	// Find the corresponding component in the list and move it up
	SwisTrackCore::tComponentList::iterator it=find(mSwisTrackCore->mDeployedComponents.begin(), mSwisTrackCore->mDeployedComponents.end(), mSelectedComponent);
	if (it==mSwisTrackCore->mDeployedComponents.end()) {return;}
	SwisTrackCore::tComponentList::iterator it2=it;
	it2--;
	if (it2==mSwisTrackCore->mDeployedComponents.end()) {return;}
	mSwisTrackCore->mDeployedComponents.erase(it);
	mSwisTrackCore->mDeployedComponents.insert(it2, mSelectedComponent);

	// Update the list
	OnUpdate();
}

void THISCLASS::OnButtonDown(wxCommandEvent& event) {
	// Find the corresponding component in the list and move it up
	SwisTrackCore::tComponentList::iterator it=find(mSwisTrackCore->mDeployedComponents.begin(), mSwisTrackCore->mDeployedComponents.end(), mSelectedComponent);
	if (it==mSwisTrackCore->mDeployedComponents.end()) {return;}
	SwisTrackCore::tComponentList::iterator it2=it;
	it2++;
	if (it2==mSwisTrackCore->mDeployedComponents.end()) {return;}
	it2++;
	mSwisTrackCore->mDeployedComponents.erase(it);
	mSwisTrackCore->mDeployedComponents.insert(it2, mSelectedComponent);

	// Update the list
	OnUpdate();
}

void THISCLASS::OnListItemSelected(wxListEvent& event) {
	mSelectedComponent=(Component*)(event.GetData());

	// TODO: show configuration panel of this component
}
