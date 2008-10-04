#include "CommunicationMessage.h"
#include "ConfigurationConversion.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#define THISCLASS CommunicationMessage


THISCLASS::CommunicationMessage(const wxString &cmd, CommunicationMessage *inreplyto):
		mPopIndex(0), mParameters(), mCommand(cmd), mInReplyTo(inreplyto){

}

bool THISCLASS::GetBool(unsigned int i, bool defvalue) {
	if (mParameters.size() <= i) {
		return defvalue;
	}
	return ConfigurationConversion::Bool(mParameters[i], defvalue);
}

int THISCLASS::GetInt(unsigned int i, int defvalue) {
	if (mParameters.size() <= i) {
		return defvalue;
	}
	return ConfigurationConversion::Int(mParameters[i], defvalue);
}

double THISCLASS::GetDouble(unsigned int i, double defvalue) {
	if (mParameters.size() <= i) {
		return defvalue;
	}
	return ConfigurationConversion::Double(mParameters[i], defvalue);
}

wxString THISCLASS::GetString(unsigned int i, const wxString &defvalue) {
	if (mParameters.size() <= i) {
		return defvalue;
	}
	return mParameters[i];
}

bool THISCLASS::AddBool(bool value) {
	mParameters.push_back(ConfigurationConversion::Bool(value));
	return true;
}

bool THISCLASS::AddInt(int value) {
	mParameters.push_back(ConfigurationConversion::Int(value));
	return true;
}

bool THISCLASS::AddDouble(double value) {
	mParameters.push_back(ConfigurationConversion::Double(value));
	return true;
}

bool THISCLASS::AddString(const wxString &value) {
	mParameters.push_back(value);
	return true;
}

bool THISCLASS::AddParsedArgument(const wxString &value) {
	if (mCommand.Len() == 0) {
		mCommand = value;
	} else {
		mParameters.push_back(value);
	}
	return true;
}
