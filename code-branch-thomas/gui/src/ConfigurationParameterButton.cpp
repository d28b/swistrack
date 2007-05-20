#include "ConfigurationParameterButton.h"
#define THISCLASS ConfigurationParameterButton

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_BUTTON (wxID_ANY, THISCLASS::OnClick)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterButton(wxWindow* parent):
		ConfigurationParameter(parent) {

}

THISCLASS::~ConfigurationParameterButton() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Create the controls
	mButton=new wxButton(this, wxID_ANY, config->ReadString("label", ""), wxDefaultPosition, wxSize(scParameterWidth, -1));

	// Layout the controls
	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(mButton, 0, wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(hs);
}

void THISCLASS::OnUpdate(wxWindow *updateprotection) {
}

void THISCLASS::OnSetNewValue() {
	ComponentEditor ce(mComponent);
	ce.SetConfigurationBool(mName.c_str(), true);
}

void THISCLASS::OnClick(wxCommandEvent& event) {
	SetNewValue();
}
