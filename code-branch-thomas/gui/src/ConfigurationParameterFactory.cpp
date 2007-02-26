#include "ConfigurationParameterFactory.h"
#define THISCLASS ConfigurationParameterFactory

#include "ConfigurationParameterInteger.h"

ConfigurationParameter *THISCLASS::Create(const wxString &type, wxWindow* parent) {
	if (type=="integer") {return new ConfigurationParameterInteger(parent);}

	return 0;
}
