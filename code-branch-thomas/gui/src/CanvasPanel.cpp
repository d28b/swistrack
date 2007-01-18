#include "CanvasPanel.h"
#define THISCLASS CanvasPanel

#include "cv.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_LEFT_DOWN(THISCLASS::OnMouseLeftDown)
	EVT_MENU(wxID_ANY, THISCLASS::OnMenu)
END_EVENT_TABLE()

THISCLASS::CanvasPanel(SwisTrack *st):
		wxPanel(st, -1, wxDefaultPosition, wxSize(100, 100)),
		DisplayImageSubscriberInterface(),
		mSwisTrack(st), mMenu(0) {

	// Create the canvas
	mCanvas=new Canvas(this);

	// Create the combo box
	mTitle=new wxComboBox(this, eID_ComboBox);
	mTitle->SetStyle(wxCB_READONLY);
	mTitle->PushEventHandler(this);

	// Layout the components in the panel
	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(mTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT_HORIZONTAL, 0);
	vs->Add(mCanvas, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 0);
	SetSizer(vs);
}

~THISCLASS::CanvasPanel() {
}

void THISCLASS::OnDisplayImageChanged(DisplayImage *di) {
	wxSize size=GetClientSize();
	IplImage *img=di->CreateImage(size.width, size.height);
	mCanvas->SetImage(img);
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
	wxMessageDialog dlg(this, "Test", "Test", wxOK);
	dlg.ShowModal();

	// Create a new menu
	if (mMenu) {delete mMenu;}
	mMenu = new wxMenu;

	// Add all possible displays
	SwisTrackCore *stc=mSwisTrack->mSwisTrackCore;
	SwisTrackCore::tComponentList::iterator it=stc->mDeployedComponents.begin();
	while (it!=stc->mDeployedComponents.end()) {
		Component *c=(*it);

		Component::tDisplayImageList::iterator itdi=c->mDisplayImages.begin();
		while (itdi!=c->mDisplayImages.end()) {
			mDisplayImage *di=(*it);

			std::ostringstream oss;
			oss << c->mDisplayName << ": " << di->mDisplayName;
			mMenu->Append(id++, oss.str().c_str());

			itdi++;
		}
		
		it++;
	}

	// Show the popup menu
	PopupMenu(mMenu);
}

void THISCLASS::OnMenu(wxCommandEvent& event) {
	wxMessageDialog dlg(this, "Test2", "Test2", wxOK);
	dlg.ShowModal();
}
