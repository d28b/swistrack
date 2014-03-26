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
		mValueDefault(*wxBLACK) {

}

THISCLASS::~ConfigurationParameterColor() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueDefault = config->ReadInt(wxT("default"), 0);

	// Create the controls
	wxStaticText *label = new wxStaticText(this, wxID_ANY, config->ReadString(wxT("label"), wxT("")), wxDefaultPosition, wxSize(scLabelWidth, -1), wxST_NO_AUTORESIZE);
	mTextCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(scParameterWidth - 25, -1), wxTE_PROCESS_ENTER);
	mTextCtrl->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
	mButton = new wxButton(this, wxID_ANY, wxT("..."), wxDefaultPosition, wxSize(25, -1), wxST_NO_AUTORESIZE);

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
	wxColor value = mComponent->GetConfigurationColor(mName, mValueDefault);
	mTextCtrl->SetValue(wxColour(value).GetAsString(wxC2S_HTML_SYNTAX));
}

bool THISCLASS::ValidateNewValue() {
	return true;
}

bool THISCLASS::CompareNewValue() {
	wxColor value = mComponent->GetConfigurationColor(mName, mValueDefault);
	return (value == mNewValue);
}

void THISCLASS::OnSetNewValue() {
	ComponentEditor ce(mComponent);
	ce.SetConfigurationColor(mName, mNewValue);
}

void THISCLASS::OnButtonClicked(wxCommandEvent& event) {
	wxColourDialog dlg(this);
	dlg.GetColourData().SetColour(wxColour(mTextCtrl->GetValue()));
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}
	mNewValue = dlg.GetColourData().GetColour();
	ValidateNewValue();
	SetNewValue();
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	//wxFocusEvent ev;
	//OnKillFocus(ev);
}

void THISCLASS::OnKillFocus(wxFocusEvent& event) {
	wxColour color = wxColour(mTextCtrl->GetValue());
	mNewValue = color;
	ValidateNewValue();
	SetNewValue();
}


