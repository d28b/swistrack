#include "Canvas.h"
#define THISCLASS CanvasPanel

BEGIN_EVENT_TABLE(THISCLASS, wxControl)
    EVT_PAINT(THISCLASS::OnPaint)
	EVT_ERASE_BACKGROUND(THISCLASS::EraseBackground)
    EVT_LEFT_DOWN(THISCLASS::OnMouseLeftClick)
	EVT_RIGHT_DOWN(THISCLASS::OnMouseRightClick)
	EVT_MOTION(THISCLASS::OnMouseMove)
END_EVENT_TABLE()

THISCLASS::Canvas(CanvasPanel *cp):
		wxControl(cp, -1), mCanvasPanel(cp), mImage(0) {
 
}

void THISCLASS::EraseBackground(wxEraseEvent& event){
	// Overwrite this method to avoid drawing the background
}

void THISCLASS::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	if (mImage==0) {
		wxSize size=GetClientSize();
		dc.DrawRectangle(0, 0, size.width, size.height);
		return;
	}

	wxImage img(mImage->width, mImage->height, (unsigned char*) mImage->imageData, true);
	wxBitmap bmp(img, 3);
	dc.DrawBitmap(bmp, 0, 0, false);
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
