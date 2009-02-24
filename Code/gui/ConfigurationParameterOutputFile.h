#ifndef HEADER_ConfigurationParameterOutputFile
#define HEADER_ConfigurationParameterOutputFile

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class ConfigurationParameterOutputFile;

#include "ConfigurationParameter.h"

class ConfigurationParameterOutputFile: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterOutputFile(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterOutputFile();

	//! Handles the corresponding GUI event.
	void OnTextEnter(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnKillFocus(wxFocusEvent& event);

private:
	wxTextCtrl *mTextCtrl;		//!< The corresponding GUI widget.
	wxString mValueDefault;		//!< The default value.
	wxString mNewValue;			//!< The new value.

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
