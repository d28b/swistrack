#include "ComponentListPanel.h"
#define THISCLASS ComponentListPanel

#include <wx/sizer.h>

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
  EVT_BUTTON  (eID_ButtonAdd, THISCLASS::OnButtonAdd)
  EVT_BUTTON  (eID_ButtonRemove, THISCLASS::OnButtonRemove)
  EVT_BUTTON  (eID_ButtonUp, THISCLASS::OnButtonUp)
  EVT_BUTTON  (eID_ButtonDown, THISCLASS::OnButtonDown)
END_EVENT_TABLE()

THISCLASS::ComponentListPanel(wxWindow* parent, SwisTrackCore *stc):
		wxPanel(parent, -1), mSwisTrackCore(stc), mComponentsDialog(0) {

	// Create List
	mList=new wxListCtrl(this, -1);
	mList->SetWindowStyle(wxLC_REPORT|wxLC_HRULES);

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

	SwisTrackCore::tComponentList::iterator it=mSwisTrackCore->mComponentList.begin();
	while (it!=mSwisTrackCore->mComponentList.end()) {
		int col=0;
		wxListItem li;

		// Name
		li.SetColumn(col++);
		li.SetText((*it)->mDisplayName.c_str());
		li.SetTextColour(*wxBLACK);
		mList->InsertItem(li);

		// Status
		li.SetColumn(col++);
		if ((*it)->mStatusHasError) {
			li.SetText("E");
			li.SetTextColour(*wxRED);
			mList->InsertItem(li);
		} else if ((*it)->mStatusHasWarning) {
			li.SetText("W");
			li.SetTextColour(*wxBLUE);
			mList->InsertItem(li);
		}

		// Data structures
		SwisTrackCore::tDataStructures::iterator itds=mSwisTrackCore->mDataStructures.begin();
		while (itds!=mSwisTrackCore->mDataStructures.end()) {
			li.SetColumn(col++);
			
			bool read=(*it)->HasDataStructureRead(*itds);
			bool write=(*it)->HasDataStructureWrite(*itds);
			if (read && write) {
				li.SetText("E");
				li.SetTextColour(*wxBLUE);
				mList->InsertItem(li);
			} else if (read) {
				li.SetText("R");
				li.SetTextColour(*wxBLUE);
				mList->InsertItem(li);
			} else if (write) {
				li.SetText("W");
				li.SetTextColour(*wxBLUE);
				mList->InsertItem(li);
			}

			itds++;
		}

		// Status messages
		li.SetColumn(col++);
		if ((*it)->mStatusHasError) {
			li.SetTextColour(*wxRED);
		} else if ((*it)->mStatusHasWarning) {
			li.SetTextColour(*wxBLUE);
		}
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
			mList->InsertItem(li);
		}

		// Get next item
		it++;
	}
}

void THISCLASS::OnButtonAdd(wxCommandEvent& event) {
	if (mComponentsDialog==0) {
		mComponentsDialog=new ComponentsDialog(this->GetParent(), mSwisTrackCore);
	}

	mComponentsDialog->mSelectedComponent=0;
	mComponentsDialog->ShowModal();
	if (mComponentsDialog->mSelectedComponent) {
		mSwisTrackCore->mComponentList.push_back(mComponentsDialog->mSelectedComponent->Create());
		OnUpdate();
	}
}

void THISCLASS::OnButtonRemove(wxCommandEvent& event) {
	//mSwisTrackCore->mComponentList.erase();
}

void THISCLASS::OnButtonUp(wxCommandEvent& event) {
	
}

void THISCLASS::OnButtonDown(wxCommandEvent& event) {
	
}
