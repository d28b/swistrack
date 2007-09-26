#include "ConfigurationParameterString.h"
#define THISCLASS ConfigurationParameterString

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
  EVT_TEXT (wxID_ANY, THISCLASS::OnTextUpdated)
  EVT_TEXT_ENTER (wxID_ANY, THISCLASS::OnTextEnter)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterString(wxWindow* parent):
		ConfigurationParameter(parent),
		mValueDefault("") {

}

THISCLASS::~ConfigurationParameterString() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueDefault=config->ReadString("default", "");

	// Create the controls
	wxStaticText *label=new wxStaticText(this, wxID_ANY, config->ReadString("label", ""), wxDefaultPosition, wxSize(scLabelWidth, -1), wxST_NO_AUTORESIZE);
	mTextCtrl=new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(scTextBoxWidth+scUnitWidth, -1), wxTE_PROCESS_ENTER);
	mTextCtrl->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);

	// Layout the controls
	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mTextCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(hs);
}

void THISCLASS::OnUpdate(wxWindow *updateprotection) {
	if (updateprotection==mTextCtrl) {return;}
	wxString value=mComponent->GetConfigurationString(mName.c_str(), mValueDefault.c_str());
	mTextCtrl->SetValue(value);
}

bool THISCLASS::ValidateNewValue() {
	return true;
}

bool THISCLASS::CompareNewValue() {
	wxString value=mComponent->GetConfigurationString(mName.c_str(), mValueDefault.c_str());
	return (value==mNewValue);
}

void THISCLASS::OnSetNewValue() {
	ComponentEditor ce(mComponent);
	ce.SetConfigurationString(mName.c_str(), mNewValue.c_str());
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	mNewValue=mTextCtrl->GetValue();
	ValidateNewValue();
	if (CompareNewValue()) {return;}
	SetNewValue(mTextCtrl);
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	mNewValue=mTextCtrl->GetValue();
	ValidateNewValue();
	SetNewValue();
}

void THISCLASS::OnKillFocus(wxFocusEvent& event) {
    wxCommandEvent e;
	OnTextEnter(e);
}
