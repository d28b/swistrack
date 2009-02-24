#ifndef HEADER_ConfigurationParameterInputFile
#define HEADER_ConfigurationParameterInputFile

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class ConfigurationParameterInputFile;

#include "ConfigurationParameter.h"

class ConfigurationParameterInputFile: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterInputFile(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterInputFile();

	//! Handles the corresponding GUI event.
	void OnTextEnter(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnKillFocus(wxFocusEvent& event);
	//! Handles the corresponding GUI event.
	void OnButtonClicked(wxCommandEvent& event);

private:
	wxTextCtrl *mTextCtrl;		//!< The corresponding GUI widget.
	wxButton *mButton;			//!< The corresponding GUI widget.
	wxString mValueDefault;		//!< The default value.
	wxString mFileFilter;		//!< The file filter shown in the file selection dialog box.
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
