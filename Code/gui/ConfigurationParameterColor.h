#ifndef HEADER_ConfigurationParameterColor
#define HEADER_ConfigurationParameterColor

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class ConfigurationParameterColor;

#include "ConfigurationParameter.h"

class ConfigurationParameterColor: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterColor(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterColor();

	//! Handles the corresponding GUI event.
	void OnTextEnter(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnKillFocus(wxFocusEvent& event);
	//! Handles the corresponding GUI event.
	void OnButtonClicked(wxCommandEvent& event);

private:
	wxTextCtrl *mTextCtrl;		//!< The corresponding GUI widget.
	wxButton *mButton;			//!< The corresponding GUI widget.
	wxColor mValueDefault;			//!< The default value.
	wxColor mNewValue;				//!< The new value.

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
