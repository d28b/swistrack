#ifndef HEADER_ConfigurationParameterGBitCameraList
#define HEADER_ConfigurationParameterGBitCameraList

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class ConfigurationParameterGBitCameraList;

#include "ConfigurationParameterDropdownList.h"

class ConfigurationParameterGBitCameraList: public ConfigurationParameterDropdownList {

public:
	//! Constructor.
	ConfigurationParameterGBitCameraList(wxWindow* parent): ConfigurationParameterDropdownList(parent) {}
	//! Destructor.
	~ConfigurationParameterGBitCameraList() {}

private:
	// ConfigurationParameterDropdownList methods
	virtual void FillList(ConfigurationXML *config, ErrorList *errorlist);

};

#endif
