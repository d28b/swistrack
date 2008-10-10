#ifndef HEADER_ConfigurationParameterDate
#define HEADER_ConfigurationParameterDate

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#endif

class ConfigurationParameterDate;

#include "ConfigurationParameter.h"

class ConfigurationParameterDate: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterDate(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterDate();


	//! Handles the corresponding GUI event.
	void OnDateChanged(wxDateEvent& event);


private:
	wxDatePickerCtrl *mDateCtrl;		//!< The corresponding GUI widget.
	wxDateTime mValueDefault;		//!< The default value.
	wxDateTime mNewValue;		//!< The new value.

	// ConfigurationParameter methods
	virtual void OnInitialize(ConfigurationXML *config, ErrorList *errorlist);
	virtual void OnUpdate(wxWindow *updateprotection);
	virtual void OnSetNewValue();

	// Validates the new value.
	bool ValidateNewValue();
	// Returns true if the new value is the same as the currently set value.
	bool CompareNewValue();

	DECLARE_EVENT_TABLE()
};

#endif
