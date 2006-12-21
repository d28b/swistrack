#include "ComponentListView.h"
#define THISCLASS ComponentListView

THISCLASS::ComponentListCtrl(wxWindow* parent, SwisTrackCore *stc):
		wxPanel(parent, -1), mSwisTrackCore(stc) {

	mList=new wxListCtrl(this, -1);
	mList->SetWindowStyle(wxLC_REPORT);

	// Column for data structures
	SwisTrackCore::tDataStructures::iterator it=SwisTrackCore.mDataStructures.end();
	while (it!=SwisTrackCore.mDataStructures.begin()) {
		it--;
		mList->InsertColumn(0, (*it)->mDisplayName, wxLIST_FORMAT_CENTER, 50);
	}

	// Column for component name
	mList->InsertColumn(0, "Component", wxLIST_FORMAT_LEFT, 200);
}

void THISCLASS::OnUpdate() {
	SwisTrackCore::tComponentList::iterator it=SwisTrackCore.mComponentList.end();
	while (it!=SwisTrackCore.mDataStructures.begin()) {
		wxListItem li;
		li.SetText("Camera");
		li.SetTextColour(*wxBLACK);
		li.SetColumn(0);
		mList->mListCtrlComponents->InsertItem(li);

		it++;
	}

}
