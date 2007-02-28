#ifndef HEADER_ConfigurationParameterString
#define HEADER_ConfigurationParameterString

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class ConfigurationParameterString;

#include "ConfigurationParameter.h"

class ConfigurationParameterString: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterString(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterString();

	// ConfigurationParameter methods
	virtual void OnInitialize(ConfigurationXML *config, ErrorList *errorlist);
	virtual void OnUpdate();

	//! Sets a new value of the component parameter and commits these changes.
	void SetValue(const wxString &value);

	//! Handles the corresponding GUI event.
	void OnItemSelected(wxCommandEvent& event);

private:
	wxComboBox *mComboBox;		//!< The corresponding GUI widget.
	wxString mValueDefault;		//!< The default value.

	DECLARE_EVENT_TABLE()
};

#endif
