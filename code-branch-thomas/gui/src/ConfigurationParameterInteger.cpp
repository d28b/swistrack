#include "ConfigurationParameterInteger.h"
#define THISCLASS ConfigurationParameterInteger

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
  EVT_COMMAND_TEXT_UPDATED (wxID_ANY, THISCLASS::OnTextUpdated)
  EVT_TEXT_ENTER (wxID_ANY, THISCLASS::OnTextEnter)
  EVT_BUTTON (eID_ButtonUp, THISCLASS::OnButtonUpClick)
  EVT_BUTTON (eID_ButtonDown, THISCLASS::OnButtonDownClick)
  EVT_LIST_ITEM_SELECTED (eID_List, THISCLASS::OnListItemSelected)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterInteger(wxWindow* parent, SwisTrack *st):
		ConfigurationParameter(parent, st) {

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
	mTextCtrl=new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(50, 25), wxTE_RIGHT|wxTE_PROCESS_ENTER);
	wxStaticText *unitlabel=new wxStaticText(this, -1, config->ReadString("unit", ""), wxDefaultPosition, wxSize(50, 25), wxST_NO_AUTORESIZE);

	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 1, 0, 0);
	hs->Add(mTextCtrl, 0, 0, 0);
	hs->Add(unitlabel, 0, 0, 0);
	SetSizer(hs);
}

void THISCLASS::OnUpdate() {
	int value=mComponent->GetConfigurationInt(mName, mValueDefault);
	mText->SetText(wxString::Format("%d", value));
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	long value=(long)mValueDefault;
	mText->GetText().ToLong(&value);
	if (value<mValueMin) {value=mValueMin;}
	if (value>mValueMax) {value=mValueMax;}
	mComponent->SetConfigurationInt(value);
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	long value=(long)mValueDefault;
	mText->GetText().ToLong(&value);
	if (value<mValueMin) {value=mValueMin;}
	if (value>mValueMax) {value=mValueMax;}
	mComponent->SetConfigurationInt(value);
}
