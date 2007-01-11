#include "Component.h"
#include <sstream>
#define THISCLASS Component

THISCLASS::Component(SwisTrackCore *stc, const std::string &name):
		mCore(stc), mName(name), mDisplayName(name), mCategory(0), mStatus(), mStatusHasError(false), mStatusHasWarning(false), mStarted(false) {

	//mConfigurationRoot=dynamic_cast<const xmlpp::Element*>(mCore->mConfigurationRoot->find(mName)).begin());
}

void THISCLASS::ClearStatus() {
	mStatus.clear();
	mStatusHasError=false;
	mStatusHasWarning=false;
}

void THISCLASS::AddError(const std::string &msg) {
	mStatus.push_back(StatusItem(StatusItem::sTypeError, msg));
	mStatusHasError=true;
}

void THISCLASS::AddWarning(const std::string &msg) {
	mStatus.push_back(StatusItem(StatusItem::sTypeWarning, msg));
	mStatusHasWarning=true;
}

void THISCLASS::AddInfo(const std::string &msg) {
	mStatus.push_back(StatusItem(StatusItem::sTypeInfo, msg));
}

void THISCLASS::AddDataStructureRead(DataStructure *ds) {
}

void THISCLASS::AddDataStructureWrite(DataStructure *ds) {
}

bool THISCLASS::HasDataStructureRead(DataStructure *ds) {
	return false;
}

bool THISCLASS::HasDataStructureWrite(DataStructure *ds) {
	return false;
}

bool THISCLASS::GetConfigurationBool(const std::string &path, bool defvalue) {
	xmlpp::Element *e=GetConfigurationElement(path);
	if (! e) {return defvalue;}
	xmlpp::TextNode *t=e->get_child_text();
	if (! t) {return defvalue;}

	std::istringstream istr(t->get_content());
	bool val=0;
	istr >> val;
	return val;
}

int THISCLASS::GetConfigurationInt(const std::string &path, int defvalue) {
	xmlpp::Element *e=GetConfigurationElement(path);
	if (! e) {return defvalue;}
	xmlpp::TextNode *t=e->get_child_text();
	if (! t) {return defvalue;}

	std::istringstream istr(t->get_content());
	int val=0;
	istr >> val;
	return val;
}

double THISCLASS::GetConfigurationDouble(const std::string &path, double defvalue) {
	xmlpp::Element *e=GetConfigurationElement(path);
	if (! e) {return defvalue;}
	xmlpp::TextNode *t=e->get_child_text();
	if (! t) {return defvalue;}

	std::istringstream istr(t->get_content());
	double val=0;
	istr >> val;
	return val;
}

std::string THISCLASS::GetConfigurationString(const std::string &path, const std::string &defvalue) {
	xmlpp::Element *e=GetConfigurationElement(path);
	if (! e) {return defvalue;}
	xmlpp::TextNode *t=e->get_child_text();
	if (! t) {return defvalue;}

	return t->get_content();
}

bool THISCLASS::SetConfigurationBool(const std::string &path, bool value) {
	xmlpp::Element *e=GetConfigurationElement(path);
	if (! e) {return false;}

	std::ostringstream oss;
	oss << value;
	e->set_child_text(oss.str());
	return true;
}

bool THISCLASS::SetConfigurationInt(const std::string &path, int value) {
	xmlpp::Element *e=GetConfigurationElement(path);
	if (! e) {return false;}

	std::ostringstream oss;
	oss << value;
	e->set_child_text(oss.str());
	return true;
}

bool THISCLASS::SetConfigurationDouble(const std::string &path, double value) {
	xmlpp::Element *e=GetConfigurationElement(path);
	if (! e) {return false;}

	std::ostringstream oss;
	oss << value;
	e->set_child_text(oss.str());
	return true;
}

bool THISCLASS::SetConfigurationString(const std::string &path, const std::string &value) {
	xmlpp::Element *e=GetConfigurationElement(path);
	if (! e) {return false;}

	e->set_child_text(value);
	return true;
}
