#include "ComponentListPanel.h"
#define THISCLASS ComponentListPanel

#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/minifram.h>
#include <algorithm>
#include "bitmaps/bitmap_plus.xpm"
#include "bitmaps/bitmap_minus.xpm"
#include "bitmaps/bitmap_up.xpm"
#include "bitmaps/bitmap_down.xpm"
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_BUTTON (cID_ButtonAdd, THISCLASS::OnButtonAddClick)
	EVT_BUTTON (cID_ButtonRemove, THISCLASS::OnButtonRemoveClick)
	EVT_BUTTON (cID_ButtonUp, THISCLASS::OnButtonUpClick)
	EVT_BUTTON (cID_ButtonDown, THISCLASS::OnButtonDownClick)
	EVT_LIST_ITEM_SELECTED (cID_List, THISCLASS::OnListItemSelected)
	EVT_LIST_ITEM_DESELECTED (cID_List, THISCLASS::OnListItemDeselected)
	EVT_MENU(cID_PopupMenu_Enabled, THISCLASS::OnPopupMenuEnabled)
END_EVENT_TABLE()

THISCLASS::ComponentListPanel(wxWindow* parent, SwisTrack *st):
		wxPanel(parent, -1), SwisTrackCoreInterface(),
		mSwisTrack(st), mSelectedComponent(0),
		mPopupMenu(), mList(0), mComponentsDialog(0) {

	// Add SwisTrackCoreInterface
	SwisTrackCore *stc = mSwisTrack->mSwisTrackCore;
	stc->AddInterface(this);

	// Create the popup menu
	mPopupMenu.AppendCheckItem(cID_PopupMenu_Enabled, wxT("Enabled"));

	// Create List
	mList = new wxListCtrl(this, cID_List);
	mList->SetWindowStyle(wxLC_REPORT | wxLC_HRULES | wxLC_SINGLE_SEL | wxBORDER_SIMPLE);
	mList->Connect(wxID_ANY, wxEVT_LEFT_DCLICK, wxMouseEventHandler(THISCLASS::OnListLeftDoubleClick), 0, this);
	mList->Connect(wxID_ANY, wxEVT_RIGHT_DOWN, wxMouseEventHandler(THISCLASS::OnListMouseRightDown), 0, this);

	// Add column for the component name
	int col = 0;
	mList->InsertColumn(col++, wxT("Component"), wxLIST_FORMAT_LEFT, 200);

	// Add column for the trigger flag
	mList->InsertColumn(col++, wxT("Trigger"), wxLIST_FORMAT_CENTER, 50);

	// Add columns for data structures
	SwisTrackCore::tDataStructureList::iterator it = stc->mDataStructures.begin();
	while (it != stc->mDataStructures.end()) {
		mList->InsertColumn(col++, (*it)->mDisplayName, wxLIST_FORMAT_CENTER, 50);
		it++;
	}

	// Add column for error messages
	mColumnMessages = col;
	mList->InsertColumn(col++, wxT("Messages"), wxLIST_FORMAT_LEFT, 400);
	mColumnStepDuration = col;
	mList->InsertColumn(col++, wxT("Step duration"), wxLIST_FORMAT_LEFT, 100);

	// Create Buttons
	//wxStaticLine *line1=new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(25, 2), wxLI_HORIZONTAL);
	//wxStaticLine *line2=new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(25, 2), wxLI_HORIZONTAL);
	mButtonAdd = new wxBitmapButton(this, cID_ButtonAdd, wxBITMAP(bitmap_plus), wxDefaultPosition, wxSize(25, 25));
	mButtonRemove = new wxBitmapButton(this, cID_ButtonRemove, wxBITMAP(bitmap_minus), wxDefaultPosition, wxSize(25, 25));
	mButtonUp = new wxBitmapButton(this, cID_ButtonUp, wxBITMAP(bitmap_up), wxDefaultPosition, wxSize(25, 25));
	mButtonDown = new wxBitmapButton(this, cID_ButtonDown, wxBITMAP(bitmap_down), wxDefaultPosition, wxSize(25, 25));

	// Create top and bottom lines
	wxStaticLine *topline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(25, 1), wxLI_HORIZONTAL);
	wxStaticLine *bottomline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(25, 1), wxLI_HORIZONTAL);

	// Layout the components in the panel
	wxBoxSizer *vs = new wxBoxSizer(wxVERTICAL);
	//vs->Add(line1, 0, 0, 0);
	vs->Add(mButtonAdd, 0, 0, 0);
	vs->Add(mButtonRemove, 0, 0, 0);
	vs->AddStretchSpacer(1);
	vs->Add(mButtonUp, 0, 0, 0);
	vs->Add(mButtonDown, 0, 0, 0);
	//vs->Add(line2, 0, 0, 0);

	wxBoxSizer *hs = new wxBoxSizer(wxHORIZONTAL);
	hs->Add(mList, 1, wxEXPAND, 0);
	hs->Add(vs, 0, wxEXPAND, 0);

	wxBoxSizer *vstop = new wxBoxSizer(wxVERTICAL);
	vstop->Add(topline, 0, wxEXPAND, 0);
	vstop->Add(hs, 1, wxEXPAND, 0);
	vstop->Add(bottomline, 0, wxEXPAND, 0);

	SetSizer(vstop);

	// Update list
	Update();
}

THISCLASS::~ComponentListPanel() {
	// Remove from the SwisTrackCore subscriber list
	mSwisTrack->mSwisTrackCore->RemoveInterface(this);

	// Delete the dialog
	if (mComponentsDialog) {
		mComponentsDialog->Destroy();
	}
}

void THISCLASS::Update() {
	// Empty the list
	mList->DeleteAllItems();

	// Needed to keep selection visible
	int selecteditemid = -1;

	// Add all components
	int row = 0;
	SwisTrackCore *stc = mSwisTrack->mSwisTrackCore;
	const SwisTrackCore::tComponentList *cl = stc->GetDeployedComponents();
	SwisTrackCore::tComponentList::const_iterator it = cl->begin();
	while (it != cl->end()) {
		int col = 0;
		wxListItem li;

		// Name
		li.SetId(row);
		li.SetColumn(col++);
		li.SetText((*it)->mDisplayName);
		li.SetTextColour(*wxBLACK);
		li.SetFont((*it)->GetEnabled() ? *wxNORMAL_FONT : *wxITALIC_FONT);
		li.SetData((void*)(*it));
		if (mSelectedComponent == (*it)) {
			li.SetState(wxLIST_STATE_SELECTED);
			selecteditemid = li.GetId();
		}
		mList->InsertItem(li);

		// Trigger flag
		li.SetColumn(col++);
		if ((*it)->mTrigger) {
			li.SetText(wxT("T"));
			mList->SetItem(li);
		}

		// Data structures
		SwisTrackCore::tDataStructureList::iterator itds = stc->mDataStructures.begin();
		while (itds != stc->mDataStructures.end()) {
			li.SetColumn(col++);

			bool read = (*it)->HasDataStructureRead(*itds);
			bool write = (*it)->HasDataStructureWrite(*itds);
			if (read && write) {
				li.SetText(wxT("E"));
				mList->SetItem(li);
			} else if (read) {
				li.SetText(wxT("R"));
				mList->SetItem(li);
			} else if (write) {
				li.SetText(wxT("W"));
				mList->SetItem(li);
			}

			itds++;
		}

		// Next item
		it++;
		row++;
	}

	// Set optimal column widths of all DataStructure columns
	for (int i = 1; i < mColumnMessages - 1; i++) {
		mList->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
	}

	// Make the selection visible
	if (selecteditemid>-1) {
		mList->EnsureVisible(selecteditemid);
	}

	// Update the status messages
	UpdateStatus();
}

void THISCLASS::UpdateStatus() {
	long item = -1;
	while (1) {
		item = mList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_DONTCARE);
		if (item == -1) {
			break;
		}

		// Get the corresponding component
		Component *c = (Component*)mList->GetItemData(item);

		// Update the color
		if (c->mStatusHasError) {
			mList->SetItemBackgroundColour(item, wxColour(255, 225, 225));
		} else if (c->mStatusHasWarning) {
			mList->SetItemBackgroundColour(item, wxColour(225, 225, 225));
		} else {
			mList->SetItemBackgroundColour(item, *wxWHITE);
		}

		// Status messages
		if ((c->mStatusHasError) || (c->mStatusHasWarning)) {
			wxListItem li;
			li.SetId(item);
			li.SetColumn(mColumnMessages);

			wxString str;
			Component::tStatusItemList::iterator itsl = c->mStatus.begin();
			while (itsl != c->mStatus.end()) {
				if (itsl->mType == StatusItem::sTypeError) {
					str += itsl->mMessage;
					str += wxT(" ");
				} else if (itsl->mType == StatusItem::sTypeWarning) {
					str += itsl->mMessage;
					str += wxT(" ");
				}

				itsl++;
			}
			li.SetText(str);

			mList->SetItem(li);
		} else {
			wxListItem li;
			li.SetId(item);
			li.SetColumn(mColumnMessages);
			li.SetText(wxT(""));
			mList->SetItem(li);
		}

		// Step duration
		wxListItem li;
		li.SetId(item);
		li.SetColumn(mColumnStepDuration);
		if (c->mStepDuration < 0) {
			li.SetText(wxT(""));
		} else {
			li.SetText(wxString::Format(wxT("%0.3f ms"), c->mStepDuration*1000.));
		}
		mList->SetItem(li);
	}
}

void THISCLASS::OnAfterStart(bool productionmode) {
	UpdateStatus();

	if (productionmode) {
		mButtonAdd->Enable(false);
		mButtonRemove->Enable(false);
		mButtonUp->Enable(false);
		mButtonDown->Enable(false);
	}
}

void THISCLASS::OnAfterStop() {
	UpdateStatus();
	mButtonAdd->Enable(true);
	mButtonRemove->Enable(true);
	mButtonUp->Enable(true);
	mButtonDown->Enable(true);
}

void THISCLASS::OnAfterStep() {
	UpdateStatus();
}

void THISCLASS::OnAfterEdit() {
	Update();
	mSwisTrack->Control_ReloadConfiguration();
}

void THISCLASS::OnButtonAddClick(wxCommandEvent& event) {
	SwisTrackCoreEditor stce(mSwisTrack->mSwisTrackCore);
	if (! stce.IsEditable()) {
		return;
	}

	// Create the dialog if necessary
	if (mComponentsDialog == 0) {
		mComponentsDialog = new ComponentsDialog(this->GetParent(), mSwisTrack->mSwisTrackCore);
	}

	// Show dialog, returning if the user selected "Cancel"
	mComponentsDialog->mSelectedComponent = 0;
	mComponentsDialog->ShowModal();
	if (! mComponentsDialog->mSelectedComponent) {
		return;
	}

	// Create the new component
	Component *newcomponent = mComponentsDialog->mSelectedComponent->Create();

	// Try to insert the new component in a suitable spot in the list
	SwisTrackCore::tComponentList *cl = stce.GetDeployedComponents();
	if (newcomponent->mCategory && newcomponent->mCategory->mOrder) {
		SwisTrackCore::tComponentList::iterator it;
		for (it = cl->begin(); it != cl->end(); it++) {
			if ((*it)->mCategory && ((*it)->mCategory->mOrder > newcomponent->mCategory->mOrder)) {
				cl->insert(it, newcomponent);
				return;
			}
		}
	}

	// If no optimal position could be determined, add the component to the end
	cl->push_back(newcomponent);
}

void THISCLASS::OnButtonRemoveClick(wxCommandEvent& event) {
	SwisTrackCoreEditor stce(mSwisTrack->mSwisTrackCore);
	if (! stce.IsEditable()) {
		return;
	}

	// Find the corresponding component in the list and delete it
	SwisTrackCore::tComponentList *cl = stce.GetDeployedComponents();
	SwisTrackCore::tComponentList::iterator it = find(cl->begin(), cl->end(), mSelectedComponent);
	if (it == cl->end()) {
		return;
	}
	cl->erase(it);
	delete mSelectedComponent;
}

void THISCLASS::OnButtonUpClick(wxCommandEvent& event) {
	SwisTrackCoreEditor stce(mSwisTrack->mSwisTrackCore);
	if (! stce.IsEditable()) {
		return;
	}

	// Find the corresponding component in the list and move it up
	SwisTrackCore::tComponentList *cl = stce.GetDeployedComponents();
	SwisTrackCore::tComponentList::iterator it = find(cl->begin(), cl->end(), mSelectedComponent);
	if (it == cl->end()) {
		return;
	}
	if (it == cl->begin()) {
		return;
	}
	SwisTrackCore::tComponentList::iterator it2 = it;
	it2--;
	cl->erase(it);
	cl->insert(it2, mSelectedComponent);
}

void THISCLASS::OnButtonDownClick(wxCommandEvent& event) {
	SwisTrackCoreEditor stce(mSwisTrack->mSwisTrackCore);
	if (! stce.IsEditable()) {
		return;
	}

	// Find the corresponding component in the list and move it up
	SwisTrackCore::tComponentList *cl = stce.GetDeployedComponents();
	SwisTrackCore::tComponentList::iterator it = find(cl->begin(), cl->end(), mSelectedComponent);
	if (it == cl->end()) {
		return;
	}
	SwisTrackCore::tComponentList::iterator it2 = it;
	it2++;
	if (it2 == cl->end()) {
		return;
	}
	it2++;
	cl->erase(it);
	cl->insert(it2, mSelectedComponent);
}

void THISCLASS::OnListItemSelected(wxListEvent& event) {
	mSelectedComponent = (Component*)(event.GetData());

	mSwisTrack->SetConfigurationPanel(mSelectedComponent);
	mSwisTrack->mTimelinePanel->SelectComponent(mSelectedComponent);
}

void THISCLASS::OnListItemDeselected(wxListEvent& event) {
	mSwisTrack->SetConfigurationPanel(0);
}

void THISCLASS::OnListLeftDoubleClick(wxMouseEvent& event) {
	wxMiniFrame *frame = new wxMiniFrame(this, -1, _("Configuration"), wxDefaultPosition, wxSize(230, 400), wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX | wxSYSTEM_MENU);
	new ConfigurationPanel(frame, mSwisTrack, mSelectedComponent);
	frame->Show();
}

void THISCLASS::OnListMouseRightDown(wxMouseEvent& event) {
  if (mSelectedComponent) {
    mPopupMenu.Check(cID_PopupMenu_Enabled, mSelectedComponent->GetEnabled());
    mPopupMenu.Enable(cID_PopupMenu_Enabled, ! mSwisTrack->mSwisTrackCore->IsStartedInProductionMode());
    PopupMenu(&mPopupMenu);
  }
}

void THISCLASS::OnPopupMenuEnabled(wxCommandEvent& event) {
	if (mSwisTrack->mSwisTrackCore->IsStartedInProductionMode()) {
		return;
	}

	// Set the enabled flag
	ComponentEditor ce(mSelectedComponent);
	ce.SetEnabled(event.IsChecked());
	Update(); // update ComponentList, in case enabled-ness changed
	// Restart
	mSwisTrack->mSwisTrackCore->Stop();
	mSwisTrack->mSwisTrackCore->Start(false);
	mSwisTrack->Control_Step();
}
