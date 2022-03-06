#include "CanvasAnnotation.h"
#define THISCLASS CanvasAnnotation

#include <wx/image.h>

BEGIN_EVENT_TABLE(THISCLASS, wxControl)
	EVT_PAINT(THISCLASS::OnPaint)
	EVT_LEFT_DOWN(THISCLASS::OnMouseLeftClick)
	EVT_RIGHT_DOWN(THISCLASS::OnMouseRightClick)
	EVT_MOTION(THISCLASS::OnMouseMove)
END_EVENT_TABLE()

THISCLASS::CanvasAnnotation(CanvasPanel * cp):
	wxControl(cp, -1), mCanvasPanel(cp), mTextLeft(), mTextRight() {

	SetWindowStyle(wxNO_BORDER);
	SetBackgroundColour(*wxBLACK);
}

THISCLASS::~CanvasAnnotation() {
}

void THISCLASS::OnPaint(wxPaintEvent & WXUNUSED(event)) {
	wxPaintDC dc(this);

	// Prepare
	wxFont f = GetFont();
	dc.SetFont(f);
	dc.SetTextForeground(*wxWHITE);
	wxSize size = GetClientSize();

	// Left
	dc.DrawText(mTextLeft, 4, 2);

	// Right
	wxSize textExtent = dc.GetTextExtent(mTextRight);
	dc.DrawText(mTextRight, size.GetWidth() - textExtent.GetWidth() - 4, 2);
}

void THISCLASS::OnMouseLeftClick(wxMouseEvent & event) {
}

void THISCLASS::OnMouseRightClick(wxMouseEvent & event) {
}

void THISCLASS::OnMouseMove(wxMouseEvent & event) {
}

void THISCLASS::SetTextLeft(const wxString & textLeft) {
	mTextLeft = textLeft;
	Refresh(true);
}

void THISCLASS::SetTextRight(const wxString & textRight) {
	mTextRight = textRight;
	Refresh(true);
}
