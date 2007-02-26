#include "ConfigurationParameterInteger.h"
#define THISCLASS ConfigurationParameterInteger

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
  EVT_TEXT (wxID_ANY, THISCLASS::OnTextUpdated)
  EVT_TEXT_ENTER (wxID_ANY, THISCLASS::OnTextEnter)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterInteger(wxWindow* parent):
		ConfigurationParameter(parent),
		mValueMin(0), mValueMax(255), mValueDefault(0) {

}

THISCLASS::~ConfigurationParameterInteger() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config) {
	// Read specific configuration
	config->SelectRootNode();
	mValueMin=config->ReadInt("min", 0);
	mValueMax=config->ReadInt("max", 255);
	mValueDefault=config->ReadInt("default", 0);

	// Create the controls
	wxStaticText *label=new wxStaticText(this, -1, config->ReadString("name", ""), wxDefaultPosition, wxSize(100, 25), wxST_NO_AUTORESIZE);
	mTextCtrl=new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(50, 25), wxTE_RIGHT|wxTE_PROCESS_ENTER);
	wxStaticText *unitlabel=new wxStaticText(this, -1, config->ReadString("unit", ""), wxDefaultPosition, wxSize(50, 25), wxST_NO_AUTORESIZE);

	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 1, 0, 0);
	hs->Add(mTextCtrl, 0, 0, 0);
	hs->Add(unitlabel, 0, 0, 0);
	SetSizer(hs);
}

void THISCLASS::OnUpdate() {
	int value=mComponent->GetConfigurationInt(mName.c_str(), mValueDefault);
	mTextCtrl->SetValue(wxString::Format("%d", value));
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	long value=(long)mValueDefault;
	mTextCtrl->GetValue().ToLong(&value);
	if (value<mValueMin) {value=mValueMin;}
	if (value>mValueMax) {value=mValueMax;}
	int curvalue=mComponent->GetConfigurationInt(mName.c_str(), mValueDefault);
	if (curvalue==value) {return;}

	mComponent->SetConfigurationInt(mName.c_str(), value);
	CommitChanges();
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	OnTextUpdated(event);
}
