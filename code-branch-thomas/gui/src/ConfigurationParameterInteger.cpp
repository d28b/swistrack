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
	mValueMin=config->ReadInt("min", INT_MIN);
	mValueMax=config->ReadInt("max", INT_MAX);
	mValueDefault=config->ReadInt("default", 0);

	// Create the controls
	wxStaticText *label=new wxStaticText(this, wxID_ANY, config->ReadString("label", ""), wxDefaultPosition, wxSize(scLabelWidth, -1), wxST_NO_AUTORESIZE);
	mSpinCtrl=new wxSpinCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(scTextBoxWidth, -1), wxTE_RIGHT|wxTE_PROCESS_ENTER, mValueMin, mValueMax, mValueDefault);
	mSpinCtrl->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
	wxStaticText *unitlabel=new wxStaticText(this, wxID_ANY, " "+config->ReadString("unit", ""), wxDefaultPosition, wxSize(scUnitWidth, -1), wxST_NO_AUTORESIZE);

	if (config->ReadBool("slider", false)) {
		mSlider = new wxSlider(this, wxID_ANY, mValueDefault, mValueMin, mValueMax, wxDefaultPosition, wxSize(175,-1)); //wxSL_AUTOTICKS
	}

	// Layout the controls
	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mSpinCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(unitlabel, 0, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(hs, 0, 0, 0);
	if (mSlider) {vs->Add(mSlider, 0, 0, 0);}
	SetSizer(vs);
}

void THISCLASS::OnUpdate(wxWindow *updateprotection) {
	int value=mComponent->GetConfigurationInt(mName.c_str(), mValueDefault);
	if (updateprotection!=mSpinCtrl) {mSpinCtrl->SetValue(value);}
	if ((mSlider) && (updateprotection!=mSlider)) {mSlider->SetValue(value);}
}

bool THISCLASS::ValidateNewValue() {
	bool valid=true;

	// Check bounds
	if (mNewValue<mValueMin) {mNewValue=mValueMin; valid=false;}
	if (mNewValue>mValueMax) {mNewValue=mValueMax; valid=false;}

	return valid;
}

bool THISCLASS::CompareNewValue() {
	int value=mComponent->GetConfigurationInt(mName.c_str(), mValueDefault);
	return (value==mNewValue);
}

void THISCLASS::OnSetNewValue() {
	ComponentEditor ce(mComponent);
	ce.SetConfigurationInt(mName.c_str(), mNewValue);
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	mNewValue=mSpinCtrl->GetValue();

	if (ValidateNewValue()) {
		mSpinCtrl->SetOwnForegroundColour(*wxBLACK);
	} else {
		mSpinCtrl->SetOwnForegroundColour(*wxRED);
	}
	mSpinCtrl->Refresh();

	if (! CompareNewValue()) {return;}
	SetNewValue(mSpinCtrl);
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	mNewValue=mSpinCtrl->GetValue();
	ValidateNewValue();
	SetNewValue();
}

void THISCLASS::OnSpin(wxSpinEvent& event) {
	mNewValue=mSpinCtrl->GetValue();
	ValidateNewValue();
	if (CompareNewValue()) {return;}
	SetNewValue(mSpinCtrl);
}

void THISCLASS::OnKillFocus(wxFocusEvent& event) {
	OnTextEnter(wxCommandEvent());
}

void THISCLASS::OnScrollChanged(wxScrollEvent& event) {
	mNewValue=mSlider->GetValue();
	ValidateNewValue();
	if (CompareNewValue()) {return;}
	SetNewValue(mSlider);
}
