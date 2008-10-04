#include "ConfigurationParameterCheckBox.h"
#define THISCLASS ConfigurationParameterCheckBox

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_CHECKBOX (wxID_ANY, THISCLASS::OnChecked)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterCheckBox(wxWindow* parent):
		ConfigurationParameter(parent),
		mValueDefault(wxT("")) {

}

THISCLASS::~ConfigurationParameterCheckBox() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueDefault = config->ReadBool(wxT("default"), false);

	// Create the controls
	mCheckBox = new wxCheckBox(this, wxID_ANY, config->ReadString(wxT("label"), wxT("")), wxDefaultPosition, wxSize(scParameterWidth, -1));

	// Layout the controls
	wxBoxSizer *hs = new wxBoxSizer(wxHORIZONTAL);
	hs->Add(mCheckBox, 0, wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(hs);
}

void THISCLASS::OnUpdate(wxWindow *updateprotection) {
	if (updateprotection == mCheckBox) {
		return;
	}
	bool value = mComponent->GetConfigurationBool(mName, mValueDefault);
	mCheckBox->SetValue(value);
}

bool THISCLASS::ValidateNewValue() {
	return true;
}

bool THISCLASS::CompareNewValue() {
	bool value = mComponent->GetConfigurationBool(mName, mValueDefault);
	return (value == mNewValue);
}

void THISCLASS::OnSetNewValue() {
	ComponentEditor ce(mComponent);
	ce.SetConfigurationBool(mName, mNewValue);
}

void THISCLASS::OnChecked(wxCommandEvent& event) {
	mNewValue = mCheckBox->GetValue();
	ValidateNewValue();
	if (CompareNewValue()) {
		return;
	}
	SetNewValue();
}
