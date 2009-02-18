#ifndef HEADER_ConfigurationParameterFile
#define HEADER_ConfigurationParameterFile

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class ConfigurationParameterFile;

#include "ConfigurationParameter.h"

class ConfigurationParameterFile: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterFile(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterFile();

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
	bool mIsInput;		//!< Specifies whether the file is read or written by the component.
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
