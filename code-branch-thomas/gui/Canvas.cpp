#include "Canvas.h"
#define THISCLASS Canvas

#include <wx/image.h>
#include <highgui.h>
#include "ImageConversion.h"

BEGIN_EVENT_TABLE(THISCLASS, wxControl)
    EVT_PAINT(THISCLASS::OnPaint)
	EVT_ERASE_BACKGROUND(THISCLASS::OnEraseBackground)
	EVT_SIZE(THISCLASS::OnSize)
    EVT_LEFT_DOWN(THISCLASS::OnMouseLeftDown)
	EVT_LEFT_UP(THISCLASS::OnMouseLeftUp)
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
		wxControl(cp, -1), mCanvasPanel(cp), mDisplayRenderer(), mPopupMenu(),
		mMoveStarted(false), mMoveStartPoint() {

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
	UpdateView();
}

void THISCLASS::UpdateView() {
	mDisplayRenderer.DeleteCache();
	mCanvasPanel->UpdateSize();
	Refresh();
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
}

bool THISCLASS::OnPaintImage(wxPaintDC &dc) {
	IplImage *img=mDisplayRenderer.GetImage();
	if (! img) {return false;}

	// Create an image that has the size of the DC
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();
	wxImage wximg(dw, dh, false);

	// Prepare the area of interest of the source image
	int sx1=-mViewOffset.x;
	int sy1=-mViewOffset.y;
	if (sx1<0) {sx1=0;}
	if (sy1<0) {sy1=0;}
	if (sx1>img->width) {sx1=img->width;}
	if (sy1>img->height) {sy1=img->height;}
	int sx2=-mViewOffset.x+dw;
	int sy2=-mViewOffset.y+dh;
	if (sx2<0) {sx2=0;}
	if (sy2<0) {sy2=0;}
	if (sx2>img->width) {sx2=img->width;}
	if (sy2>img->height) {sy2=img->height;}
	int w=sx2-sx1;
	int h=sy2-sy1;

	// Prepare the area of interest of the destination image
	int dx1=mViewOffset.x;
	int dy1=mViewOffset.y;
	if (dx1<0) {dx1=0;}
	if (dy1<0) {dy1=0;}
	if (dx1>dw) {dx1=dw;}
	if (dy1>dh) {dy1=dh;}
	int dx2=mViewOffset.x+img->width;
	int dy2=mViewOffset.y+img->height;
	if (dx2<0) {dx2=0;}
	if (dy2<0) {dy2=0;}
	if (dx2>dw) {dx2=dw;}
	if (dy2>dh) {dy2=dh;}

	// Convert the area of interest to a wximg
	unsigned char *cw=wximg.GetData();
	unsigned char *cr=(unsigned char *)img->imageData;
	cr+=sx1*3+sy1*img->widthStep;

	// Top white area
	for (int y=0; y<dy1; y++) {
		memset(cw, 255, dw*3);
		cw+=dw*3;
	}

	// Image area
	for (int y=dy1; y<dy2; y++) {
		// White on the left
		memset(cw, 255, dx1*3);
		cw+=dx1*3;
		
		// Image
		memcpy(cw, cr, w*3);
		cw+=w*3;
		cr+=img->widthStep;

		// White on the right
		memset(cw, 255, (dw-dx2)*3);
		cw+=(dw-dx2)*3;
	}

	// Bottom white area
	for (int y=dy2; y<dh; y++) {
		memset(cw, 255, dw*3);
		cw+=dw*3;
	}

	// Draw image
	wxBitmap bmp(wximg);
	dc.DrawBitmap(bmp, 0, 0, false);
	return true;
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
	mMoveStartPoint=event.GetPosition();
	mMoveStarted=true;
}

void THISCLASS::OnMouseLeftUp(wxMouseEvent &event) {
	mMoveStarted=false;
}

void THISCLASS::OnMouseMove(wxMouseEvent &event) {
	if (! mMoveStarted) {return;}
	if (! event.LeftIsDown()) {mMoveStarted=false; return;}
	wxPoint nowpoint=event.GetPosition();

	wxPoint oldviewoffset=mViewOffset;
	mViewOffset.x+=nowpoint.x-mMoveStartPoint.x;
	mViewOffset.y+=nowpoint.y-mMoveStartPoint.y;
	mMoveStartPoint=nowpoint;

	if (mViewOffset.x>0) {mViewOffset.x=0;}
	if (mViewOffset.y>0) {mViewOffset.y=0;}

	wxSize dcsize=this->GetSize();
	CvSize imagesize=mDisplayRenderer.GetSize();
	if (mViewOffset.x<dcsize.GetWidth()-imagesize.width) {mViewOffset.x=dcsize.GetWidth()-imagesize.width;}
	if (mViewOffset.y<dcsize.GetHeight()-imagesize.height) {mViewOffset.y=dcsize.GetHeight()-imagesize.height;}

	if (mViewOffset==oldviewoffset) {return;}
	Refresh(true);
}

void THISCLASS::OnMouseRightDown(wxMouseEvent &event) {
	mPopupMenu.Check(sID_FlipVertically, mDisplayRenderer.GetFlipVertical());
	mPopupMenu.Check(sID_FlipHorizontally, mDisplayRenderer.GetFlipHorizontal());

	mPopupMenu.Check(sID_Zoom200, (mDisplayRenderer.GetScalingFactor()==2));
	mPopupMenu.Check(sID_Zoom100, (mDisplayRenderer.GetScalingFactor()==1));
	mPopupMenu.Check(sID_Zoom50, (mDisplayRenderer.GetScalingFactor()==0.5));

	PopupMenu(&mPopupMenu);
}

void THISCLASS::OnSize(wxSizeEvent &event) {
	Refresh(true);
}

void THISCLASS::OnMenuSaveOriginalImageAs(wxCommandEvent& event) {
	// Get the current image
	Display *display=mCanvasPanel->mCurrentDisplay;
	if (display==0) {return;}
	IplImage *img=display->mMainImage;
	if (img==0) {return;}

	// Convert and make sure we have a *copy* the current image (since the file dialog will allow other threads to run, which may modify the image)
	IplImage *imgcopy=ImageConversion::ToBGR(img);
	if (imgcopy==img) {
		imgcopy=cvCloneImage(img);
	}

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

	// Convert and make sure we have a *copy* the current image (since the file dialog will allow other threads to run, which may modify the image)
	IplImage *imgcopy=ImageConversion::ToBGR(img);
	if (imgcopy==img) {
		imgcopy=cvCloneImage(img);
	}

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