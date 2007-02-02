#include "CanvasTitle.h"
#define THISCLASS CanvasTitle

#include <sstream>
#include <wx/image.h>

BEGIN_EVENT_TABLE(THISCLASS, wxControl)
	EVT_ENTER_WINDOW(THISCLASS::OnMouseEnter)
	EVT_LEAVE_WINDOW(THISCLASS::OnMouseLeave)
    EVT_LEFT_DOWN(THISCLASS::OnMouseLeftDown)
	EVT_RIGHT_DOWN(THISCLASS::OnMouseRightDown)
	EVT_MENU(wxID_ANY, THISCLASS::OnMenu)
    EVT_PAINT(THISCLASS::OnPaint)
END_EVENT_TABLE()

THISCLASS::CanvasTitle(CanvasPanel *cp):
		wxControl(cp, -1), mCanvasPanel(cp), mTitle(""), mHighlight(false), mMenu(0) {

	SetWindowStyle(wxNO_BORDER);
	SetBackgroundColour(*wxBLACK);
	wxFont f=GetFont();
	f.SetWeight(wxFONTWEIGHT_BOLD);
	f.SetUnderlined(true);
	SetFont(f);
}

THISCLASS::~CanvasTitle() {
	if (mMenu) {delete mMenu;}
}

void THISCLASS::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	wxFont f=GetFont();
	dc.SetFont(f);
	if (mHighlight) {
		dc.SetTextForeground(*wxWHITE);
	} else {
		dc.SetTextForeground(wxColour(255, 255, 0));
	}
	dc.DrawText(mTitle, 0, 0);
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
	// Create a new menu
	if (mMenu) {delete mMenu;}
	mMenu = new wxMenu;

	// Add the no-display option
	mMenu->Append(0, "No display (maximum speed)");

	// Add all possible displays
	int id=1;
	SwisTrackCore *stc=mCanvasPanel->mSwisTrack->mSwisTrackCore;
	const SwisTrackCore::tComponentList *cl=stc->GetDeployedComponents();
	SwisTrackCore::tComponentList::const_iterator it=cl->begin();
	while (it!=cl->end()) {
		Component *c=(*it);

		Component::tDisplayImageList::iterator itdi=c->mDisplayImages.begin();
		while (itdi!=c->mDisplayImages.end()) {
			DisplayImage *di=(*itdi);

			std::ostringstream oss;
			oss << c->mDisplayName << ": " << di->mDisplayName;
			mMenu->Append(id++, oss.str().c_str());

			itdi++;
		}
		
		it++;
	}

	// Show the popup menu
	wxSize size=GetSize();
	PopupMenu(mMenu, 0, size.GetHeight());
}

void THISCLASS::OnMouseRightDown(wxMouseEvent &event) {
}

void THISCLASS::OnMouseEnter(wxMouseEvent &event) {
	mHighlight=true;
	Refresh(true);
}

void THISCLASS::OnMouseLeave(wxMouseEvent &event) {
	mHighlight=false;
	Refresh(true);
}

void THISCLASS::OnMenu(wxCommandEvent& event) {
	DisplayImage *di=GetDisplayImage(event.GetId());
	mCanvasPanel->SetDisplayImage(di);
}

DisplayImage *THISCLASS::GetDisplayImage(int selid) {
	if (selid<1) {return 0;}

	int id=1;
	SwisTrackCore *stc=mCanvasPanel->mSwisTrack->mSwisTrackCore;
	const SwisTrackCore::tComponentList *cl=stc->GetDeployedComponents();
	SwisTrackCore::tComponentList::const_iterator it=cl->begin();
	while (it!=cl->end()) {
		Component *c=(*it);

		Component::tDisplayImageList::iterator itdi=c->mDisplayImages.begin();
		while (itdi!=c->mDisplayImages.end()) {
			if (id==selid) {
				return (*itdi);
			}

			id++;
			itdi++;
		}

		it++;
	}

	return 0;
}
