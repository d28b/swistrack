#include "ConfigurationParameterDate.h"
#define THISCLASS ConfigurationParameterDate

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/dateevt.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
       EVT_DATE_CHANGED(wxID_ANY, THISCLASS::OnDateChanged)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterDate(wxWindow* parent):
		ConfigurationParameter(parent),
		mDateCtrl(0),
		mValueDefault() {

}

THISCLASS::~ConfigurationParameterDate() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();


	// Create the controls
	wxStaticText *label = new wxStaticText(this, wxID_ANY, config->ReadString(wxT("label"), wxT("")), wxDefaultPosition, wxSize(scLabelWidth, -1), wxST_NO_AUTORESIZE);
	mDateCtrl = new wxDatePickerCtrl(this, wxID_ANY, mValueDefault, wxDefaultPosition, wxSize(scParameterWidth - 25, -1), wxTE_PROCESS_ENTER);
	mDateCtrl->Connect(wxID_ANY, wxEVT_DATE_CHANGED, wxDateEventHandler(THISCLASS::OnDateChanged), 0, this);

	// Layout the controls
	wxBoxSizer *hs = new wxBoxSizer(wxHORIZONTAL);
	hs->Add(mDateCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer *vs = new wxBoxSizer(wxVERTICAL);
	vs->Add(label, 0, wxBOTTOM, 2);
	vs->Add(hs, 0, 0, 0);
	SetSizer(vs);
}

void THISCLASS::OnUpdate(wxWindow *updateprotection) {
	if (updateprotection == mDateCtrl) {
		return;
	}
	wxDateTime value = mComponent->GetConfigurationDate(mName, mValueDefault);
	mDateCtrl->SetValue(value);
}

bool THISCLASS::ValidateNewValue() {
	return true;
}

bool THISCLASS::CompareNewValue() {
	wxDateTime value = mComponent->GetConfigurationDate(mName, mValueDefault);
	return (value == mNewValue);
}

void THISCLASS::OnSetNewValue() {
	ComponentEditor ce(mComponent);
	ce.SetConfigurationDate(mName, mNewValue);
}


void THISCLASS::OnDateChanged(wxDateEvent& event) {
	wxCommandEvent ev;
	printf("Date changed\n");
}
