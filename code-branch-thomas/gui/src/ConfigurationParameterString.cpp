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
		mValueDefault(0) {

}

THISCLASS::~ConfigurationParameterString() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueDefault=config->ReadString("default", "");

	// Create the controls
	wxStaticText *label=new wxStaticText(this, -1, config->ReadString("label", ""), wxDefaultPosition, wxSize(75, -1), wxST_NO_AUTORESIZE);
	mTextCtrl=new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(100, -1), wxTE_RIGHT|wxTE_PROCESS_ENTER);

	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 1, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mTextCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(hs);
}

void THISCLASS::OnUpdate() {
	wxString value=mComponent->GetConfigurationString(mName.c_str(), mValueDefault);
	mTextCtrl->SetValue(value);
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	SetValue(mTextCtrl->GetValue());
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	OnTextUpdated(event);
}

void THISCLASS::SetValue(const wxString &value) {
	wxString curvalue=mComponent->GetConfigurationString(mName.c_str(), mValueDefault);
	if (curvalue==value) {return;}

	// Set the new configuration values
	ComponentEditor ce(mComponent);
	ce.SetConfigurationString(mName.c_str(), value);

	// Commit these changes
	CommitChanges();
}
