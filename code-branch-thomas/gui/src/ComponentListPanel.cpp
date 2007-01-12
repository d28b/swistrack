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
		wxPanel(parent, -1), mSwisTrackCore(stc), mComponentsDialog(0) {

	// Create List
	mList=new wxListCtrl(this, eID_List);
	mList->SetWindowStyle(wxLC_REPORT|wxLC_HRULES|wxLC_SINGLE_SEL);

	// Add column for component name
	int col=0;
	mList->InsertColumn(col++, "Component", wxLIST_FORMAT_LEFT, 200);
	mList->InsertColumn(col++, "Status", wxLIST_FORMAT_CENTER, 50);
	
	// Add columns for data structures
	SwisTrackCore::tDataStructures::iterator it=mSwisTrackCore->mDataStructures.begin();
	while (it!=mSwisTrackCore->mDataStructures.end()) {
		mList->InsertColumn(col++, (*it)->mDisplayName.c_str(), wxLIST_FORMAT_CENTER, 50);
		it++;
	}

	// Add column for error messages
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

void THISCLASS::OnUpdate() {
	mList->DeleteAllItems();

	int row=0;
	SwisTrackCore::tComponentList::iterator it=mSwisTrackCore->mComponentList.begin();
	while (it!=mSwisTrackCore->mComponentList.end()) {
		int col=0;
		wxListItem li;

		// Name
		li.SetId(row);
		li.SetColumn(col++);
		li.SetText((*it)->mDisplayName.c_str());
		li.SetTextColour(*wxBLACK);
		li.SetData((void*)(*it));
		mList->InsertItem(li);

		// Status
		li.SetColumn(col++);
		if ((*it)->mStatusHasError) {
			li.SetText("E");
			li.SetBackgroundColour(wxColour(255, 225, 225));
			mList->SetItem(li);
		} else if ((*it)->mStatusHasWarning) {
			li.SetText("W");
			li.SetBackgroundColour(wxColour(225, 225, 225));
			mList->SetItem(li);
		}

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

		// Status messages
		li.SetColumn(col++);
		if (((*it)->mStatusHasError) || ((*it)->mStatusHasWarning)) {
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
			}
			li.SetText(str);
			mList->SetItem(li);
		}

		// Next item
		it++;
		row++;
	}
}

void THISCLASS::OnButtonAdd(wxCommandEvent& event) {
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
	mSwisTrackCore->mComponentList.push_back(mComponentsDialog->mSelectedComponent->Create());

	// Update the list
	OnUpdate();
}

void THISCLASS::OnButtonRemove(wxCommandEvent& event) {
	// Find out which item is selected
	long selitem = mList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selitem<0) {return;}

	// Get the corresponding component
	Component *sel=(Component*)(mList->GetItemData(selitem));
	if (! sel) {return;}

	// Find the corresponding component in the list and delete it
	SwisTrackCore::tComponentList::iterator it=find(mSwisTrackCore->mComponentList.begin(), mSwisTrackCore->mComponentList.end(), sel);
	if (it==mSwisTrackCore->mComponentList.end()) {return;}
	mSwisTrackCore->mComponentList.erase(it);
	delete sel;

	// Update the list
	OnUpdate();
}

void THISCLASS::OnButtonUp(wxCommandEvent& event) {
	// Find out which item is selected
	long selitem = mList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selitem<0) {return;}

	// Get the corresponding component
	Component *sel=(Component*)(mList->GetItemData(selitem));
	if (! sel) {return;}

	// Find the corresponding component in the list and move it up
	SwisTrackCore::tComponentList::iterator it=find(mSwisTrackCore->mComponentList.begin(), mSwisTrackCore->mComponentList.end(), sel);
	if (it==mSwisTrackCore->mComponentList.end()) {return;}
	SwisTrackCore::tComponentList::iterator it2=it-1;
	if (it2==mSwisTrackCore->mComponentList.end()) {return;}
	reverse(it2, it);

	// Update the list
	OnUpdate();
}

void THISCLASS::OnButtonDown(wxCommandEvent& event) {
	// Find out which item is selected
	long selitem = mList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selitem<0) {return;}

	// Get the corresponding component
	Component *sel=(Component*)(mList->GetItemData(selitem));
	if (! sel) {return;}

	// Find the corresponding component in the list and move it up
	SwisTrackCore::tComponentList::iterator it=find(mSwisTrackCore->mComponentList.begin(), mSwisTrackCore->mComponentList.end(), sel);
	if (it==mSwisTrackCore->mComponentList.end()) {return;}
	SwisTrackCore::tComponentList::iterator it2=it+1;
	if (it2==mSwisTrackCore->mComponentList.end()) {return;}
	reverse(it2, it);

	// Update the list
	OnUpdate();
}

void THISCLASS::OnListItemSelected(wxListEvent& event) {
	Component *sel=(Component*)(event.GetData());
	// TODO: show configuration panel of this component
}
