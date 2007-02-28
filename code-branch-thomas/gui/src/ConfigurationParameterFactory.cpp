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
	wxString typelc=type.Lower();

	// Basic data types
	if (typelc=="integer") {return new ConfigurationParameterInteger(parent);}
	if (typelc=="double") {return new ConfigurationParameterDouble(parent);}
	if (typelc=="string") {return new ConfigurationParameterString(parent);}
	if (typelc=="dropdownlist") {return new ConfigurationParameterDropdownList(parent);}

	// Files
	if (typelc=="file") {return new ConfigurationParameterFile(parent);}
	if (typelc=="image") {return new ConfigurationParameterImage(parent);}

	// Points
	if (typelc=="pointinteger") {return new ConfigurationParameterPointInteger(parent);}

	return 0;
}
