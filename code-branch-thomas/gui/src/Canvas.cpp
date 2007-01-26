#include "Canvas.h"
#define THISCLASS Canvas

#include <wx/image.h>
#include <highgui.h>

BEGIN_EVENT_TABLE(THISCLASS, wxControl)
    EVT_PAINT(THISCLASS::OnPaint)
	EVT_ERASE_BACKGROUND(THISCLASS::OnEraseBackground)
    EVT_LEFT_DOWN(THISCLASS::OnMouseLeftDown)
	EVT_RIGHT_DOWN(THISCLASS::OnMouseRightDown)
	EVT_MOTION(THISCLASS::OnMouseMove)
	EVT_MENU(sID_SaveImageAs, THISCLASS::OnMenuSaveImageAs)
END_EVENT_TABLE()

THISCLASS::Canvas(CanvasPanel *cp):
		wxControl(cp, -1), mCanvasPanel(cp), mImage(0), mMenu(0) {

	mMenu = new wxMenu;
	mMenu->Append(sID_SaveImageAs, "Save image as ...");
}

THISCLASS::~Canvas() {
}

void THISCLASS::OnEraseBackground(wxEraseEvent& event) {
	// Overwrite this method to avoid drawing the background
}

void THISCLASS::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	if (mImage==0) {
		wxSize size=GetClientSize();
		dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());
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

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
}

void THISCLASS::OnMouseRightDown(wxMouseEvent &event) {
	PopupMenu(mMenu);
}

void THISCLASS::OnMouseMove(wxMouseEvent &event) {
}

void THISCLASS::OnMenuSaveImageAs(wxCommandEvent& event) {
	if (mImage==0) {return;}

	wxFileDialog *dlg = new wxFileDialog(this, "Save current image", "", "", "Bitmap (*.bmp)|*.bmp", wxSAVE, wxDefaultPosition);
	if (dlg->ShowModal() != wxID_OK)	{return;}

	wxString filename=dlg->GetPath();
	if(! cvSaveImage(filename.c_str(), mImage)) {
		wxMessageDialog dlg(this, "The file could not be saved!", "Save current image", wxOK);
		dlg.ShowModal();
		return;
	}
}
