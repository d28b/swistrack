#include "ConfigurationParameterFile.h"
#define THISCLASS ConfigurationParameterFile

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
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
	wxStaticText *label=new wxStaticText(this, wxID_ANY, config->ReadString("label", ""), wxDefaultPosition, wxSize(scLabelWidth, -1), wxST_NO_AUTORESIZE);
	mTextCtrl=new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(scParameterWidth-25, -1), wxTE_PROCESS_ENTER);
	mTextCtrl->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
	mButton=new wxButton(this, wxID_ANY, "...", wxDefaultPosition, wxSize(25, -1), wxST_NO_AUTORESIZE);

	// Layout the controls
	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(mTextCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mButton, 0, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(label, 0, wxBOTTOM, 2);
	vs->Add(hs, 0, 0, 0);
	SetSizer(vs);
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

void THISCLASS::OnButtonClicked(wxCommandEvent& event) {
	wxFileDialog dlg(this, "Select file", "", mTextCtrl->GetValue(), mFileFilter, wxFD_OPEN);
	if (dlg.ShowModal() != wxID_OK) {return;}
	mNewValue=dlg.GetPath();
	ValidateNewValue();
	SetNewValue();
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
