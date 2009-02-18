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
	EVT_MENU(cID_PopupMenu_EnabledInterval0, THISCLASS::OnPopupMenuEnabledInterval)
	EVT_MENU(cID_PopupMenu_EnabledInterval1, THISCLASS::OnPopupMenuEnabledInterval)
	EVT_MENU(cID_PopupMenu_EnabledInterval2, THISCLASS::OnPopupMenuEnabledInterval)
	EVT_MENU(cID_PopupMenu_EnabledInterval3, THISCLASS::OnPopupMenuEnabledInterval)
	EVT_MENU(cID_PopupMenu_EnabledInterval4, THISCLASS::OnPopupMenuEnabledInterval)
	EVT_MENU(cID_PopupMenu_EnabledInterval5, THISCLASS::OnPopupMenuEnabledInterval)
	EVT_MENU(cID_PopupMenu_EnabledInterval10, THISCLASS::OnPopupMenuEnabledInterval)
	EVT_MENU(cID_PopupMenu_EnabledInterval20, THISCLASS::OnPopupMenuEnabledInterval)
	EVT_MENU(cID_PopupMenu_EnabledInterval50, THISCLASS::OnPopupMenuEnabledInterval)
	EVT_MENU(cID_PopupMenu_EnabledInterval100, THISCLASS::OnPopupMenuEnabledInterval)
END_EVENT_TABLE()

THISCLASS::ComponentListPanel(wxWindow* parent, SwisTrack *st):
		wxPanel(parent, -1), SwisTrackCoreInterface(),
		mSwisTrack(st), mSelectedComponent(0),
		mPopupMenu(), mList(0), mComponentsDialog(0) {

	// Add SwisTrackCoreInterface
	SwisTrackCore *stc = mSwisTrack->mSwisTrackCore;
	stc->AddInterface(this);

	// Create the popup menu
	mPopupMenu.AppendCheckItem(cID_PopupMenu_EnabledInterval1, wxT("Enabled for every frame"));
	mPopupMenu.AppendCheckItem(cID_PopupMenu_EnabledInterval2, wxT("Enabled for every 2nd frame"));
	mPopupMenu.AppendCheckItem(cID_PopupMenu_EnabledInterval3, wxT("Enabled for every 3rd frame"));
	mPopupMenu.AppendCheckItem(cID_PopupMenu_EnabledInterval4, wxT("Enabled for every 4th frame"));
	mPopupMenu.AppendCheckItem(cID_PopupMenu_EnabledInterval5, wxT("Enabled for every 5th frame"));
	mPopupMenu.AppendCheckItem(cID_PopupMenu_EnabledInterval10, wxT("Enabled for every 10th frame"));
	mPopupMenu.AppendCheckItem(cID_PopupMenu_EnabledInterval20, wxT("Enabled for every 20th frame"));
	mPopupMenu.AppendCheckItem(cID_PopupMenu_EnabledInterval50, wxT("Enabled for every 50th frame"));
	mPopupMenu.AppendCheckItem(cID_PopupMenu_EnabledInterval100, wxT("Enabled for every 100th frame"));
	mPopupMenu.AppendCheckItem(cID_PopupMenu_EnabledInterval0, wxT("Disabled"));

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

	// Add column for error messages, enabled interval and step duration
	mColumnEnabledInterval = col;
	mList->InsertColumn(col++, wxT("Enabled"), wxLIST_FORMAT_CENTER, 50);
	mColumnStepDuration = col;
	mList->InsertColumn(col++, wxT("Step duration"), wxLIST_FORMAT_CENTER, 100);
	mColumnMessages = col;
	mList->InsertColumn(col++, wxT("Messages"), wxLIST_FORMAT_LEFT, 400);

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
		li.SetFont(*wxNORMAL_FONT);
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
	if (selecteditemid > -1) {
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
		int enabledinterval = c->GetEnabledInterval();
		if (c->mStatusHasError) {
			mList->SetItemBackgroundColour(item, wxColour(255, 225, 225));
		} else if (c->mStatusHasWarning) {
			mList->SetItemBackgroundColour(item, wxColour(225, 225, 225));
		} else if (enabledinterval == 0) {
			mList->SetItemBackgroundColour(item, wxColour(192, 192, 192));
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

		// EnabledInterval
		wxListItem li;
		li.SetId(item);
		li.SetColumn(mColumnEnabledInterval);
		if (enabledinterval == 0) {
			li.SetText(wxT("no"));
		} else if (enabledinterval == 1) {
			li.SetText(wxT("yes"));
		} else {
			li.SetText(wxString::Format(wxT("yes (%% %d)"), enabledinterval));
		}
		mList->SetItem(li);

		// Step duration
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
	if (! mSelectedComponent) {
		return;
	}

	mPopupMenu.Enable(cID_PopupMenu_EnabledInterval0, ! mSwisTrack->mSwisTrackCore->IsStartedInProductionMode());
	mPopupMenu.Enable(cID_PopupMenu_EnabledInterval1, ! mSwisTrack->mSwisTrackCore->IsStartedInProductionMode());
	mPopupMenu.Enable(cID_PopupMenu_EnabledInterval2, ! mSwisTrack->mSwisTrackCore->IsStartedInProductionMode());
	mPopupMenu.Enable(cID_PopupMenu_EnabledInterval3, ! mSwisTrack->mSwisTrackCore->IsStartedInProductionMode());
	mPopupMenu.Enable(cID_PopupMenu_EnabledInterval4, ! mSwisTrack->mSwisTrackCore->IsStartedInProductionMode());
	mPopupMenu.Enable(cID_PopupMenu_EnabledInterval5, ! mSwisTrack->mSwisTrackCore->IsStartedInProductionMode());
	mPopupMenu.Enable(cID_PopupMenu_EnabledInterval10, ! mSwisTrack->mSwisTrackCore->IsStartedInProductionMode());
	mPopupMenu.Enable(cID_PopupMenu_EnabledInterval20, ! mSwisTrack->mSwisTrackCore->IsStartedInProductionMode());
	mPopupMenu.Enable(cID_PopupMenu_EnabledInterval50, ! mSwisTrack->mSwisTrackCore->IsStartedInProductionMode());
	mPopupMenu.Enable(cID_PopupMenu_EnabledInterval100, ! mSwisTrack->mSwisTrackCore->IsStartedInProductionMode());

	int enabledinterval = mSelectedComponent->GetEnabledInterval();
	mPopupMenu.Check(cID_PopupMenu_EnabledInterval0, (enabledinterval == 0));
	mPopupMenu.Check(cID_PopupMenu_EnabledInterval1, (enabledinterval == 1));
	mPopupMenu.Check(cID_PopupMenu_EnabledInterval2, (enabledinterval == 2));
	mPopupMenu.Check(cID_PopupMenu_EnabledInterval3, (enabledinterval == 3));
	mPopupMenu.Check(cID_PopupMenu_EnabledInterval4, (enabledinterval == 4));
	mPopupMenu.Check(cID_PopupMenu_EnabledInterval5, (enabledinterval == 5));
	mPopupMenu.Check(cID_PopupMenu_EnabledInterval10, (enabledinterval == 10));
	mPopupMenu.Check(cID_PopupMenu_EnabledInterval20, (enabledinterval == 20));
	mPopupMenu.Check(cID_PopupMenu_EnabledInterval50, (enabledinterval == 50));
	mPopupMenu.Check(cID_PopupMenu_EnabledInterval100, (enabledinterval == 100));

	PopupMenu(&mPopupMenu);
}

void THISCLASS::OnPopupMenuEnabledInterval(wxCommandEvent& event) {
	if (mSwisTrack->mSwisTrackCore->IsStartedInProductionMode()) {
		return;
	}

	// Set the enabled flag
	ComponentEditor ce(mSelectedComponent);

	if (event.GetId() == cID_PopupMenu_EnabledInterval2) {
		ce.SetEnabledInterval(2);
	} else if (event.GetId() == cID_PopupMenu_EnabledInterval3) {
		ce.SetEnabledInterval(3);
	} else if (event.GetId() == cID_PopupMenu_EnabledInterval4) {
		ce.SetEnabledInterval(4);
	} else if (event.GetId() == cID_PopupMenu_EnabledInterval5) {
		ce.SetEnabledInterval(5);
	} else if (event.GetId() == cID_PopupMenu_EnabledInterval10) {
		ce.SetEnabledInterval(10);
	} else if (event.GetId() == cID_PopupMenu_EnabledInterval20) {
		ce.SetEnabledInterval(20);
	} else if (event.GetId() == cID_PopupMenu_EnabledInterval50) {
		ce.SetEnabledInterval(50);
	} else if (event.GetId() == cID_PopupMenu_EnabledInterval100) {
		ce.SetEnabledInterval(100);
	} else if (event.GetId() == cID_PopupMenu_EnabledInterval0) {
		ce.SetEnabledInterval(0);
	} else {
		ce.SetEnabledInterval(1);
	}

	// Update ComponentList to reflect these changes
	UpdateStatus();
}
