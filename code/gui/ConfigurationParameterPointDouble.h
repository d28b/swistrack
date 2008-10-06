#ifndef HEADER_ConfigurationParameterPointDouble
#define HEADER_ConfigurationParameterPointDouble

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class ConfigurationParameterPointDouble;

#include <wx/textctrl.h>
#include "ConfigurationParameter.h"

class ConfigurationParameterPointDouble: public ConfigurationParameter {

public:
	//! Constructor.
	ConfigurationParameterPointDouble(wxWindow* parent);
	//! Destructor.
	~ConfigurationParameterPointDouble();

	//! Handles the corresponding GUI event.
	void OnTextUpdated(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnTextEnter(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnKillFocus(wxFocusEvent& event);

private:
	wxTextCtrl *mTextCtrlX;		//!< The corresponding GUI widget.
	wxTextCtrl *mTextCtrlY;		//!< The corresponding GUI widget.
	double mValueMinX;			//!< The minimum value allowed for X.
	double mValueMinY;			//!< The minimum value allowed for Y.
	double mValueMaxX;			//!< The maximum value allowed for X.
	double mValueMaxY;			//!< The maximum value allowed for Y.
	double mValueDefaultX;		//!< The default value for X.
	double mValueDefaultY;		//!< The default value for Y.
	double mNewValueX;			//!< The new value for X.
	double mNewValueY;			//!< The new value for Y.

	//! Textbox IDs
	enum eID {
		cID_X,
		cID_Y,
	};

	// ConfigurationParameter methods
	virtual void OnInitialize(ConfigurationXML *config, ErrorList *errorlist);
	virtual void OnUpdate(wxWindow *updateprotection);
	virtual void OnSetNewValue();

	// Validates the new value for X.
	bool ValidateNewValueX();
	// Validates the new value for Y.
	bool ValidateNewValueY();
	// Returns true if the new value is the same as the currently set value.
	bool CompareNewValue();

	DECLARE_EVENT_TABLE()
};

#endif
