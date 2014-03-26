#ifndef HEADER_ConfigurationParameterButton
#define HEADER_ConfigurationParameterButton

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class ConfigurationParameterButton;

#include <wx/button.h>
#include "ConfigurationParameter.h"

class ConfigurationParameterButton: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterButton(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterButton();

	//! Handles the corresponding GUI event.
	void OnClick(wxCommandEvent& event);

private:
	wxButton *mButton;			//!< The corresponding GUI widget.
	wxString mValue;			//!< The value to set when the user clicks the button.

	// ConfigurationParameter methods
	virtual void OnInitialize(ConfigurationXML *config, ErrorList *errorlist);
	virtual void OnUpdate(wxWindow *updateprotection);
	virtual void OnSetNewValue();

	DECLARE_EVENT_TABLE()
};

#endif
