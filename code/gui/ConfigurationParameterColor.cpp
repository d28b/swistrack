#include "ConfigurationParameterColor.h"
#define THISCLASS ConfigurationParameterColor

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/colordlg.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_TEXT_ENTER (wxID_ANY, THISCLASS::OnTextEnter)
	EVT_BUTTON (wxID_ANY, THISCLASS::OnButtonClicked)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterColor(wxWindow* parent):
		ConfigurationParameter(parent),
		mTextCtrl(0), mButton(0),
		mValueDefault(0) {

}

THISCLASS::~ConfigurationParameterColor() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueDefault = config->ReadInt("default", 0);

	// Create the controls
	wxStaticText *label = new wxStaticText(this, wxID_ANY, config->ReadString("label", ""), wxDefaultPosition, wxSize(scLabelWidth, -1), wxST_NO_AUTORESIZE);
	mTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(scParameterWidth - 25, -1), wxTE_PROCESS_ENTER);
	mTextCtrl->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
	mButton = new wxButton(this, wxID_ANY, "...", wxDefaultPosition, wxSize(25, -1), wxST_NO_AUTORESIZE);

	// Layout the controls
	wxBoxSizer *hs = new wxBoxSizer(wxHORIZONTAL);
	hs->Add(mTextCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mButton, 0, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer *vs = new wxBoxSizer(wxVERTICAL);
	vs->Add(label, 0, wxBOTTOM, 2);
	vs->Add(hs, 0, 0, 0);
	SetSizer(vs);
}

void THISCLASS::OnUpdate(wxWindow *updateprotection) {
	if (updateprotection == mTextCtrl) {
		return;
	}
	int value = mComponent->GetConfigurationInt(mName.c_str(), mValueDefault);
	mTextCtrl->SetValue(wxColour(value).GetAsString(wxC2S_HTML_SYNTAX));
}

bool THISCLASS::ValidateNewValue() {
	return true;
}

bool THISCLASS::CompareNewValue() {
	int value = mComponent->GetConfigurationInt(mName.c_str(), mValueDefault);
	return (value == mNewValue);
}

void THISCLASS::OnSetNewValue() {
	ComponentEditor ce(mComponent);
	ce.SetConfigurationInt(mName.c_str(), mNewValue);
}

void THISCLASS::OnButtonClicked(wxCommandEvent& event) {
	wxColourDialog dlg(this);
	dlg.GetColourData().SetColour(wxColour(mTextCtrl->GetValue()));
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}
	mNewValue = ColorToInt(dlg.GetColourData().GetColour());
	ValidateNewValue();
	SetNewValue();
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	//wxFocusEvent ev;
	//OnKillFocus(ev);
}

void THISCLASS::OnKillFocus(wxFocusEvent& event) {
	mNewValue = ColorToInt(wxColour(mTextCtrl->GetValue()));
	ValidateNewValue();
	SetNewValue();
}

int THISCLASS::ColorToInt(wxColour &color) {
	return color.Red()+(color.Green()<<8)+(color.Blue()<<16);
}
