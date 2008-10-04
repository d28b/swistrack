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

	//! Handles the corresponding GUI event.
	void OnItemSelected(wxCommandEvent& event);

protected:
	wxComboBox *mComboBox;		//!< The corresponding GUI widget.
	wxString mValueDefault;		//!< The default value.
	wxString mNewValue;			//!< The new value.

	// ConfigurationParameter methods
	virtual void OnInitialize(ConfigurationXML *config, ErrorList *errorlist);
	virtual void OnUpdate(wxWindow *updateprotection);
	virtual void OnSetNewValue();

	// Fills in the list. This method can be overwritten to create list with specific entries. It should use the AddItem() method to add items to the list.
	virtual void FillList(ConfigurationXML *config, ErrorList *errorlist);
	// Adds an item to the list.
	void AddItem(const wxString &key, const wxString &label);

	// Validates the new value.
	bool ValidateNewValue();
	// Returns true if the new value is the same as the currently set value.
	bool CompareNewValue();

	DECLARE_EVENT_TABLE()
};

#endif
