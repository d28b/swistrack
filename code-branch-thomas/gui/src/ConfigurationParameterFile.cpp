#include "ConfigurationParameterFile.h"
#define THISCLASS ConfigurationParameterFile

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_TEXT (wxID_ANY, THISCLASS::OnTextUpdated)
	EVT_TEXT_ENTER (wxID_ANY, THISCLASS::OnTextEnter)
	EVT_BUTTON (wxID_ANY, THISCLASS::OnButtonClicked)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterFile(wxWindow* parent):
		ConfigurationParameter(parent),
		mTextCtrl(0), mButton(0),
		mValueDefault("") {

}

THISCLASS::~ConfigurationParameterFile() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueDefault=config->ReadString("default", "");
	mFileFilter=config->ReadString("filefilter", "All files|*.*");

	// Create the controls
	wxStaticText *label=new wxStaticText(this, -1, config->ReadString("label", ""), wxDefaultPosition, wxSize(75, -1), wxST_NO_AUTORESIZE);
	mTextCtrl=new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(75, -1), wxTE_RIGHT|wxTE_PROCESS_ENTER);
	mButton=new wxButton(this, -1, "...", wxDefaultPosition, wxSize(25, -1), wxST_NO_AUTORESIZE);

	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 1, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mTextCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mButton, 0, wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(hs);
}

void THISCLASS::OnUpdate() {
	wxString value=mComponent->GetConfigurationString(mName.c_str(), mValueDefault.c_str());
	mTextCtrl->ChangeValue(value);
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	SetValue(mTextCtrl->GetValue());
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	OnTextUpdated(event);
}

void THISCLASS::OnButtonClicked(wxCommandEvent& event) {
	wxFileDialog dlg(this, "Select file", "", mTextCtrl->GetValue(), mFileFilter, wxFD_OPEN);
	if (dlg.ShowModal() != wxID_OK) {return;}
	SetValue(dlg.GetPath());
}

void THISCLASS::SetValue(const wxString &value) {
	wxString curvalue=mComponent->GetConfigurationString(mName.c_str(), mValueDefault.c_str());
	if (curvalue==value) {return;}

	// Set the new configuration values
	ComponentEditor ce(mComponent);
	ce.SetConfigurationString(mName.c_str(), value.c_str());

	// Commit these changes
	CommitChanges();
}
