#include "ConfigurationParameterAngle.h"
#define THISCLASS ConfigurationParameterAngle

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"
#include <cmath>

#define PI (std::acos(-1.0))

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_TEXT (wxID_ANY, THISCLASS::OnTextUpdated)
	EVT_TEXT_ENTER (wxID_ANY, THISCLASS::OnTextEnter)
	EVT_COMMAND_SCROLL_CHANGED (wxID_ANY, THISCLASS::OnScrollChanged)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterAngle(wxWindow* parent):
		ConfigurationParameter(parent),
		mTextCtrl(0), mSlider(0),
		mValueMin(0), mValueMax(1), mValueDefault(0), mSliderStep(1) {

}

THISCLASS::~ConfigurationParameterAngle() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueMin=config->ReadDouble("min", 0);
	mValueMax=config->ReadDouble("max", 2*PI);
	mValueDefault=config->ReadDouble("default", 0);

	// Create the controls
	wxStaticText *label=new wxStaticText(this, wxID_ANY, config->ReadString("label", ""), wxDefaultPosition, wxSize(scLabelWidth, -1), wxST_NO_AUTORESIZE);
	mTextCtrl=new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(scTextBoxWidth, -1), wxTE_RIGHT|wxTE_PROCESS_ENTER);
	mTextCtrl->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
	wxStaticText *unitlabel=new wxStaticText(this, wxID_ANY, " deg", wxDefaultPosition, wxSize(scUnitWidth, -1), wxST_NO_AUTORESIZE);

	if (config->ReadBool("slider", false)) {
		mSliderStep=config->ReadDouble("sliderstep", PI/180);
		mSlider = new wxSlider(this, wxID_ANY, (int)floor((mValueDefault-mValueMin)/mSliderStep+0.5), 0, (int)floor((mValueMax-mValueMin)/mSliderStep+0.5), wxDefaultPosition, wxSize(scParameterWidth, -1), wxSL_AUTOTICKS);
	}

	// Layout the controls
	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mTextCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(unitlabel, 0, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(hs, 0, 0, 0);
	if (mSlider) {vs->Add(mSlider, 0, 0, 0);}
	SetSizer(vs);
}

void THISCLASS::OnUpdate(wxWindow *updateprotection) {
	double value=mComponent->GetConfigurationDouble(mName.c_str(), mValueDefault);
	if (updateprotection!=mTextCtrl) {mTextCtrl->SetValue(wxString::Format("%f", value*180/PI));}
	if ((mSlider) && (updateprotection!=mSlider)) {mSlider->SetValue((int)floor((value-mValueMin)/mSliderStep+0.5));}
}

bool THISCLASS::ValidateNewValue() {
	bool valid=true;

	// Check bounds
	if (mNewValue<mValueMin) {mNewValue=mValueMin; valid=false;}
	if (mNewValue>mValueMax) {mNewValue=mValueMax; valid=false;}

	return valid;
}

bool THISCLASS::CompareNewValue() {
	double value=mComponent->GetConfigurationDouble(mName.c_str(), mValueDefault);
	return (value==mNewValue);
}

void THISCLASS::OnSetNewValue() {
	ComponentEditor ce(mComponent);
	ce.SetConfigurationDouble(mName.c_str(), mNewValue);
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	mNewValue=mValueDefault;
	mTextCtrl->GetValue().ToDouble(&mNewValue);
	mNewValue=mNewValue*PI/180;

	if (ValidateNewValue()) {
		mTextCtrl->SetOwnForegroundColour(*wxBLACK);
	} else {
		mTextCtrl->SetOwnForegroundColour(*wxRED);
	}
	mTextCtrl->Refresh();

	if (CompareNewValue()) {return;}
	SetNewValue(mTextCtrl);
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	mNewValue=mValueDefault;
	mTextCtrl->GetValue().ToDouble(&mNewValue);
	mNewValue=mNewValue*PI/180;
	ValidateNewValue();
	SetNewValue();
}

void THISCLASS::OnKillFocus(wxFocusEvent& event) {
	OnTextEnter(wxCommandEvent());
}

void THISCLASS::OnScrollChanged(wxScrollEvent& event) {
	mNewValue=mSlider->GetValue()*mSliderStep+mValueMin;
	ValidateNewValue();
	if (CompareNewValue()) {return;}
	SetNewValue(mSlider);
}