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
		wxControl(cp, -1), mCanvasPanel(cp), mDisplayRenderer(0), mPopupMenu() {

	SetWindowStyle(wxNO_BORDER);

	// Create the popup menu
	mPopupMenu.Append(sID_Zoom200, "Zoom: 200 %");
	mPopupMenu.Append(sID_Zoom100, "Zoom: 100 %");
	mPopupMenu.Append(sID_Zoom50, "Zoom: 50 %");
	mPopupMenu.Append(sID_Zoom25, "Zoom: 25 %");
	mPopupMenu.Append(sID_Zoom10, "Zoom: 10 %");
	mPopupMenu.Append(sID_ZoomFit, "Fit zoom");
	mPopupMenu.AppendSeparator();
	mPopupMenu.AppendCheckItem(sID_FlipVertically, "Flip vertically");
	mPopupMenu.AppendCheckItem(sID_FlipHorizontally, "Flip horizontally");
	mPopupMenu.AppendSeparator();
	mPopupMenu.Append(sID_SaveViewImageAs, "Save displayed image as ...");
	mPopupMenu.Append(sID_SaveOriginalImageAs, "Save original image as ...");

	// Set non-bold font
	wxFont f=GetFont();
	f.SetWeight(wxNORMAL);
	
	// Create a DisplayRenderer with an empty display
	mDisplayRenderer=new DisplayRenderer(0);
}

THISCLASS::~Canvas() {
}

void THISCLASS::SetDisplay(Display *display) {
	mDisplayRenderer.SetDisplay(display);
	UpdateView();
}

void THISCLASS::UpdateView() {
	mCanvas->OnSize();
}

wxSize THISCLASS::GetMaximumSize() {
	CvSize size=mDisplayRenderer->GetSize();
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
	IplImage *img=mDisplayRenderer->GetImage();
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
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
	mMoveStartPoint=event.GetPosition();
}

void THISCLASS::OnMouseRightDown(wxMouseEvent &event) {
	mPopupMenu->Check(sID_FlipVertically, mDisplayRenderer->GetFlipVertically());
	mPopupMenu->Check(sID_FlipHorizontally, mDisplayRenderer->GetFlipHorizontally());

	mPopupMenu->Check(sID_FlipHorizontally, mDisplayRenderer->Get());

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
	mDisplayRenderer.SetFlipVertical(event.GetChecked());
}

void THISCLASS::OnMenuFlipHorizontally(wxCommandEvent& event) {
	mDisplayRenderer.SetFlipHorizontal(event.GetChecked());
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
		mDisplayRenderer.SetMaxSize(mCanvasPanel->mAvailableSpace);
	}
	UpdateView();
}
