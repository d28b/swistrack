#include "ConfigurationParameterDropdownList.h"
#define THISCLASS ConfigurationParameterDropdownList

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
  EVT_COMBOBOX (wxID_ANY, THISCLASS::OnItemSelected)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterDropdownList(wxWindow* parent):
		ConfigurationParameter(parent),
		mValueDefault("") {

}

THISCLASS::~ConfigurationParameterDropdownList() {
	int cc=mComboBox->GetCount();
	for (int i=0; i<cc; i++) {
		wxString *itemvalue=(wxString *)(mComboBox->GetClientData(i));
		mComboBox->SetClientData(i, 0);
		delete itemvalue;
	}
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueDefault=config->ReadString("default", "");

	// Create the controls
	wxStaticText *label=new wxStaticText(this, -1, config->ReadString("label", ""), wxDefaultPosition, wxSize(75, -1), wxST_NO_AUTORESIZE);
	mComboBox=new wxComboBox(this, -1, "", wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY);

	// Layout the controls
	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 1, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mComboBox, 0, wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(hs);

	// Fill the list
	wxXmlNode *items=config->GetChildNode("items");
	wxXmlNode *node=items->GetChildren();
	while (node) {
		if (node->GetName()=="item") {
			wxString *itemvalue=new wxString("");
			wxXmlProperty *prop=node->GetProperties();
			while (prop) {
				if (prop->GetName()=="key") {*itemvalue=prop->GetValue();}
				prop=prop->GetNext();
			}
			mComboBox->Append(node->GetNodeContent(), itemvalue);
		}
		node=node->GetNext();
	}
}

void THISCLASS::OnUpdate() {
	if (mFocusWindow==mComboBox) {return;}
		
	// Fetch the configuration value
	wxString value=mComponent->GetConfigurationString(mName.c_str(), mValueDefault.c_str());

	// Select the appropriate item in the list
	int cc=mComboBox->GetCount();
	for (int i=0; i<cc; i++) {
		wxString *itemvalue=(wxString *)(mComboBox->GetClientData(i));
		if (*itemvalue==value) {
			mComboBox->SetSelection(i);
			return;
		}
	}

	// If the item is not available, add a new entry to the list
	wxString *itemvalue=new wxString(value);
	mComboBox->Append(value, itemvalue);
}

void THISCLASS::OnItemSelected(wxCommandEvent& event) {
	int sel=mComboBox->GetSelection();
	if (sel==wxNOT_FOUND) {return;}
	wxString *itemvalue=(wxString *)mComboBox->GetClientData(sel);
	SetValue(*itemvalue);
}

void THISCLASS::SetValue(const wxString &value) {
	// If we are in OnUpdate(), do nothing
	if (mUpdating) {return;}

	// Check if the same value is set already
	wxString curvalue=mComponent->GetConfigurationString(mName.c_str(), mValueDefault.c_str());
	if (curvalue==value) {return;}

	// Set the new configuration values
	ComponentEditor ce(mComponent);
	ce.SetConfigurationString(mName.c_str(), value.c_str());

	// Commit these changes
	CommitChanges();
}
