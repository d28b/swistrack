#include "CanvasPanel.h"
#define THISCLASS CanvasPanel

#include "cv.h"
#include <sstream>

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_LEFT_DOWN(THISCLASS::OnMouseLeftDown)
	EVT_SIZE(THISCLASS::OnSize)
END_EVENT_TABLE()

THISCLASS::CanvasPanel(SwisTrack *st):
		wxPanel(st, -1, wxDefaultPosition, wxSize(100, 100)),
		DisplayImageSubscriberInterface(),
		mSwisTrack(st), mUpdateRate(1), mUpdateCounter(0) {

	SetBackgroundColour(st->GetBackgroundColour());

	// Create the canvas, title and annotation box
	mCanvas=new Canvas(this);
	mCanvasTitle=new CanvasTitle(this);
	mCanvasAnnotation=new CanvasAnnotation(this);

	// Layout the components in the panel
	//wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	//vs->Add(mTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	//vs->Add(mCanvas, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 0);
	//SetSizer(vs);
}

THISCLASS::~CanvasPanel() {
}

void THISCLASS::OnDisplayImageChanged(DisplayImage *di) {
	if (mUpdateRate==0) {return;}
	mUpdateCounter--;
	if (mUpdateCounter>0) {return;}
	mUpdateCounter=mUpdateRate;

	// Do nothing if the canvas is invisible
	if (! mCanvas->IsShown()) {return;}

	// Get the new image
	IplImage *img=di->CreateImage(mAvailableSpace.GetWidth(), mAvailableSpace.GetHeight());
	mCanvas->SetImage(img);

	// Set title and annotation
	mCanvasTitle->mTitle=di->mDisplayName.c_str();
	mCanvasAnnotation->mTextLeft=di->mAnnotation1.c_str();
	mCanvasAnnotation->mTextRight=di->mAnnotation2.c_str();

	// Move the children
	OnSize(wxSizeEvent());
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
}

void THISCLASS::OnSize(wxSizeEvent &event) {
	// Get the sizes
	wxSize size=GetClientSize();
	wxSize titlesize=mCanvasTitle->GetClientSize();
	titlesize.SetHeight(20);
	wxSize annotationsize=mCanvasAnnotation->GetClientSize();
	annotationsize.SetHeight(20);
	IplImage *img=mCanvas->GetImage();

	// Set the available space (for future images)
	mAvailableSpace.SetWidth(size.GetWidth());
	mAvailableSpace.SetHeight(size.GetHeight()-titlesize.GetHeight()-annotationsize.GetHeight());

	if ((mAvailableSpace.GetWidth()<30) || (mAvailableSpace.GetHeight()<30)) {
		mCanvasTitle->Hide();
		mCanvas->Hide();
		mCanvasAnnotation->Hide();
		return;
	}

	// Determine the size of the image
	int imagewidth=320;
	int imageheight=180;
	if (img) {
		imagewidth=img->width;
		imageheight=img->height;
	}
	if (imagewidth>mAvailableSpace.GetWidth()) {
		imagewidth=mAvailableSpace.GetWidth();
	}
	if (imageheight>mAvailableSpace.GetHeight()) {
		imageheight=mAvailableSpace.GetHeight();
	}

	// Layout the children
	int x=(size.GetWidth()-imagewidth)/2;
	int y=(size.GetHeight()-imageheight-titlesize.GetHeight()-annotationsize.GetHeight())/2;
	mCanvasTitle->SetSize(x, y, imagewidth, titlesize.GetHeight());
	y+=titlesize.GetHeight();
	mCanvas->SetSize(x, y, imagewidth, imageheight);
	y+=imageheight;
	mCanvasAnnotation->SetSize(x, y, imagewidth, annotationsize.GetHeight());

	mCanvasTitle->Show();
	mCanvas->Show();
	mCanvasAnnotation->Show();
}
