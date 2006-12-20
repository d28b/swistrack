#include "Component.h"
#define THISCLASS Component

THISCLASS::Component(SwisTrackCore *stc, const std::string &name, const std::string &displayname):
		mSwisTrackCore(stc), mName(name), mDisplayName(displayname) {

	mConfigurationRoot=dynamic_cast<const xmlpp::Element*>(mSwisTrackCore->mConfigurationRoot->find(mName)).begin());
}

void THISCLASS::ClearStatus() {
	mStatus->clear();
}

void THISCLASS::AddError(const std::string &msg) {
	mStatus->push_back(StatusItem(StatusItem::sTypeError, msg));
}

void THISCLASS::AddWarning(const std::string &msg) {
	mStatus->push_back(StatusItem(StatusItem::sTypeWarning, msg));
}

void THISCLASS::AddInfo(const std::string &msg) {
	mStatus->push_back(StatusItem(StatusItem::sTypeInfo, msg));
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

bool THISCLASS::SetConfigurationInt(const std::string &path, int value) {
	xmlpp::Element *e=SetConfigurationElement(path);
	if (! e) {return false;}

	std::ostringstream oss;
	oss << value;
	e->set_child_text(std::string(oss));
	return true;
}

bool SetConfigurationDouble(const std::string &path, double value) {
	xmlpp::Element *e=SetConfigurationElement(path);
	if (! e) {return false;}

	std::ostringstream oss;
	oss << value;
	e->set_child_text(std::string(oss));
	return true;
}

bool SetConfigurationString(const std::string &path, const std::string &value) {
	xmlpp::Element *e=SetConfigurationElement(path);
	if (! e) {return false;}

	e->set_child_text(value);
	return true;
}
