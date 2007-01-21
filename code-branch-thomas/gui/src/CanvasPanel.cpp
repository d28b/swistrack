#include "CanvasPanel.h"
#define THISCLASS CanvasPanel

#include "cv.h"
#include <sstream>

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_LEFT_DOWN(THISCLASS::OnMouseLeftDown)
END_EVENT_TABLE()

THISCLASS::CanvasPanel(SwisTrack *st):
		wxPanel(st, -1, wxDefaultPosition, wxSize(100, 100)),
		DisplayImageSubscriberInterface(),
		mSwisTrack(st), mMenu(0), mUpdateRate(1), mUpdateCounter(0) {

	// Create the canvas
	mCanvas=new Canvas(this);

	// Create the combo box
	mTitle=new wxStaticText(this, -1, "Test");
	//mTitle->PushEventHandler(this);

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
	int maximageheight=size.GetHeight()-titlesize.GetHeight()-annotationSize.GetHeight();
	if (maximageheight<10) {
		SetVisible(false);
		return;
	}

	// Get the new image
	IplImage *img=di->CreateImage(maximagewidth, maximageheight);
	mCanvas->SetImage(img);

	// Layout the children
	int x=(size.GetWidth()-img->width)/2;
	int y=(size.GetHeight()-img->height)/2;
	mCanvas->SetSize(img->width, img->height);
	mCanvas->SetPosition(x, y);
	mTitle->SetPosition(x, y-titlesize.GetHeight());
	mTitle->SetSize(img->width, titlesize.GetHeight());
	mAnnotation->SetPosition(x, y+img->height);
	mAnnotation->SetSize(img->width, annotationsize.GetHeight());
	SetVisible(true);
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
}
