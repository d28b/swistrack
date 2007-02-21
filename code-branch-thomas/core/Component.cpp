#include "Component.h"
#include <sstream>
#include <algorithm>
#define THISCLASS Component

THISCLASS::Component(SwisTrackCore *stc, const std::string &name):
		mCore(stc), mName(name), mDisplayName(name), mCategory(0), mStatus(), mStatusHasError(false), mStatusHasWarning(false), mStarted(false), mDataStructureRead(), mDataStructureWrite() {

	//mConfigurationRoot=dynamic_cast<const wxXmlNode*>(mCore->mConfigurationRoot->find(mName)).begin());
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

void THISCLASS::AddDisplayImage(DisplayImage *di) {
	mDisplayImages.push_back(di);
}

bool THISCLASS::HasDataStructureRead(DataStructure *ds) {
	tDataStructureList::iterator it=find(mDataStructureRead.begin(), mDataStructureRead.end(), ds);
	return (it != mDataStructureRead.end());
}

bool THISCLASS::HasDataStructureWrite(DataStructure *ds) {
	tDataStructureList::iterator it=find(mDataStructureWrite.begin(), mDataStructureWrite.end(), ds);
	return (it != mDataStructureWrite.end());
}

void THISCLASS::ConfigurationReadXML(wxXmlNode *configuration, ErrorList *xmlerr) {
	mConfiguration.clear();

	wxXmlNode *node=configuration->GetChildren();
	while (node) {
		if (node->GetName()=="parameter") {
			wxString name="";
			wxString value="";
			wxXmlProperty *prop=node->GetProperties();
			while (prop) {
				if (prop->GetName()=="name") {name=prop->GetValue();}
				if (prop->GetName()=="value") {value=prop->GetValue();}
				prop=prop->GetNext();
			}
			if ((name=="") && (value=="")) {
				std::ostringstream oss;
				oss << "A parameter of the component '" << mName << "' was ignored because either the attribute 'name' or 'value' are missing.";
				xmlerr->Add(oss.str(), 0);  // The current wxXml implementation doesn't care about line numbers.
			} else {
				mConfiguration[name.c_str()]=value.c_str();
			}
		}

		node=node->GetNext();
	}
}

void THISCLASS::ConfigurationWriteXML(wxXmlNode *configuration, ErrorList *xmlerr) {
	tConfigurationMap::iterator it=mConfiguration.begin();
	while (it!=mConfiguration.end()) {
		wxXmlNode *node=new wxXmlNode(0, wxXML_ELEMENT_NODE, "parameter");
		configuration->AddChild(node);
		node->AddProperty("name", it->first.c_str());
		node->AddProperty("value", it->second.c_str());
		it++;
	}
}

bool THISCLASS::GetConfigurationBool(const std::string &key, bool defvalue) {
	std::istringstream istr(mConfiguration[key]);
	istr >> defvalue;
	return defvalue;
}

int THISCLASS::GetConfigurationInt(const std::string &key, int defvalue) {
	std::istringstream istr(mConfiguration[key]);
	istr >> defvalue;
	return defvalue;
}

double THISCLASS::GetConfigurationDouble(const std::string &key, double defvalue) {
	std::istringstream istr(mConfiguration[key]);
	istr >> defvalue;
	return defvalue;
}

std::string THISCLASS::GetConfigurationString(const std::string &key, const std::string &defvalue) {
	// TODO: check whether key exists
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
