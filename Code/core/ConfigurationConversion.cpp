#include "ConfigurationConversion.h"
#define THISCLASS ConfigurationConversion
#include <iostream>

bool THISCLASS::Bool(const wxString & str, bool defaultValue) {
	if (str.Len() == 0) return defaultValue;

	wxString strlc = str.Lower();
	if (strlc == wxT("true")) return true;
	if (strlc == wxT("false")) return false;

	long value = (defaultValue ? 1 : 0);
	str.ToLong(&value);
	return value != 0;
}

int THISCLASS::Int(const wxString & str, int defaultValue) {
	if (str.Len() == 0) return defaultValue;

	long value = (long)defaultValue;
	str.ToLong(&value);
	return value;
}

double THISCLASS::Double(const wxString & str, double defaultValue) {
	if (str.Len() == 0) return defaultValue;

	str.ToDouble(&defaultValue);
	return defaultValue;
}

wxString THISCLASS::Bool(bool value) {
	return value ? wxT("true") : wxT("false");
}

wxString THISCLASS::Int(int value) {
	return wxString::Format(wxT("%d"), value);
}

wxString THISCLASS::Double(double value) {
	return wxString::Format(wxT("%f"), value);
}

wxString THISCLASS::Date(wxDateTime value) {
	return value.Format();
}

wxDateTime THISCLASS::Date(const wxString & str, const wxDateTime & defaultValue) {
	if (str.Len() == 0) return defaultValue;

	wxDateTime retval = wxDateTime();
	retval.ParseFormat(str);
	return retval;
}

wxColor THISCLASS::Color(const wxString & str, const wxColor defValue) {
	if (str.Len() == 0) return defValue;

	return wxColor(str);
}

cv::Scalar THISCLASS::Color(const wxString & str, const cv::Scalar defValue) {
	if (str.Len() == 0) return defValue;

	wxColor color(str);
	return cv::Scalar(color.Blue(), color.Green(), color.Red());
}

wxString THISCLASS::Color(const wxColor value) {
	std::cout << "Color: " << value.GetAsString(wxC2S_HTML_SYNTAX).ToAscii() << "\n";
	return value.GetAsString(wxC2S_HTML_SYNTAX);
}
