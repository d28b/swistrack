#ifndef HEADER_ConfigurationParameterDate
#define HEADER_ConfigurationParameterDate

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class ConfigurationParameterDate;

#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/spinctrl.h>
#include <wx/combobox.h>
#include "ConfigurationParameter.h"

class ConfigurationParameterDate: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterDate(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterDate();

private:
#ifdef wxUSE_DATEPICKCTRL
	wxDatePickerCtrl *mDateCtrl;		//!< The corresponding GUI widget.
#endif
	wxSpinCtrl *mHourCtrl;
	wxSpinCtrl *mMinuteCtrl;
	wxComboBox *mAmPmCtrl;
	wxDateTime mValueDefault;		//!< The default value.
	wxDateTime mNewValue;		//!< The new value.

	// ConfigurationParameter methods
	virtual void OnInitialize(ConfigurationXML *config, ErrorList *errorlist);
	virtual void OnUpdate(wxWindow *updateprotection);
	virtual void OnSetNewValue();

	//! Handles the corresponding GUI event.
	void OnDateChanged(wxDateEvent& event);
	void OnHourMinuteChanged(wxSpinEvent& event);
	void OnAmPmChanged(wxCommandEvent& event);


	// Validates the new value.
	bool ValidateNewValue();
	// Returns true if the new value is the same as the currently set value.
	bool CompareNewValue();
	void UpdateDate();

	DECLARE_EVENT_TABLE()
};

#endif
