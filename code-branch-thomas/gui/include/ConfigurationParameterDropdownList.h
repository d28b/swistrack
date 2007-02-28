#ifndef HEADER_ConfigurationParameterDropdownList
#define HEADER_ConfigurationParameterDropdownList

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class ConfigurationParameterDropdownList;

#include "ConfigurationParameter.h"

class ConfigurationParameterDropdownList: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterDropdownList(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterDropdownList();

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
