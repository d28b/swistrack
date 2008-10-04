#ifndef HEADER_ConfigurationParameterCheckBox
#define HEADER_ConfigurationParameterCheckBox

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class ConfigurationParameterCheckBox;

#include <wx/checkbox.h>
#include "ConfigurationParameter.h"

class ConfigurationParameterCheckBox: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterCheckBox(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterCheckBox();

	//! Handles the corresponding GUI event.
	void OnChecked(wxCommandEvent& event);

private:
	wxCheckBox *mCheckBox;		//!< The corresponding GUI widget.
	bool mValueDefault;			//!< The default value.
	bool mNewValue;				//!< The new value.

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
