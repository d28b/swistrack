#include "ConfigurationParameterString.h"
#define THISCLASS ConfigurationParameterString

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
  EVT_COMBOBOX (wxID_ANY, THISCLASS::OnItemSelected)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterString(wxWindow* parent):
		ConfigurationParameter(parent),
		mValueDefault(0) {

}

THISCLASS::~ConfigurationParameterString() {
	int cc=mComboBox.GetCount();
	for (int i=0; i<cc; i++) {
		wxString *itemvalue=mComboBox.GetClientData(i);
		mComboBox.SetClientData(i, 0);
		delete itemvalue;
	}
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueDefault=config->ReadString("default", "");

	// Create the controls
	wxStaticText *label=new wxStaticText(this, -1, config->ReadString("label", ""), wxDefaultPosition, wxSize(75, -1), wxST_NO_AUTORESIZE);
	mComboBox=new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(100, -1), wxCB_READONLY);

	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 1, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mTextCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(hs);

	// Fill the list
	wxXmlNode *items=config->GetChildNode("items");
	wxXmlNode *node=items->GetFirstChild();
	while (node) {
		if (node.GetName()=="item") {
			wxString *itemvalue=new wxString("");
			wxXmlProperty *prop=node->GetProperties();
			while (prop) {
				if (prop->GetName()=="key") {itemvalue=prop->GetValue();}
				prop=prop->GetNext();
			}
			mComboBox->Append(node->GetNodeContent(), itemvalue);
		}
		node=node->GetNext();
	}
}

void THISCLASS::OnUpdate() {
	// Fetch the configuration value
	wxString value=mComponent->GetConfigurationString(mName.c_str(), mValueDefault);

	// Select the appropriate item in the list
	int cc=mComboBox.GetCount();
	for (int i=0; i<cc; i++) {
		wxString *itemvalue=mComboBox.GetClientData(i);
		if (*itemvalue==value) {
			mComboBox.SetSelection(i);
			return;
		}
	}

	// If the item is not available, add a new entry to the list
	wxString *itemvalue=new wxString(value);
	mComboBox->Append(value, itemvalue);
}

void THISCLASS::OnItemSelected(wxCommandEvent& event) {
	int sel=mComboBox.GetSelection();
	if (sel==wxNOT_FOUND) {return;}
	SetValue(*(mComboBox.GetClientData(sel)));
}

void THISCLASS::SetValue(const wxString &value) {
	wxString curvalue=mComponent->GetConfigurationString(mName.c_str(), mValueDefault);
	if (curvalue==value) {return;}

	// Set the new configuration values
	ComponentEditor ce(mComponent);
	ce.SetConfigurationString(mName.c_str(), value);

	// Commit these changes
	CommitChanges();
}
