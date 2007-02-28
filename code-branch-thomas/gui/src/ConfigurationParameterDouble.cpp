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
		mValueMin(0), mValueMax(1), mValueDefault(0) {

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
	wxStaticText *unitlabel=new wxStaticText(this, -1, " "+config->ReadString("unit", ""), wxDefaultPosition, wxSize(75, -1), wxST_NO_AUTORESIZE);

	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 1, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mTextCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(unitlabel, 0, wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(hs);
}

void THISCLASS::OnUpdate() {
	double value=mComponent->GetConfigurationDouble(mName.c_str(), mValueDefault);
	mTextCtrl->SetValue(wxString::Format("%f", value));
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	double value=mValueDefault;
	mTextCtrl->GetValue().ToDouble(&value);
	SetValue(value);
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	OnTextUpdated(event);
}

void THISCLASS::SetValue(double value) {
	if (value<mValueMin) {value=mValueMin;}
	if (value>mValueMax) {value=mValueMax;}
	double curvalue=mComponent->GetConfigurationDouble(mName.c_str(), mValueDefault);
	if (curvalue==value) {return;}

	// Set the new configuration values
	ComponentEditor ce(mComponent);
	ce.SetConfigurationDouble(mName.c_str(), value);

	// Commit these changes
	CommitChanges();
}
