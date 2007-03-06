#include "Component.h"
#include <sstream>
#include <cctype>
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

DisplayImage *THISCLASS::GetDisplayImageByName(const std::string &name) {
	tDisplayImageList::iterator it=mDisplayImages.begin();
	while (it!=mDisplayImages.end()) {
		if ((*it)->mName==name) {return (*it);}
		it++;
	}

	return 0;
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
	std::string str=mConfiguration[key];
	if (str=="") {return defvalue;}

	std::transform(str.begin(), str.end(), str.begin(), std::tolower);
	if (str=="true") {return true;}
	if (str=="false") {return false;}
	int value=(defvalue ? 1 : 0);
	std::istringstream istr(str);
	istr >> value;
	return (value!=0);
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
	tConfigurationMap::const_iterator it=mConfiguration.find(key);
	if (it==mConfiguration.end()) {return defvalue;}
	return mConfiguration[key];
}

bool THISCLASS::IncrementEditLocks() {
	if (mEditLocks>0) {mEditLocks++; return true;}

	// Notify the interfaces
	SwisTrackCore::tSwisTrackCoreInterfaceList::iterator it=mCore->mSwisTrackCoreInterfaces.begin();
	while (it!=mCore->mSwisTrackCoreInterfaces.end()) {
		(*it)->OnBeforeEditComponent(this);
		it++;
	}

	mEditLocks=1;
	return true;
}

void THISCLASS::DecrementEditLocks() {
	if (mEditLocks<1) {return;}

	// Decrement and return if there are still other locks
	mEditLocks--;
	if (mEditLocks>0) {return;}

	// Notify the interfaces
	SwisTrackCore::tSwisTrackCoreInterfaceList::iterator it=mCore->mSwisTrackCoreInterfaces.begin();
	while (it!=mCore->mSwisTrackCoreInterfaces.end()) {
		(*it)->OnAfterEditComponent(this);
		it++;
	}
}
