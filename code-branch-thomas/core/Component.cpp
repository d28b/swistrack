#include "Component.h"
#include <sstream>
#include <algorithm>
#define THISCLASS Component

THISCLASS::Component(SwisTrackCore *stc, const std::string &name):
		mCore(stc), mName(name), mDisplayName(name), mCategory(0), mStatus(), mStatusHasError(false), mStatusHasWarning(false), mStarted(false), mDataStructureRead(), mDataStructureWrite() {

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
	mDataStructureRead.push_back(ds);
}

void THISCLASS::AddDataStructureWrite(DataStructure *ds) {
	mDataStructureWrite.push_back(ds);
}

bool THISCLASS::HasDataStructureRead(DataStructure *ds) {
	tDataStructureList::iterator it=find(mDataStructureRead.begin(), mDataStructureRead.end(), ds);
	return (it != mDataStructureRead.end());
}

bool THISCLASS::HasDataStructureWrite(DataStructure *ds) {
	tDataStructureList::iterator it=find(mDataStructureWrite.begin(), mDataStructureWrite.end(), ds);
	return (it != mDataStructureWrite.end());
}

void THISCLASS::ConfigurationReadXML(xmlpp::Element *configuration, ErrorList *xmlerr) {
	mConfiguration.clear();

	xmlpp::Node::NodeList list=configuration->get_children("parameter");
	xmlpp::Node::NodeList::iterator it=list.begin();
	while (it!=list.end()) {
		xmlpp::Element *element=dynamic_cast<xmlpp::Element *>(*it);
		if (element) {
			xmlpp::Attribute *att_name=element->get_attribute("name");
			xmlpp::Attribute *att_value=element->get_attribute("value");
			if ((att_name!=0) && (att_value!=0)) {
				mConfiguration[att_name->get_value()]=att_value->get_value();
			} else {
				std::ostringstream oss;
				oss << "The parameter at line " << element->get_line() << " was ignored because either the attribute 'name' or 'value' are missing.";
				xmlerr->Add(oss.str(), element->get_line());
			}
		}

		it++;
	}
}

void THISCLASS::ConfigurationWriteXML(xmlpp::Element *configuration, ErrorList *xmlerr) {
	tConfigurationMap::iterator it=mConfiguration.begin();
	while (it!=mConfiguration.end()) {
		xmlpp::Element *element=configuration->add_child("parameter");
		element->set_attribute("name", it->first);
		element->set_attribute("value", it->second);
		it++;
	}
}

bool THISCLASS::GetConfigurationBool(const std::string &key, bool defvalue) {
	std::istringstream istr(mConfiguration[key]);
	bool val=0;
	istr >> val;
	return val;
}

int THISCLASS::GetConfigurationInt(const std::string &key, int defvalue) {
	std::istringstream istr(mConfiguration[key]);
	int val=0;
	istr >> val;
	return val;
}

double THISCLASS::GetConfigurationDouble(const std::string &key, double defvalue) {
	std::istringstream istr(mConfiguration[key]);
	double val=0;
	istr >> val;
	return val;
}

std::string THISCLASS::GetConfigurationString(const std::string &key, const std::string &defvalue) {
	return mConfiguration[key];
}

bool THISCLASS::SetConfigurationBool(const std::string &key, bool value) {
	std::ostringstream oss;
	oss << value;
	mConfiguration[key]=oss.str();
	return true;
}

bool THISCLASS::SetConfigurationInt(const std::string &key, int value) {
	std::ostringstream oss;
	oss << value;
	mConfiguration[key]=oss.str();
	return true;
}

bool THISCLASS::SetConfigurationDouble(const std::string &key, double value) {
	std::ostringstream oss;
	oss << value;
	mConfiguration[key]=oss.str();
	return true;
}

bool THISCLASS::SetConfigurationString(const std::string &key, const std::string &value) {
	mConfiguration[key]=value;
	return true;
}
