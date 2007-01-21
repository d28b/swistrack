#include "CanvasAnnotation.h"
#define THISCLASS CanvasAnnotation

#include <wx/image.h>

BEGIN_EVENT_TABLE(THISCLASS, wxControl)
    EVT_PAINT(THISCLASS::OnPaint)
    EVT_LEFT_DOWN(THISCLASS::OnMouseLeftClick)
	EVT_RIGHT_DOWN(THISCLASS::OnMouseRightClick)
	EVT_MOTION(THISCLASS::OnMouseMove)
END_EVENT_TABLE()

THISCLASS::CanvasAnnotation(CanvasAnnotationPanel *cp):
		wxControl(cp, -1), mCanvasAnnotationPanel(cp), mTextLeft(), mTextRight() {

	SetBackgroundColor(*wxBLACK);
}

THISCLASS::~CanvasAnnotation() {
}

void THISCLASS::EraseBackground(wxEraseEvent& event) {
	// Overwrite this method to avoid drawing the background
}

void THISCLASS::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	// Prepare
	wxFont f=GetFont();
	dc.SetFont(f);
	dc.SetTextForeground(*wxWHITE);
	wxSize size=GetClientSize();

	// Left
	dc.DrawText(mTextLeft, 0, 0);

	// Right
	wxSize textsize=dc.GetTextExtent(mTextRight);
	dc.DrawText(mTextRight, 0, size.GetWidth()-textsize.GetWidth());
}

void THISCLASS::SetImage(IplImage *img) {
	SetSize(img->width, img->height);
	cvReleaseImage(&mImage);
	mImage=img;
	Refresh(false);
}

void THISCLASS::OnMouseLeftClick(wxMouseEvent &event) {
}

void THISCLASS::OnMouseRightClick(wxMouseEvent &event) {
}

void THISCLASS::OnMouseMove(wxMouseEvent &event) {
}
