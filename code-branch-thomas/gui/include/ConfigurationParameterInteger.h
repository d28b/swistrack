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
	ConfigurationParameterInteger(wxWindow* parent, wxXmlNode *node, SwisTrack *st);
	//! Destructor.
	~ConfigurationParameterInteger();

	//! Handles the corresponding GUI event.
	void OnTextUpdated(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnTextEnter(wxCommandEvent& event);

private:
	wxTextCtrl *mTextCtrl;		//!< The corresponding GUI widget.
	int mMin;					//!< The minimum value allowed.
	int mMax;					//!< The maximum value allowed.

	DECLARE_EVENT_TABLE()
};

#endif
