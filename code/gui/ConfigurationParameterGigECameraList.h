#ifndef HEADER_ConfigurationParameterGigECameraList
#define HEADER_ConfigurationParameterGigECameraList

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class ConfigurationParameterGigECameraList;

#include "ConfigurationParameterDropdownList.h"

class ConfigurationParameterGigECameraList: public ConfigurationParameterDropdownList {

public:
	//! Constructor.
	ConfigurationParameterGigECameraList(wxWindow* parent): ConfigurationParameterDropdownList(parent) {}
	//! Destructor.
	~ConfigurationParameterGigECameraList() {}

private:
	// ConfigurationParameterDropdownList methods
	virtual void FillList(ConfigurationXML *config, ErrorList *errorlist);

};

#endif
