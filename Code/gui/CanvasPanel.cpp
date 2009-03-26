#include "CanvasPanel.h"
#define THISCLASS CanvasPanel

#include "cv.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_SIZE(THISCLASS::OnSize)
END_EVENT_TABLE()

THISCLASS::CanvasPanel(wxWindow *parent, SwisTrack *st):
		wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(100, 100)),
		DisplaySubscriberInterface(), mSwisTrack(st),
		mCurrentDisplay(0), mUpdateRate(1), mUpdateStepCounter(0) {

	SetBackgroundColour(st->GetBackgroundColour());

	// Create the canvas, title and annotation box
	mCanvas = new Canvas(this);
	mCanvasTitle = new CanvasTitle(this);
	mCanvasAnnotation = new CanvasAnnotation(this);

	// Layout the components in the panel
	//wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	//vs->Add(mTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	//vs->Add(mCanvas, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 0);
	//SetSizer(vs);

	// Set the title
	mCanvasTitle->SetText(wxT("No display (maximum speed)"), wxT(""));
}

THISCLASS::~CanvasPanel() {
	SetDisplay(0);
}

void THISCLASS::SetDisplay(Display *display) {
	if (mCurrentDisplay == display) {
		return;
	}

	if (display) {
		// Subscribe to the new display
		display->Subscribe(this);
	} else if (mCurrentDisplay) {
		// Unsubscribe from the current display
		mCurrentDisplay->Unsubscribe(this);
	}
}

void THISCLASS::OnDisplaySubscribe(Display *display) {
	Display *olddisplay = mCurrentDisplay;
	mCurrentDisplay = display;

	// Unsubscribe from the current display
	if (olddisplay) {
		olddisplay->Unsubscribe(this);
	}

	// Update the GUI
	mCanvas->SetDisplay(display);
	OnDisplayChanged(display);
}

void THISCLASS::OnDisplayUnsubscribe(Display *display) {
	if (mCurrentDisplay != display) {
		return;
	}

	// Set everything to empty
	mCurrentDisplay = 0;
	mCanvas->SetDisplay(0);
	mCanvasTitle->SetText(wxT("No display (maximum speed)"), wxT(""));
	mCanvasAnnotation->SetText(wxT(""), wxT(""));
}

void THISCLASS::OnDisplayBeforeStep(Display *display) {
	if (mCurrentDisplay != display) {
		return;
	}

	// If the display is disabled, return
	if (mUpdateRate <= 0) {
		return;
	}

	// Count to mUpdateRate from the last displayed image (the counter is reset in OnDisplayChanged)
	//mUpdateStepCounter++;
	SwisTrackCore *stc = mSwisTrack->mSwisTrackCore;
	if ((stc->GetStepCounter() % mUpdateRate) != 0) {
		return;
	}

	// Do nothing if the canvas is invisible
	if (! mCanvas->IsShown()) {
		return;
	}

	// Activate the display (such that OnDisplayChanged is called)
	display->SetActive();
}

void THISCLASS::OnDisplayChanged(Display *display) {
	if (mCurrentDisplay != display) {
		return;
	}

	// Reset the counter
	//mUpdateStepCounter = 0;

	// Do nothing if the canvas is invisible
	if (! mCanvas->IsShown()) {
		return;
	}

	// Update the canvas
	mCanvas->OnDisplayChanged();

	// Update title and annotation
	mCanvasTitle->SetText(display->mDisplayName, wxT(""));
	wxString str = wxString::Format(wxT("Step %d"), display->mStepCounter);
	if (display->mFramesCount >= 0) {
		str += wxString::Format(wxT(" (Frame %d / %d)"), display->mFrameNumber, display->mFramesCount);
	}
	str += wxString::Format(wxT(", %dx%d, %s"), display->mSize.width, display->mSize.height, display->mTime.FormatTime().c_str());
	mCanvasAnnotation->SetText(str, display->mAnnotation);

	// Move the children
	UpdateSize();
}

void THISCLASS::OnSize(wxSizeEvent &event) {
	UpdateSize();
}

void THISCLASS::UpdateSize() {
	// Get the sizes
	wxSize size = GetClientSize();
	wxSize titlesize = mCanvasTitle->GetClientSize();
	titlesize.SetHeight(20);
	wxSize annotationsize = mCanvasAnnotation->GetClientSize();
	annotationsize.SetHeight(20);

	// Set the available space (for future images)
	mAvailableSpace.SetWidth(size.GetWidth());
	mAvailableSpace.SetHeight(size.GetHeight() - titlesize.GetHeight() - annotationsize.GetHeight());

	if ((mAvailableSpace.GetWidth() < 30) || (mAvailableSpace.GetHeight() < 30)) {
		mCanvasTitle->Hide();
		mCanvas->Hide();
		mCanvasAnnotation->Hide();
		return;
	}

	// Get the maximal size of the canvas
	wxSize canvassize = mCanvas->GetMaximumSize();
	canvassize.DecTo(mAvailableSpace);

	// Layout the children
	int x = (size.GetWidth() - canvassize.GetWidth()) / 2;
	int y = (size.GetHeight() - canvassize.GetHeight() - titlesize.GetHeight() - annotationsize.GetHeight()) / 2;
	mCanvasTitle->SetSize(x, y, canvassize.GetWidth(), titlesize.GetHeight());
	y += titlesize.GetHeight();
	if (canvassize.GetWidth()*canvassize.GetHeight() == 0) {
		wxTrap();
	}
	mCanvas->SetSize(x, y, canvassize.GetWidth(), canvassize.GetHeight());
	y += canvassize.GetHeight();
	mCanvasAnnotation->SetSize(x, y, canvassize.GetWidth(), annotationsize.GetHeight());

	mCanvasTitle->Show();
	mCanvas->Show();
	mCanvasAnnotation->Show();
}
