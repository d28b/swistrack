#include "ConfigurationParameterFactory.h"
#define THISCLASS ConfigurationParameterFactory

#include "ConfigurationParameterInteger.h"
#include "ConfigurationParameterDouble.h"
#include "ConfigurationParameterString.h"
#include "ConfigurationParameterDropdownList.h"
#include "ConfigurationParameterFile.h"
#include "ConfigurationParameterImage.h"
#include "ConfigurationParameterPointInteger.h"

ConfigurationParameter *THISCLASS::Create(const wxString &type, wxWindow* parent) {
	type.MakeLower();

	// Basic data types
	if (type=="integer") {return new ConfigurationParameterInteger(parent);}
	if (type=="double") {return new ConfigurationParameterDouble(parent);}
	if (type=="string") {return new ConfigurationParameterString(parent);}
	if (type=="dropdownlist") {return new ConfigurationParameterDropdownList(parent);}

	// Files
	if (type=="file") {return new ConfigurationParameterFile(parent);}
	if (type=="image") {return new ConfigurationParameterImage(parent);}

	// Points
	if (type=="pointinteger") {return new ConfigurationParameterPointInteger(parent);}

	return 0;
}
