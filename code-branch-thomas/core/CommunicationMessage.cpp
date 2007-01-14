#include "CommunicationMessage.h"
#include <sstream>
#define THISCLASS CommunicationMessage


THISCLASS::CommunicationMessage(const std::string &cmd, CommunicationMessage *inreplyto):
		mParameters(), mCommand(cmd), mInReplyTo(inreplyto) {

}

bool THISCLASS::GetBool(bool defvalue) {
	if (mParameters.size()<1) {return defvalue;}
	std::string t=mParameters.front();

	std::istringstream istr(t);
	bool val=0;
	istr >> val;
	return val;
}

int THISCLASS::GetInt(int defvalue) {
	if (mParameters.size()<1) {return defvalue;}
	std::string t=mParameters.front();

	std::istringstream istr(t);
	int val=0;
	istr >> val;
	return val;
}

double THISCLASS::GetDouble(double defvalue) {
	if (mParameters.size()<1) {return defvalue;}
	std::string t=mParameters.front();

	std::istringstream istr(t);
	double val=0;
	istr >> val;
	return val;
}

std::string THISCLASS::GetString(const std::string &defvalue) {
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

bool THISCLASS::AddInt(int value) {
	std::ostringstream oss;
	oss << value;
	mParameters.push_back(oss.str());
	return true;
}

bool THISCLASS::AddDouble(double value) {
	std::ostringstream oss;
	oss << value;
	mParameters.push_back(oss.str());
	return true;
}

bool THISCLASS::AddString(const std::string &value) {
	mParameters.push_back(value);
	return true;
}

bool THISCLASS::AddParsedArgument(const std::string &value) {
	if (mCommand=="") {
		mCommand=value;
	} else {
		mParameters.push_back(value);
	}
	return true;
}
