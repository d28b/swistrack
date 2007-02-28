#include "ConfigurationParameterInteger.h"
#define THISCLASS ConfigurationParameterInteger

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_TEXT (wxID_ANY, THISCLASS::OnTextUpdated)
	EVT_TEXT_ENTER (wxID_ANY, THISCLASS::OnTextEnter)
	EVT_SPINCTRL (wxID_ANY, THISCLASS::OnSpin)
	EVT_COMMAND_SCROLL_CHANGED (wxID_ANY, THISCLASS::OnScrollChanged)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterInteger(wxWindow* parent):
		ConfigurationParameter(parent),
		mSpinCtrl(0), mSlider(0),
		mValueMin(0), mValueMax(255), mValueDefault(0) {

}

THISCLASS::~ConfigurationParameterInteger() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueMin=config->ReadInt("min", 0);
	mValueMax=config->ReadInt("max", 255);
	mValueDefault=config->ReadInt("default", 0);

	// Create the controls
	wxStaticText *label=new wxStaticText(this, -1, config->ReadString("label", ""), wxDefaultPosition, wxSize(75, -1), wxST_NO_AUTORESIZE);
	mSpinCtrl=new wxSpinCtrl(this, -1, "", wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT|wxTE_PROCESS_ENTER, mValueMin, mValueMax, mValueDefault);
	wxStaticText *unitlabel=new wxStaticText(this, -1, " "+config->ReadString("unit", ""), wxDefaultPosition, wxSize(50, -1), wxST_NO_AUTORESIZE);

	if (config->ReadBool("slider", false)) {
		mSlider = new wxSlider(this, -1, mValueDefault, mValueMin, mValueMax, wxDefaultPosition, wxSize(175,-1)); //wxSL_AUTOTICKS
	}

	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 1, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mSpinCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(unitlabel, 0, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(hs, 0, 0, 0);
	if (mSlider) {vs->Add(mSlider, 0, 0, 0);}
	SetSizer(vs);
}

void THISCLASS::OnUpdate() {
	int value=mComponent->GetConfigurationInt(mName.c_str(), mValueDefault);
	mSpinCtrl->SetValue(value);
	mSlider->SetValue(value);
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	SetValue(mSpinCtrl->GetValue());
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	OnTextUpdated(event);
}

void THISCLASS::OnSpin(wxSpinEvent& event) {
	SetValue(mSpinCtrl->GetValue());
}

void THISCLASS::OnScrollChanged(wxScrollEvent& event) {
	SetValue(mSlider->GetValue());
}

void THISCLASS::SetValue(int value) {
	if (value<mValueMin) {value=mValueMin;}
	if (value>mValueMax) {value=mValueMax;}
	int curvalue=mComponent->GetConfigurationInt(mName.c_str(), mValueDefault);
	if (curvalue==value) {return;}

	// Set the new configuration values
	ComponentEditor ce(mComponent);
	ce.SetConfigurationInt(mName.c_str(), value);

	// Commit these changes
	CommitChanges();
}
