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
	EVT_MENU(sID_Zoom200, THISCLASS::OnMenuZoom)
	EVT_MENU(sID_Zoom100, THISCLASS::OnMenuZoom)
	EVT_MENU(sID_Zoom50, THISCLASS::OnMenuZoom)
	EVT_MENU(sID_Zoom25, THISCLASS::OnMenuZoom)
	EVT_MENU(sID_Zoom10, THISCLASS::OnMenuZoom)
	EVT_MENU(sID_ZoomFit, THISCLASS::OnMenuZoom)
	EVT_MENU(sID_SaveViewImageAs, THISCLASS::OnMenuSaveViewImageAs)
	EVT_MENU(sID_SaveOriginalImageAs, THISCLASS::OnMenuSaveOriginalImageAs)
END_EVENT_TABLE()

THISCLASS::Canvas(CanvasPanel *cp):
		wxControl(cp, -1), mCanvasPanel(cp), mDisplayRenderer(), mPopupMenu() {

	SetWindowStyle(wxNO_BORDER);

	// Create the popup menu
	mPopupMenu.AppendCheckItem(sID_Zoom200, "Zoom: 200 %");
	mPopupMenu.AppendCheckItem(sID_Zoom100, "Zoom: 100 %");
	mPopupMenu.AppendCheckItem(sID_Zoom50, "Zoom: 50 %");
	mPopupMenu.AppendCheckItem(sID_Zoom25, "Zoom: 25 %");
	mPopupMenu.AppendCheckItem(sID_Zoom10, "Zoom: 10 %");
	mPopupMenu.AppendCheckItem(sID_ZoomFit, "Fit zoom");
	mPopupMenu.AppendSeparator();
	mPopupMenu.AppendCheckItem(sID_FlipVertically, "Flip vertically");
	mPopupMenu.AppendCheckItem(sID_FlipHorizontally, "Flip horizontally");
	mPopupMenu.AppendSeparator();
	mPopupMenu.Append(sID_SaveViewImageAs, "Save displayed image as ...");
	mPopupMenu.Append(sID_SaveOriginalImageAs, "Save original image as ...");

	// Set non-bold font
	wxFont f=GetFont();
	f.SetWeight(wxNORMAL);
}

THISCLASS::~Canvas() {
}

void THISCLASS::SetDisplay(Display *display) {
	mDisplayRenderer.SetDisplay(display);
	OnDisplayChanged();
}

void THISCLASS::OnDisplayChanged() {
	mDisplayRenderer.DeleteCache();
	Refresh();
}

void THISCLASS::UpdateView() {
	//wxSizeEvent e;
	//mCanvasPanel->OnSize(e);
}

wxSize THISCLASS::GetMaximumSize() {
	CvSize size=mDisplayRenderer.GetSize();
	if (size.width<32) {size.width=32;}
	if (size.height<32) {size.height=32;}
	return wxSize(size.width, size.height);
}

void THISCLASS::OnEraseBackground(wxEraseEvent& event) {
	// Overwrite this method to avoid drawing the background
}

void THISCLASS::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);
	bool ok=OnPaintImage(dc);

	// Display an error if no image can be displayed
	if (! ok) {
		wxSize size=GetClientSize();
		dc.SetPen(*wxWHITE_PEN);
		dc.SetBrush(*wxWHITE_BRUSH);
		dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());
		dc.SetFont(GetFont());
		dc.DrawText("No image.", 4, 4);
	}

	wxDateTime now = wxDateTime::Now();
	dc.DrawText(now.FormatTime(), 4, 4);
}

bool THISCLASS::OnPaintImage(wxPaintDC &dc) {
	IplImage *img=mDisplayRenderer.GetImage();
	if (! img) {return false;}

	// Create an image that has the size of the DC
	wxSize dcsize=dc.GetSize();
	wxImage wximg(dcsize.GetWidth(), dcsize.GetHeight(), false);

	// Prepare the area of interest
	int sx=mViewOffset.x;
	int sy=mViewOffset.y;
	if (sx<0) {sx=0;}
	if (sy<0) {sy=0;}
	int sw=img->width-sx;
	int sh=img->height-sy;
	if (sw>dcsize.GetWidth()) {sw=dcsize.GetWidth();}
	if (sh>dcsize.GetHeight()) {sh=dcsize.GetHeight();}

	// Convert the area of interest to BGR to RGB for display
	unsigned char *cw=wximg.GetData();
	unsigned char *crl=(unsigned char *)img->imageData;
	crl+=sx+sy*img->widthStep;
	for (int y=0; y<sh; y++) {
		unsigned char *cr=crl;
		for (int x=0; x<sw; x++) {
			int b=(int)*cr; cr++;
			int g=(int)*cr; cr++;
			int r=(int)*cr; cr++;

			*cw=r; cw++;
			*cw=g; cw++;
			*cw=b; cw++;
		}
		crl+=img->widthStep;
	}

	// Draw image
	wxBitmap bmp(wximg);
	dc.DrawBitmap(bmp, 0, 0, false);
	return true;
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
	mMoveStartPoint=event.GetPosition();
}

void THISCLASS::OnMouseRightDown(wxMouseEvent &event) {
	mPopupMenu.Check(sID_FlipVertically, mDisplayRenderer.GetFlipVertical());
	mPopupMenu.Check(sID_FlipHorizontally, mDisplayRenderer.GetFlipHorizontal());

	mPopupMenu.Check(sID_Zoom200, (mDisplayRenderer.GetScalingFactor()==2));
	mPopupMenu.Check(sID_Zoom100, (mDisplayRenderer.GetScalingFactor()==1));
	mPopupMenu.Check(sID_Zoom50, (mDisplayRenderer.GetScalingFactor()==0.5));

	PopupMenu(&mPopupMenu);
}

void THISCLASS::OnMouseMove(wxMouseEvent &event) {
	wxPoint nowpoint=event.GetPosition();

	//mViewOffset.x+=nowpoint.x-mMoveStartPoint.x;
	//mViewOffset.y+=nowpoint.y-mMoveStartPoint.y;
	mMoveStartPoint=nowpoint;
}

void THISCLASS::OnSize(wxSizeEvent &event) {
	Refresh(true);
}

void THISCLASS::OnMenuSaveOriginalImageAs(wxCommandEvent& event) {
	// Copy the current image (since the file dialog will allow other threads to run)
	Display *display=mCanvasPanel->mCurrentDisplay;
	if (display==0) {return;}
	IplImage *img=display->mMainImage;
	if (img==0) {return;}
	IplImage *imgcopy=cvCloneImage(img);

	// Show the file save dialog
	wxFileDialog *dlg = new wxFileDialog(this, "Save original image", "", "", "Bitmap (*.bmp)|*.bmp", wxSAVE, wxDefaultPosition);
	if (dlg->ShowModal() != wxID_OK) {return;}

	// Save the image
	wxString filename=dlg->GetPath();
	if(! cvSaveImage(filename.c_str(), imgcopy)) {
		wxMessageDialog dlg(this, "The file could not be saved!", "Save original image", wxOK);
		dlg.ShowModal();
		return;
	}
}

void THISCLASS::OnMenuSaveViewImageAs(wxCommandEvent& event) {
	// Copy the current image (since the file dialog will allow other threads to run)
	IplImage *img=mDisplayRenderer.GetImage();
	if (img==0) {return;}
	IplImage *imgcopy=cvCloneImage(img);

	// Show the file save dialog
	wxFileDialog *dlg = new wxFileDialog(this, "Save displayed image", "", "", "Bitmap (*.bmp)|*.bmp", wxSAVE, wxDefaultPosition);
	if (dlg->ShowModal() != wxID_OK) {return;}

	// Save the image
	wxString filename=dlg->GetPath();
	if(! cvSaveImage(filename.c_str(), imgcopy)) {
		wxMessageDialog dlg(this, "The file could not be saved!", "Save displayed image", wxOK);
		dlg.ShowModal();
		return;
	}
}

void THISCLASS::OnMenuFlipVertically(wxCommandEvent& event) {
	mDisplayRenderer.SetFlipVertical(event.IsChecked());
}

void THISCLASS::OnMenuFlipHorizontally(wxCommandEvent& event) {
	mDisplayRenderer.SetFlipHorizontal(event.IsChecked());
}

void THISCLASS::OnMenuZoom(wxCommandEvent& event) {
	if (event.GetId()==sID_Zoom200) {
		mDisplayRenderer.SetScalingFactor(2);
	} else if (event.GetId()==sID_Zoom100) {
		mDisplayRenderer.SetScalingFactor(1);
	} else if (event.GetId()==sID_Zoom50) {
		mDisplayRenderer.SetScalingFactor(0.5);
	} else if (event.GetId()==sID_Zoom25) {
		mDisplayRenderer.SetScalingFactor(0.25);
	} else if (event.GetId()==sID_Zoom10) {
		mDisplayRenderer.SetScalingFactor(0.1);
	} else {
		mDisplayRenderer.SetScalingFactorMax(cvSize(mCanvasPanel->mAvailableSpace.GetWidth(), mCanvasPanel->mAvailableSpace.GetHeight()));
	}
	UpdateView();
}
