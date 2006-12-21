#include "ComponentListPanel.h"
#define THISCLASS ComponentListPanel

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
  EVT_BUTTON  (wxID_ButtonAdd, THISCLASS::OnButtonAdd)
  EVT_BUTTON  (wxID_ButtonRemove, THISCLASS::OnButtonRemove)
  EVT_BUTTON  (wxID_ButtonUp, THISCLASS::OnButtonUp)
  EVT_BUTTON  (wxID_ButtonDown, THISCLASS::OnButtonDown)
END_EVENT_TABLE()

THISCLASS::ComponentListCtrl(wxWindow* parent, SwisTrackCore *stc):
		wxPanel(parent, -1), mSwisTrackCore(stc) {

	// Create List
	mList=new wxListCtrl(this, -1);
	mList->SetWindowStyle(wxLC_REPORT);

	// Add column for component name
	int col=0;
	mList->InsertColumn(col++, "Component", wxLIST_FORMAT_LEFT, 200);
	mList->InsertColumn(col++, "Status", wxLIST_FORMAT_CENTER, 50);
	
	// Add columns for data structures
	SwisTrackCore::tDataStructures::iterator it=SwisTrackCore.mDataStructures.begin();
	while (it!=SwisTrackCore.mDataStructures.end()) {
		mList->InsertColumn(0, (*it)->mDisplayName, wxLIST_FORMAT_CENTER, 50);
		it++;
	}

	// Add column for error messages
	mList->InsertColumn(col++, "Messages", wxLIST_FORMAT_LEFT, 200);

	// Create Buttons
	mButtonAdd=new wxButton(this, wxID_ButtonAdd, "+", wxDefaultPosition, wxSize(25, 25));
	mButtonRemove=new wxButton(this, wxID_ButtonRemove, "-", wxDefaultPosition, wxSize(25, 25));
	mButtonUp=new wxButton(this, wxID_ButtonUp, "Up", wxDefaultPosition, wxSize(25, 25));
	mButtonDown=new wxButton(this, wxID_ButtonDown, "Down", wxDefaultPosition, wxSize(25, 25));

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
}

void THISCLASS::OnUpdate() {
	mList->ClearAll();

	SwisTrackCore::tComponentList::iterator it=SwisTrackCore.mComponentList.begin();
	while (it!=SwisTrackCore.mComponentList.end()) {
		int col=0;

		// Name
		wxListItem li;
		li.SetColumn(col++);
		li.SetText((*it)->mDisplayName);
		li.SetTextColour(*wxBLACK);
		mList->InsertItem(li);

		// Status
		wxListItem li;
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
		SwisTrackCore::tDataStructures::iterator itds=SwisTrackCore.mDataStructures.begin();
		while (itds!=SwisTrackCore.mDataStructures.end()) {
			wxListItem li;
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
		wxListItem li;
		li.SetColumn(col++);
		if ((*it)->mStatusHasError) {
			li.SetTextColour(*wxRED);
		} else if ((*it)->mStatusHasWarning) {
			li.SetTextColour(*wxBLUE);
		}
		if (((*it)->mStatusHasError) || ((*it)->mStatusHasWarning)) {
			wxString str;
			Component::tStatusItemList::iterator itsl=(*its)->mStatus.begin();
			while (itsl!=(*its)->mStatus.end()) {
				if ((*itsl)->mType==StatusItem::sTypeError) {
					str+=(*itsl)->mMessage+" ";
				} else if ((*itsl)->mType==StatusItem::sTypeWarning) {
					str+=(*itsl)->mMessage+" ";
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
	//mSwisTrackCore->mComponentList.push_back();
	
}

void THISCLASS::OnButtonRemove(wxCommandEvent& event) {
	//mSwisTrackCore->mComponentList.push_back();
}

void THISCLASS::OnButtonUp(wxCommandEvent& event) {
	
}

void THISCLASS::OnButtonDown(wxCommandEvent& event) {
	
}
