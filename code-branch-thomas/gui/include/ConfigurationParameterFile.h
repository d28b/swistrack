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

	// ConfigurationParameter methods
	virtual void OnInitialize(ConfigurationXML *config, ErrorList *errorlist);
	virtual void OnUpdate();

	//! Sets a new value of the component parameter and commits these changes.
	void SetValue(const wxString &value);

	//! Handles the corresponding GUI event.
	void OnTextUpdated(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnTextEnter(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnButtonClicked(wxCommandEvent& event);

private:
	wxTextCtrl *mTextCtrl;		//!< The corresponding GUI widget.
	wxButton *mButton;			//!< The corresponding GUI widget.
	wxString mValueDefault;		//!< The default value.
	wxString mFileFilter;		//!< The file filter shown in the file selection dialog box.

	DECLARE_EVENT_TABLE()
};

#endif
