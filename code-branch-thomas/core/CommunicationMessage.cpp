#include "CommunicationMessage.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#define THISCLASS CommunicationMessage


THISCLASS::CommunicationMessage(const std::string &cmd, CommunicationMessage *inreplyto):
		mParameters(), mCommand(cmd), mInReplyTo(inreplyto) {

}

bool THISCLASS::GetBool(bool defvalue) {
	if (mParameters.size()<1) {return defvalue;}
	std::string str=mParameters.front();
	mParameters.pop_front();

	std::string strlc(str);
	std::transform(strlc.begin(), strlc.end(), strlc.begin(), std::tolower);
	if (strlc=="true") {return true;}
	if (strlc=="false") {return false;}

	std::istringstream istr(str);
	bool val=0;
	istr >> val;
	return val;
}

int THISCLASS::GetInt(int defvalue) {
	if (mParameters.size()<1) {return defvalue;}
	std::string str=mParameters.front();
	mParameters.pop_front();

	std::istringstream istr(str);
	int val=0;
	istr >> val;
	return val;
}

double THISCLASS::GetDouble(double defvalue) {
	if (mParameters.size()<1) {return defvalue;}
	std::string str=mParameters.front();
	mParameters.pop_front();

	std::istringstream istr(str);
	double val=0;
	istr >> val;
	return val;
}

std::string THISCLASS::GetString(const std::string &defvalue) {
	if (mParameters.size()<1) {return defvalue;}
	std::string str=mParameters.front();
	mParameters.pop_front();

	return str;
}

bool THISCLASS::AddBool(bool value) {
	if (value) {
		mParameters.push_back("true");
	} else {
		mParameters.push_back("false");
	}
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
