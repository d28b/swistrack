#ifndef HEADER_ConfigurationParameterIntegerSlider
#define HEADER_ConfigurationParameterIntegerSlider

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class ConfigurationParameterIntegerSlider;

#include "ConfigurationParameter.h"

class ConfigurationParameterIntegerSlider: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterIntegerSlider(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterIntegerSlider();

	// ConfigurationParameter methods
	virtual void OnInitialize(ConfigurationXML *config, ErrorList *errorlist);
	virtual void OnUpdate();

	//! Handles the corresponding GUI event.
	void OnTextUpdated(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnTextEnter(wxCommandEvent& event);

private:
	
	int mValueMin;				//!< The minimum value allowed.
	int mValueMax;				//!< The maximum value allowed.
	int mValueDefault;			//!< The default value.

	DECLARE_EVENT_TABLE()
};

#endif
