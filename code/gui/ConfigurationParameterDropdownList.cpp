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
		mValueDefault(wxT("")) {

}

THISCLASS::~ConfigurationParameterDropdownList() {
	int cc = mComboBox->GetCount();
	for (int i = 0; i < cc; i++) {
		wxString *itemvalue = (wxString *)(mComboBox->GetClientData(i));
		mComboBox->SetClientData(i, 0);
		delete itemvalue;
	}
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueDefault = config->ReadString(wxT("default"), wxT(""));

	// Create the controls
	wxStaticText *label = new wxStaticText(this, wxID_ANY, config->ReadString(wxT("label"), wxT("")), wxDefaultPosition, wxSize(scLabelWidth, -1), wxST_NO_AUTORESIZE);
	mComboBox = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(scTextBoxWidth + scUnitWidth, -1), 0, NULL, wxCB_READONLY);

	// Layout the controls
	wxBoxSizer *hs = new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mComboBox, 0, wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(hs);

	// Fill in the list
	FillList(config, errorlist);
}

void THISCLASS::FillList(ConfigurationXML *config, ErrorList *errorlist) {
	wxXmlNode *items = config->GetChildNode(wxT("items"));
	wxXmlNode *node = items->GetChildren();
	while (node) {
		// Create one list item per "item" node
		if (node->GetName() == wxT("item")) {
			wxString key(wxT(""));
			wxXmlProperty *prop = node->GetProperties();
			while (prop) {
				if (prop->GetName() == wxT("key")) {
					key = prop->GetValue();
				}
				prop = prop->GetNext();
			}
			AddItem(key, node->GetNodeContent());
		}

		// Next node
		node = node->GetNext();
	}
}

void THISCLASS::AddItem(const wxString &key, const wxString &label) {
	wxString *itemvalue = new wxString(key);
	mComboBox->Append(label, itemvalue);
}

void THISCLASS::OnUpdate(wxWindow *updateprotection) {
	if (updateprotection == mComboBox) {
		return;
	}

	// Fetch the configuration value
	wxString value = mComponent->GetConfigurationString(mName, mValueDefault);

	// Select the appropriate item in the list
	int cc = mComboBox->GetCount();
	for (int i = 0; i < cc; i++) {
		wxString *itemvalue = (wxString *)(mComboBox->GetClientData(i));
		if (*itemvalue == value) {
			mComboBox->SetSelection(i);
			return;
		}
	}

	// If the item is not available, add a new entry to the list
	wxString *itemvalue = new wxString(value);
	mComboBox->Append(value, itemvalue);
}

bool THISCLASS::ValidateNewValue() {
	return true;
}

bool THISCLASS::CompareNewValue() {
	wxString value = mComponent->GetConfigurationString(mName, mValueDefault);
	return (value == mNewValue);
}

void THISCLASS::OnSetNewValue() {
	ComponentEditor ce(mComponent);
	ce.SetConfigurationString(mName, mNewValue);
}

void THISCLASS::OnItemSelected(wxCommandEvent& event) {
	int sel = mComboBox->GetSelection();
	if (sel == wxNOT_FOUND) {
		return;
	}
	wxString *itemvalue = (wxString *)mComboBox->GetClientData(sel);
	mNewValue = *itemvalue;
	ValidateNewValue();
	if (CompareNewValue()) {
		return;
	}
	SetNewValue(mComboBox);
}
