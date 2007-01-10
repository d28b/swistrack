#include "CommunicationMessage.h"
#include <sstream>
#define THISCLASS CommunicationMessage


THISCLASS::CommunicationMessage(const std::string &cmd):
		mParameters(), mCommand(cmd) {

}

bool THISCLASS::GetBool(bool defvalue) {
	if (mParameters.size()<1) {return defvalue;}
	std::string t=mParameters.front();

	std::istringstream istr(t);
	bool val=0;
	istr >> val;
	return val;
}

int THISCLASS::GetConfigurationInt(int defvalue) {
	if (mParameters.size()<1) {return defvalue;}
	std::string t=mParameters.front();

	std::istringstream istr(t);
	int val=0;
	istr >> val;
	return val;
}

double THISCLASS::GetConfigurationDouble(double defvalue) {
	if (mParameters.size()<1) {return defvalue;}
	std::string t=mParameters.front();

	std::istringstream istr(t);
	double val=0;
	istr >> val;
	return val;
}

std::string THISCLASS::GetConfigurationString(const std::string &defvalue) {
	if (mParameters.size()<1) {return defvalue;}
	std::string t=mParameters.front();

	return t;
}

bool THISCLASS::AddBool(bool value) {
	std::ostringstream oss;
	oss << value;
	mParameters.push_back(oss.str());
	return true;
}

bool THISCLASS::SetConfigurationInt(int value) {
	std::ostringstream oss;
	oss << value;
	mParameters.push_back(oss.str());
	return true;
}

bool THISCLASS::SetConfigurationDouble(double value) {
	std::ostringstream oss;
	oss << value;
	mParameters.push_back(oss.str());
	return true;
}

bool THISCLASS::SetConfigurationString(const std::string &value) {
	mParameters.push_back(value);
	return true;
}
