#ifndef HEADER_ConfigurationParameterInteger
#define HEADER_ConfigurationParameterInteger

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class ConfigurationParameterInteger;

#include "ConfigurationParameter.h"

class ConfigurationParameterInteger: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterInteger(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterInteger();

	// ConfigurationParameter methods
	virtual void OnInitialize(ConfigurationXML *config);
	virtual void OnUpdate();

	//! Handles the corresponding GUI event.
	void OnTextUpdated(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnTextEnter(wxCommandEvent& event);

private:
	wxTextCtrl *mTextCtrl;		//!< The corresponding GUI widget.
	int mValueMin;				//!< The minimum value allowed.
	int mValueMax;				//!< The maximum value allowed.
	int mValueDefault;			//!< The default value.

	DECLARE_EVENT_TABLE()
};

#endif
