#ifndef HEADER_ConfigurationConversion
#define HEADER_ConfigurationConversion

class ConfigurationConversion;

#include <wx/xml/xml.h>
#include "SwisTrackCore.h"

//! Functions to convert configuration strings to other types and vice versa.
/*!
	The methods of this class are used by a number of other classes which convert configuration values to configuration strings and vice versa.
*/
class ConfigurationConversion {

public:
	//! Constructor.
	ConfigurationConversion() {}
	//! Destructor.
	~ConfigurationConversion() {}

	//! Converts a configuration value into a boolean value.
	static bool Bool(const wxString &str, bool defvalue);
	//! Converts a configuration value into an int value.
	static int Int(const wxString &str, int defvalue);
	//! Converts a configuration value into a double value.
	static double Double(const wxString &str, double defvalue);
	//! Converts a boolean value into a configuration value.
	static wxString Bool(bool value);
	//! Converts a boolean value into a configuration value.
	static wxString Int(int value);
	//! Converts a boolean value into a configuration value.
	static wxString Double(double value);

};

#endif
