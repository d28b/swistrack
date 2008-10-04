#ifndef HEADER_ConfigurationParameterInteger
#define HEADER_ConfigurationParameterInteger

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class ConfigurationParameterInteger;

#include <wx/spinctrl.h>
#include "ConfigurationParameter.h"

class ConfigurationParameterInteger: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterInteger(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterInteger();

	//! Handles the corresponding GUI event.
	void OnTextUpdated(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnTextEnter(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnSpin(wxSpinEvent& event);
	//! Handles the corresponding GUI event.
	void OnKillFocus(wxFocusEvent& event);
	//! Handles the corresponding GUI event.
	void OnScrollChanged(wxScrollEvent& event);

protected:
	wxSpinCtrl *mSpinCtrl;		//!< The corresponding GUI widget.
	wxSlider *mSlider;			//!< The corresponding GUI widget.
	int mValueMin;				//!< The minimum value allowed.
	int mValueMax;				//!< The maximum value allowed.
	int mValueDefault;			//!< The default value.
	int mNewValue;				//!< The new value.

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
