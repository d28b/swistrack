#ifndef HEADER_ConfigurationParameterImage
#define HEADER_ConfigurationParameterImage

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class ConfigurationParameterImage;

#include "ConfigurationParameter.h"

class ConfigurationParameterImage: public ConfigurationParameter {

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
	wxString mValueDefault;		//!< The default value.

	DECLARE_EVENT_TABLE()
};

#endif
