#include "ConfigurationParameterVideoFormat.h"
#define THISCLASS ConfigurationParameterVideoFormat

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include <cv.h>
#include <highgui.h>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_TEXT_ENTER (wxID_ANY, THISCLASS::OnTextEnter)
	EVT_BUTTON (wxID_ANY, THISCLASS::OnButtonClicked)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterVideoFormat(wxWindow* parent):
		ConfigurationParameter(parent),
		mTextCtrl(0), mButton(0),
		mValueDefault(wxT("")) {

}

THISCLASS::~ConfigurationParameterVideoFormat() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueDefault = config->ReadString(wxT("default"), wxT(""));

	// Create the controls
	wxStaticText *label = new wxStaticText(this, wxID_ANY, config->ReadString(wxT("label"), wxT("")), wxDefaultPosition, wxSize(scLabelWidth, -1), wxST_NO_AUTORESIZE);
	mTextCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(scParameterWidth - 25, -1), wxTE_PROCESS_ENTER);
	mTextCtrl->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
	mButton = new wxButton(this, wxID_ANY, wxT("..."), wxDefaultPosition, wxSize(25, -1), wxST_NO_AUTORESIZE);

	// Layout the controls
	wxBoxSizer *hs = new wxBoxSizer(wxHORIZONTAL);
	hs->Add(mTextCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mButton, 0, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer *vs = new wxBoxSizer(wxVERTICAL);
	vs->Add(label, 0, wxBOTTOM, 2);
	vs->Add(hs, 0, 0, 0);
	SetSizer(vs);
}

void THISCLASS::OnUpdate(wxWindow *updateprotection) {
	if (updateprotection == mTextCtrl) {
		return;
	}
	wxString value = mComponent->GetConfigurationString(mName, mValueDefault);
	mTextCtrl->SetValue(value);
}

bool THISCLASS::ValidateNewValue() {
	return true;
}

bool THISCLASS::CompareNewValue() {
	wxString value = mComponent->GetConfigurationString(mName, mValueDefault);
	return (value == mNewValue);
}

void THISCLASS::OnSetNewValue() {
	ComponentEditor ce(mComponent);
	ce.SetConfigurationString(mName, mNewValue);
}

void THISCLASS::OnButtonClicked(wxCommandEvent& event) {
	CvVideoWriter *writer = cvCreateVideoWriter("temp.avi", -1, 15, cvSize(640, 480));
	cvReleaseVideoWriter(&writer);
	CvCapture *capture = cvCaptureFromFile("temp.avi");
	double fourcc = cvGetCaptureProperty(capture, CV_CAP_PROP_FOURCC);
	cvReleaseCapture(&capture);

	mNewValue = wxString::Format(wxT("%f"), fourcc);
	ValidateNewValue();
	SetNewValue();
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	mNewValue = mTextCtrl->GetValue();
	ValidateNewValue();
	SetNewValue();
}

void THISCLASS::OnKillFocus(wxFocusEvent& event) {
	wxCommandEvent ev;
	OnTextEnter(ev);
}
