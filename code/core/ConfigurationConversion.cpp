#include "ConfigurationConversion.h"
#define THISCLASS ConfigurationConversion

bool THISCLASS::Bool(const wxString &str, bool defvalue) {
	if (str.Len() == 0) {
		return defvalue;
	}

	wxString strlc = str.Lower();
	if (strlc == wxT("true")) {
		return true;
	}
	if (strlc == wxT("false")) {
		return false;
	}
	long value = (defvalue ? 1 : 0);
	str.ToLong(&value);
	return (value != 0);
}

int THISCLASS::Int(const wxString &str, int defvalue) {
	if (str.Len() == 0) {
		return defvalue;
	}

	long value = (long)defvalue;
	str.ToLong(&value);
	return value;
}

double THISCLASS::Double(const wxString &str, double defvalue) {
	if (str.Len() == 0) {
		return defvalue;
	}

	str.ToDouble(&defvalue);
	return defvalue;
}

wxString THISCLASS::Bool(bool value) {
	if (value) {
		return wxT("true");
	} else {
		return wxT("false");
	}
}

wxString THISCLASS::Int(int value) {
	return wxString::Format(wxT("%d"), value);
}

wxString THISCLASS::Double(double value) {
	return wxString::Format(wxT("%f"), value);
}

