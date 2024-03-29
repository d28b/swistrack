#include "CanvasTitle.h"
#define THISCLASS CanvasTitle

#include <wx/image.h>

BEGIN_EVENT_TABLE(THISCLASS, wxControl)
	EVT_ENTER_WINDOW(THISCLASS::OnMouseEnter)
	EVT_LEAVE_WINDOW(THISCLASS::OnMouseLeave)
	EVT_LEFT_DOWN(THISCLASS::OnMouseLeftDown)
	EVT_RIGHT_DOWN(THISCLASS::OnMouseRightDown)
	EVT_MENU(wxID_ANY, THISCLASS::OnMenu)
	EVT_PAINT(THISCLASS::OnPaint)
END_EVENT_TABLE()

THISCLASS::CanvasTitle(CanvasPanel * cp):
	wxControl(cp, -1), mCanvasPanel(cp), mTitle(wxT("")), mHighlight(false), mMenu(0) {

	SetWindowStyle(wxNO_BORDER);
	SetBackgroundColour(*wxBLACK);
}

THISCLASS::~CanvasTitle() {
	if (mMenu) delete mMenu;
}

void THISCLASS::OnPaint(wxPaintEvent & WXUNUSED(event)) {
	wxPaintDC dc(this);

	// Prepare
	wxFont f = GetFont();
	wxSize size = GetClientSize();

	// Left
	f.SetWeight(wxFONTWEIGHT_BOLD);
	f.SetUnderlined(true);
	dc.SetFont(f);
	dc.SetTextForeground(mHighlight ? *wxWHITE : wxColour(255, 255, 0));
	dc.DrawText(mTitle, 4, 2);

	// Right
	f.SetWeight(wxFONTWEIGHT_NORMAL);
	f.SetUnderlined(false);
	dc.SetFont(f);
	dc.SetTextForeground(*wxWHITE);
	wxSize textExtent = dc.GetTextExtent(mTextRight);
	dc.DrawText(mTextRight, size.GetWidth() - textExtent.GetWidth() - 4, 2);
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent & event) {
	// Create a new menu
	if (mMenu) delete mMenu;
	mMenu = new wxMenu;

	// Add the no-display option
	mMenu->Append(0, wxT("No display (maximum speed)"));

	// Add all possible displays
	int id = 1;
	SwisTrackCore * stc = mCanvasPanel->mSwisTrack->mSwisTrackCore;
	const SwisTrackCore::tComponentList * cl = stc->GetDeployedComponents();
	for (auto component : *cl) {
		for (auto display : component->mDisplays) {
			mMenu->Append(id, display->mDisplayName);
			id++;
		}
	}

	// Show the popup menu
	wxSize size = GetSize();
	PopupMenu(mMenu, 0, size.GetHeight());
}

void THISCLASS::OnMouseRightDown(wxMouseEvent & event) {
}

void THISCLASS::OnMouseEnter(wxMouseEvent & event) {
	mHighlight = true;
	Refresh(true);
}

void THISCLASS::OnMouseLeave(wxMouseEvent & event) {
	mHighlight = false;
	Refresh(true);
}

void THISCLASS::OnMenu(wxCommandEvent & event) {
	Display * display = GetDisplay(event.GetId());
	mCanvasPanel->SetDisplay(display);
}

void THISCLASS::SetText(const wxString & title, const wxString & textright) {
	mTitle = title;
	mTextRight = textright;
	Refresh(true);
}

Display * THISCLASS::GetDisplay(int selid) {
	if (selid < 1) return 0;

	int id = 1;
	SwisTrackCore * stc = mCanvasPanel->mSwisTrack->mSwisTrackCore;
	const SwisTrackCore::tComponentList * cl = stc->GetDeployedComponents();
	for (auto component : *cl) {
		for (auto display : component->mDisplays) {
			if (id == selid) return display;
			id++;
		}
	}

	return 0;
}
