#include "ConfigurationParameterFactory.h"
#define THISCLASS ConfigurationParameterFactory

#include "ConfigurationParameterInteger.h"
#include "ConfigurationParameterDouble.h"
#include "ConfigurationParameterString.h"
#include "ConfigurationParameterCheckBox.h"
#include "ConfigurationParameterButton.h"
#include "ConfigurationParameterDropdownList.h"
#include "ConfigurationParameterColor.h"
#include "ConfigurationParameterInputFile.h"
#include "ConfigurationParameterOutputFile.h"
#include "ConfigurationParameterDate.h"
#include "ConfigurationParameterPointInteger.h"
#include "ConfigurationParameterPointDouble.h"
#include "ConfigurationParameterAngle.h"
#include "ConfigurationParameterGigECameraList.h"

ConfigurationParameter *THISCLASS::Create(const wxString &type, wxWindow* parent) {
	wxString typelc = type.Lower();

	// Basic data types
	if (typelc == wxT("integer")) {
		return new ConfigurationParameterInteger(parent);
	}
	if (typelc == wxT("double")) {
		return new ConfigurationParameterDouble(parent);
	}
	if (typelc == wxT("string")) {
		return new ConfigurationParameterString(parent);
	}
	if (typelc == wxT("checkbox")) {
		return new ConfigurationParameterCheckBox(parent);
	}
	if (typelc == wxT("button")) {
		return new ConfigurationParameterButton(parent);
	}
	if (typelc == wxT("dropdownlist")) {
		return new ConfigurationParameterDropdownList(parent);
	}
	if (typelc == wxT("color")) {
		return new ConfigurationParameterColor(parent);
	}

	// Files
	if (typelc == wxT("inputfile")) {
		return new ConfigurationParameterInputFile(parent);
	}
	if (typelc == wxT("outputfile")) {
		return new ConfigurationParameterOutputFile(parent);
	}
	if (typelc == wxT("date")) {
		return new ConfigurationParameterDate(parent);
	}

	// Points
	if (typelc == wxT("pointinteger")) {
		return new ConfigurationParameterPointInteger(parent);
	}
	if (typelc == wxT("pointdouble")) {
		return new ConfigurationParameterPointDouble(parent);
	}

	// Special types
	if (typelc == wxT("angle")) {
		return new ConfigurationParameterAngle(parent);
	}
	if (typelc == wxT("gigecameralist")) {
		return new ConfigurationParameterGigECameraList(parent);
	}

	return 0;
}
