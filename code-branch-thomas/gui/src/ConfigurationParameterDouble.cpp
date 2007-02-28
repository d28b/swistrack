#include "ConfigurationParameterDouble.h"
#define THISCLASS ConfigurationParameterDouble

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_TEXT (wxID_ANY, THISCLASS::OnTextUpdated)
	EVT_TEXT_ENTER (wxID_ANY, THISCLASS::OnTextEnter)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterDouble(wxWindow* parent):
		ConfigurationParameter(parent),
		mSpinCtrl(0), mSlider(0),
		mValueMin(0), mValueMax(1), mValueDefault(0), mSliderStep(1) {

}

THISCLASS::~ConfigurationParameterDouble() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueMin=config->ReadDouble("min", 0);
	mValueMax=config->ReadDouble("max", 1);
	mValueDefault=config->ReadDouble("default", 0);

	// Create the controls
	wxStaticText *label=new wxStaticText(this, -1, config->ReadString("label", ""), wxDefaultPosition, wxSize(75, -1), wxST_NO_AUTORESIZE);
	mTextCtrl=new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT|wxTE_PROCESS_ENTER);
	wxStaticText *unitlabel=new wxStaticText(this, -1, " "+config->ReadString("unit", ""), wxDefaultPosition, wxSize(50, -1), wxST_NO_AUTORESIZE);

	if (config->ReadBool("slider", false)) {
		mSliderStep=config->ReadDouble("sliderstep", 1);
		mSlider = new wxSlider(this, -1, (int)floor((mValueDefault-mValueMin)/mSliderStep+0.5), 0, (int)floor((mValueMax-mValueMin)/mSliderStep+0.5), wxDefaultPosition, wxSize(175,-1)); //wxSL_AUTOTICKS
	}

	// Layout the controls
	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 1, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mTextCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(unitlabel, 0, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(hs, 0, 0, 0);
	if (mSlider) {vs->Add(mSlider, 0, 0, 0);}
	SetSizer(vs);
}

void THISCLASS::OnUpdate() {
	double value=mComponent->GetConfigurationDouble(mName.c_str(), mValueDefault);
	if (mFocusWindow!=mTextCtrl) {mTextCtrl->SetValue(wxString::Format("%f", value));}
	if (mFocusWindow!=mSlider) {mSlider->SetValue((int)floor((mValueMax-mValueMin)/mSliderStep+0.5));}
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	double value=mValueDefault;
	mTextCtrl->GetValue().ToDouble(&value);
	SetValue(value);
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	OnTextUpdated(event);
}

void THISCLASS::OnScrollChanged(wxScrollEvent& event) {
	SetValue(mSlider->GetValue()*nSliderStep+mValueMin);
}

void THISCLASS::SetValue(double value) {
	// If we are in OnUpdate(), do nothing
	if (mUpdating) {return;}

	// Check bounds
	if (value<mValueMin) {value=mValueMin;}
	if (value>mValueMax) {value=mValueMax;}

	// Check if the same value is set already
	double curvalue=mComponent->GetConfigurationDouble(mName.c_str(), mValueDefault);
	if (curvalue==value) {return;}

	// Set the new configuration values
	ComponentEditor ce(mComponent);
	ce.SetConfigurationDouble(mName.c_str(), value);

	// Commit these changes
	CommitChanges();
}
