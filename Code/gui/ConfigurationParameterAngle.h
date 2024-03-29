#ifndef HEADER_ConfigurationParameterAngle
#define HEADER_ConfigurationParameterAngle

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class ConfigurationParameterAngle;

#include "ConfigurationParameter.h"

class ConfigurationParameterAngle: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterAngle(wxWindow * parent);
	//! Destructor.
	~ConfigurationParameterAngle();

	//! Handles the corresponding GUI event.
	void OnTextUpdated(wxCommandEvent & event);
	//! Handles the corresponding GUI event.
	void OnTextEnter(wxCommandEvent & event);
	//! Handles the corresponding GUI event.
	void OnKillFocus(wxFocusEvent & event);
	//! Handles the corresponding GUI event.
	void OnScrollChanged(wxScrollEvent & event);

protected:
	wxTextCtrl * mTextCtrl;		//!< The corresponding GUI widget.
	wxSlider * mSlider;			//!< The corresponding GUI widget.
	double mValueMin;			//!< The minimum value allowed.
	double mValueMax;			//!< The maximum value allowed.
	double mValueDefault;		//!< The default value.
	double mSliderStep;			//!< The increment of the slider.
	double mNewValue;			//!< The new value.

	// ConfigurationParameter methods
	virtual void OnInitialize(ConfigurationXML * config, ErrorList * errorlist);
	virtual void OnUpdate(wxWindow * updateprotection);
	virtual void OnSetNewValue();

	// Validates the new value.
	bool ValidateNewValue();
	// Returns true if the new value is the same as the currently set value.
	bool CompareNewValue();

	DECLARE_EVENT_TABLE()
};

#endif
