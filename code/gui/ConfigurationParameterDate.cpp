#include "ConfigurationParameterDate.h"
#define THISCLASS ConfigurationParameterDate

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
       EVT_DATE_CHANGED(wxID_ANY, THISCLASS::OnDateChanged)
       EVT_COMBOBOX(wxID_ANY, THISCLASS::OnAmPmChanged)
       EVT_SPINCTRL(wxID_ANY, THISCLASS::OnHourMinuteChanged)
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
	mDateCtrl = new wxDatePickerCtrl(this, wxID_ANY, mValueDefault, wxDefaultPosition, wxSize(scParameterWidth - 50, -1), wxTE_PROCESS_ENTER);
	//	mDateCtrl->Connect(wxID_ANY, wxEVT_DATE_CHANGED, wxDateEventHandler(THISCLASS::OnDateChanged), 0, this);

	mHourCtrl = new wxSpinCtrl(this, wxID_ANY, wxT("10"), wxDefaultPosition, wxSize(50, -1));
	mHourCtrl->SetRange(1, 12);

	mMinuteCtrl = new wxSpinCtrl(this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize(50, -1));
	mMinuteCtrl->SetRange(0, 59);

	mAmPmCtrl = new wxComboBox(this, wxID_ANY, wxT("AM"), wxDefaultPosition, wxSize(50, -1));
	mAmPmCtrl->Insert(wxT("AM"), 0);
	mAmPmCtrl->Insert(wxT("PM"), 0);

	// Layout the controls
	wxBoxSizer *hs = new wxBoxSizer(wxHORIZONTAL);
	hs->Add(mDateCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mHourCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mMinuteCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mAmPmCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);

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

	if (value.GetHour() == 0) {
	  mHourCtrl->SetValue(12);
	  mAmPmCtrl->SetValue(wxT("AM"));
	} else if (value.GetHour() == 12) {
	  mHourCtrl->SetValue(12);
	  mAmPmCtrl->SetValue(wxT("PM"));
	} else {
	  if (value.GetHour() > 12) {
	    mHourCtrl->SetValue(value.GetHour() % 12);
	    mAmPmCtrl->SetValue(wxT("PM"));
	  } else {
	    mHourCtrl->SetValue(value.GetHour());
	    mAmPmCtrl->SetValue(wxT("AM"));
	  }
	}
	mMinuteCtrl->SetValue(value.GetMinute());
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

void THISCLASS::UpdateDate() {
	mNewValue = mDateCtrl->GetValue();
	int hour = mHourCtrl->GetValue();
	if (mAmPmCtrl->GetValue() == wxT("PM")) {
	  if (hour != 12) {
	    hour += 12;
	  }
	} else {
	  if (hour == 12) {
	    hour = 0;
	  }
	}
	mNewValue.SetHour(hour);
	mNewValue.SetMinute(mMinuteCtrl->GetValue());
	SetNewValue(mDateCtrl);

	
}
void THISCLASS::OnDateChanged(wxDateEvent& event) {
        UpdateDate();
}

void THISCLASS::OnHourMinuteChanged(wxSpinEvent& event) {
        UpdateDate();
}
void THISCLASS::OnAmPmChanged(wxCommandEvent& event) {
        UpdateDate();
}
