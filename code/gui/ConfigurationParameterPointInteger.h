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

	//! Handles the corresponding GUI event.
	void OnTextUpdated(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnTextEnter(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnSpin(wxSpinEvent& event);
	//! Handles the corresponding GUI event.
	void OnKillFocus(wxFocusEvent& event);

private:
	wxSpinCtrl *mSpinCtrlX;		//!< The corresponding GUI widget.
	wxSpinCtrl *mSpinCtrlY;		//!< The corresponding GUI widget.
	int mValueMinX;				//!< The minimum value allowed.
	int mValueMinY;				//!< The minimum value allowed.
	int mValueMaxX;				//!< The maximum value allowed.
	int mValueMaxY;				//!< The maximum value allowed.
	int mValueDefaultX;			//!< The default value.
	int mValueDefaultY;			//!< The default value.
	int mNewValueX;				//!< The new value for X.
	int mNewValueY;				//!< The new value for Y.

	//! Textbox IDs
	enum eID {
		cID_X,
		cID_Y,
	};

	// ConfigurationParameter methods
	virtual void OnInitialize(ConfigurationXML *config, ErrorList *errorlist);
	virtual void OnUpdate(wxWindow *updateprotection);
	virtual void OnSetNewValue();

	// Validates the new value.
	bool ValidateNewValueX();
	// Validates the new value.
	bool ValidateNewValueY();
	// Returns true if the new value is the same as the currently set value.
	bool CompareNewValue();

	DECLARE_EVENT_TABLE()
};

#endif
