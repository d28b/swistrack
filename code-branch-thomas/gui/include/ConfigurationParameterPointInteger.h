#ifndef HEADER_ConfigurationParameterPointInteger
#define HEADER_ConfigurationParameterPointInteger

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class ConfigurationParameterPointInteger;

#include <wx/spinctrl.h>
#include "ConfigurationParameter.h"

class ConfigurationParameterPointInteger: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterPointInteger(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterPointInteger();

	// ConfigurationParameter methods
	virtual void OnInitialize(ConfigurationXML *config, ErrorList *errorlist);
	virtual void OnUpdate();

	//! Sets a new value of the component parameter and commits these changes.
	void SetValue(int valuex, int valuey);

	//! Handles the corresponding GUI event.
	void OnTextUpdated(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnTextEnter(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnSpin(wxSpinEvent& event);

private:
	wxSpinCtrl *mSpinCtrlX;		//!< The corresponding GUI widget.
	wxSpinCtrl *mSpinCtrlY;		//!< The corresponding GUI widget.
	int mValueMinX;				//!< The minimum value allowed.
	int mValueMaxX;				//!< The maximum value allowed.
	int mValueDefaultX;			//!< The default value.
	int mValueMinY;				//!< The minimum value allowed.
	int mValueMaxY;				//!< The maximum value allowed.
	int mValueDefaultY;			//!< The default value.

	DECLARE_EVENT_TABLE()
};

#endif
