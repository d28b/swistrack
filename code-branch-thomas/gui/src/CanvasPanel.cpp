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

	// Create the canvas, title and annotation box
	mCanvas=new Canvas(this);
	mTitle=new CanvasTitle(this);
	mAnnotation=new CanvasAnnotation(this);

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

	// Calculate the available space
	wxSize size=GetClientSize();
	wxSize titlesize=mTitle->GetClientSize();
	wxSize annotationsize=mAnnotation->GetClientSize();

	int maximagewidth=size.GetWidth();
	int maximageheight=size.GetHeight()-titlesize.GetHeight()-annotationsize.GetHeight();
	if (maximageheight<10) {
		Hide();
		return;
	}

	// Get the new image
	IplImage *img=di->CreateImage(maximagewidth, maximageheight);
	mCanvas->SetImage(img);

	// Layout the children
	int x=(size.GetWidth()-img->width)/2;
	int y=(size.GetHeight()-img->height)/2;
	mCanvas->SetSize(x, y, img->width, img->height);
	mTitle->SetSize(x, y-titlesize.GetHeight(), img->width, titlesize.GetHeight());
	mAnnotation->SetSize(x, y+img->height, img->width, annotationsize.GetHeight());
	Show();
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
}

void THISCLASS::OnSize(wxSizeEvent &event) {
}
