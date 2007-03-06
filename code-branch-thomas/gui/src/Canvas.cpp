#include "Canvas.h"
#define THISCLASS Canvas

#include <wx/image.h>
#include <highgui.h>

BEGIN_EVENT_TABLE(THISCLASS, wxControl)
    EVT_PAINT(THISCLASS::OnPaint)
	EVT_ERASE_BACKGROUND(THISCLASS::OnEraseBackground)
	EVT_SIZE(THISCLASS::OnSize)
    EVT_LEFT_DOWN(THISCLASS::OnMouseLeftDown)
	EVT_RIGHT_DOWN(THISCLASS::OnMouseRightDown)
	EVT_MOTION(THISCLASS::OnMouseMove)
	EVT_MENU(sID_SaveImageAs, THISCLASS::OnMenuSaveImageAs)
END_EVENT_TABLE()

THISCLASS::Canvas(CanvasPanel *cp):
		wxControl(cp, -1), mCanvasPanel(cp), mImage(0), mPopupMenu() {

	SetWindowStyle(wxNO_BORDER);

	mPopupMenu.Append(sID_SaveImageAs, "Save image as ...");

	wxFont f=GetFont();
	f.SetWeight(wxNORMAL);
}

THISCLASS::~Canvas() {
}

void THISCLASS::OnEraseBackground(wxEraseEvent& event) {
	// Overwrite this method to avoid drawing the background
}

void THISCLASS::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	// Check if we need to convert the image
	if (mImage) {
		if (mImage->nChannels==3) {
		} else if (mImage->nChannels==1) {
			IplImage *imgcolor=cvCreateImage(cvSize(mImage->width, mImage->height), mImage->depth, 3);
			cvCvtColor(mImage, imgcolor, CV_GRAY2BGR);
			cvReleaseImage(&mImage);
			mImage=imgcolor;
		} else {
			mDisplayError="Cannot display image: wrong format.";
			cvReleaseImage(&mImage);
			mImage=0;
		}
	}

	// Display an error if no image can be displayed
	if (mImage==0) {
		wxSize size=GetClientSize();
		dc.SetPen(*wxWHITE_PEN);
		dc.SetBrush(*wxWHITE_BRUSH);
		dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());
		dc.SetFont(GetFont());
		if (mDisplayError=="") {
			dc.DrawText("No image.", 4, 4);
		} else {
			dc.DrawText(mDisplayError, 4, 4);
		}
		return;
	}

	// Convert BGR to RGB
	wxImage img(mImage->width, mImage->height, false);
	unsigned char *crl=(unsigned char *)mImage->imageData;
	unsigned char *cw=img.GetData();
	for (int y=0; y<mImage->height; y++) {
		unsigned char *cr=crl;
		for (int x=0; x<mImage->width; x++) {
			int b=(int)*cr; cr++;
			int g=(int)*cr; cr++;
			int r=(int)*cr; cr++;

			*cw=r; cw++;
			*cw=g; cw++;
			*cw=b; cw++;
		}
		crl+=mImage->widthStep;
	}

	//wxImage img(mImage->width, mImage->height, (unsigned char*) mImage->imageData, true);
	wxBitmap bmp(img);
	dc.DrawBitmap(bmp, 0, 0, false);
}

void THISCLASS::SetImage(IplImage *img) {
	// Delete the old image and set the new image
	cvReleaseImage(&mImage);
	mImage=img;
	mDisplayError="";
	Refresh(false);
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
}

void THISCLASS::OnMouseRightDown(wxMouseEvent &event) {
	PopupMenu(&mPopupMenu);
}

void THISCLASS::OnMouseMove(wxMouseEvent &event) {
}

void THISCLASS::OnSize(wxSizeEvent &event) {
	Refresh(true);
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
