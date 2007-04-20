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
	EVT_MENU(sID_SaveViewImageAs, THISCLASS::OnMenuSaveViewImageAs)
	EVT_MENU(sID_SaveOriginalImageAs, THISCLASS::OnMenuSaveOriginalImageAs)
END_EVENT_TABLE()

THISCLASS::Canvas(CanvasPanel *cp):
		wxControl(cp, -1), mCanvasPanel(cp), mImage(0), mPopupMenu() {

	SetWindowStyle(wxNO_BORDER);

	mPopupMenu.Append(sID_Zoom200, "Zoom: 200 %");
	mPopupMenu.Append(sID_Zoom100, "Zoom: 100 %");
	mPopupMenu.Append(sID_Zoom50, "Zoom: 50 %");
	mPopupMenu.AppendSeparator();
	mPopupMenu.AppendCheckItem(sID_FlipVertically, "Flip vertically");
	mPopupMenu.AppendCheckItem(sID_FlipHorizontally, "Flip horizontally");
	mPopupMenu.AppendSeparator();
	mPopupMenu.Append(sID_SaveViewImageAs, "Save displayed image as ...");
	mPopupMenu.Append(sID_SaveOriginalImageAs, "Save original image as ...");

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
	Display *di=mCanvasPanel->mCurrentDisplay;
	if (! di) {return false;}
	IplImage *img=di->CreateImage(mViewScalingFactor);
	if (! img) {return false;}

	// Create an image that has the size of the DC
	wxSize dcsize=dc.GetSize();
	wxImage wximg(dcsize.GetWidth(), dcsize.GetHeight(), false);

	// TODO: flip vertically/horizontally if requested

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
}

void THISCLASS::UpdateView() {
	Display *di=mCanvasPanel->mCurrentDisplay;
	if (! di) {
		mViewSize=wxSize(320, 180);
	}

	// Determine the size of the image at the current scaling factor
	CvSize viewsize=di->GetSize(mScaleFactor);
	mViewSize.SetWidth(viewsize.width);
	mViewSize.SetHeight(viewsize.height);
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
	mMoveStartPoint=event.GetPosition();
}

void THISCLASS::OnMouseRightDown(wxMouseEvent &event) {
	mPopupMenu->Check(sID_FlipVertically, mViewFlipVertically);
	mPopupMenu->Check(sID_FlipHorizontally, mViewFlipHorizontally);

	PopupMenu(&mPopupMenu);
}

void THISCLASS::OnMouseMove(wxMouseEvent &event) {
	wxPoint nowpoint=event.GetPosition();

	mViewOffset.x+=nowpoint.x-mMoveStartPoint.x;
	mViewOffset.y+=nowpoint.y-mMoveStartPoint.y;
	mMoveStartPoint=nowpoint;
}

void THISCLASS::OnSize(wxSizeEvent &event) {
	Refresh(true);
}

void THISCLASS::OnMenuSaveOriginalImageAs(wxCommandEvent& event) {
	Display *di=mCanvasPanel->mCurrentDisplay;
	if (di==0) {return;}
	IplImage *img=di->mImage;
	if (img==0) {return;}

	wxFileDialog *dlg = new wxFileDialog(this, "Save original image", "", "", "Bitmap (*.bmp)|*.bmp", wxSAVE, wxDefaultPosition);
	if (dlg->ShowModal() != wxID_OK)	{return;}

	wxString filename=dlg->GetPath();
	if(! cvSaveImage(filename.c_str(), img)) {
		wxMessageDialog dlg(this, "The file could not be saved!", "Save original image", wxOK);
		dlg.ShowModal();
		return;
	}
}

void THISCLASS::OnMenuSaveViewImageAs(wxCommandEvent& event) {
	Display *di=mCanvasPanel->mCurrentDisplay;
	if (di==0) {return;}

	wxFileDialog *dlg = new wxFileDialog(this, "Save displayed image", "", "", "Bitmap (*.bmp)|*.bmp", wxSAVE, wxDefaultPosition);
	if (dlg->ShowModal() != wxID_OK)	{return;}

	IplImage *img=di->GetImage(mViewScalingFactor);
	if (img==0) {return;}

	wxString filename=dlg->GetPath();
	if(! cvSaveImage(filename.c_str(), img)) {
		wxMessageDialog dlg(this, "The file could not be saved!", "Save displayed image", wxOK);
		dlg.ShowModal();
		return;
	}
}

void THISCLASS::OnMenuFlipVertically(wxCommandEvent& event) {
	mFlipVertically=event.GetChecked();
}

void THISCLASS::OnMenuFlipHorizontally(wxCommandEvent& event) {
	mFlipHorizontally=event.GetChecked();
}
