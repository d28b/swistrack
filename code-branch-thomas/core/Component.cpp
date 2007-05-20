#include "Component.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <wx/log.h>
#include "ConfigurationXML.h"
#define THISCLASS Component

THISCLASS::Component(SwisTrackCore *stc, const std::string &name):
		mStatus(), mStatusHasError(false), mStatusHasWarning(false), mStarted(false),
		mName(name), mDisplayName(name), mDescription(), mHelpURL(), mCategory(0), mDefaultDisplay(), mTrigger(0),
		mInitializationErrors(), mStepDuration(-1),
		mDataStructureRead(), mDataStructureWrite(), mDisplays(),
		mCore(stc), mConfiguration(), mConfigurationDefault(), mEditLocks(0) {

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

void THISCLASS::AddDisplay(Display *di) {
	di->mComponent=this;
	mDisplays.push_back(di);
}

bool THISCLASS::HasDataStructureRead(DataStructure *ds) {
	tDataStructureList::iterator it=find(mDataStructureRead.begin(), mDataStructureRead.end(), ds);
	return (it != mDataStructureRead.end());
}

bool THISCLASS::HasDataStructureWrite(DataStructure *ds) {
	tDataStructureList::iterator it=find(mDataStructureWrite.begin(), mDataStructureWrite.end(), ds);
	return (it != mDataStructureWrite.end());
}

Display *THISCLASS::GetDisplayByName(const std::string &name) {
	tDisplayList::iterator it=mDisplays.begin();
	while (it!=mDisplays.end()) {
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
	if (str=="") {
		str=mConfigurationDefault[key];
		if (str=="") {
			return defvalue;
		}
	}

	std::transform(str.begin(), str.end(), str.begin(), std::tolower);
	if (str=="true") {return true;}
	if (str=="false") {return false;}
	int value=(defvalue ? 1 : 0);
	std::istringstream istr(str);
	istr >> value;
	return (value!=0);
}

int THISCLASS::GetConfigurationInt(const std::string &key, int defvalue) {
	std::istringstream istrdefault(mConfigurationDefault[key]);
	istrdefault >> defvalue;
	std::istringstream istr(mConfiguration[key]);
	istr >> defvalue;
	return defvalue;
}

double THISCLASS::GetConfigurationDouble(const std::string &key, double defvalue) {
	std::istringstream istrdefault(mConfigurationDefault[key]);
	istrdefault >> defvalue;
	std::istringstream istr(mConfiguration[key]);
	istr >> defvalue;
	return defvalue;
}

std::string THISCLASS::GetConfigurationString(const std::string &key, const std::string &defvalue) {
	tConfigurationMap::const_iterator it=mConfiguration.find(key);
	if (it==mConfiguration.end()) {
		it=mConfigurationDefault.find(key);
		if (it==mConfigurationDefault.end()) {return defvalue;}
	}
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

void THISCLASS::Initialize() {
	// Read the configuration and create the parameter panels
	std::string filename=mCore->mComponentConfigurationFolder+mName+".xml";

	// Open the file
	wxLogNull log;
	wxXmlDocument document;
	if (! document.Load(filename)) {
		std::ostringstream oss;
		oss << "Unable to open or parse the file '" << filename << "'.";
		mInitializationErrors.Add(oss.str());
		return;
	}

	// Prepare
	ConfigurationXML config(document.GetRoot());

	// Set title and basic information
	wxString friendlyname=config.ReadString("friendlyname", "");
	if (friendlyname!="") {
		mDisplayName=friendlyname.c_str();
	}
	mDescription=config.ReadString("description", "");
	mHelpURL=config.ReadString("url", "");
	mDefaultDisplay=config.ReadString("defaultdisplay", "");

	// Read the list of settings
	wxXmlNode *configurationnode=config.GetChildNode("configuration");
	InitializeReadConfiguration(configurationnode);
}

void THISCLASS::InitializeReadConfiguration(wxXmlNode *configurationnode) {
	if (! configurationnode) {
		mInitializationErrors.Add("The node 'configuration' is missing.");
		return;
	}

	wxXmlNode *node=configurationnode->GetChildren();
	while (node) {
		InitializeReadConfigurationNode(node);
		node=node->GetNext();
	}

	return;
}

void THISCLASS::InitializeReadConfigurationNode(wxXmlNode *node) {
	wxString nodename=node->GetName();
	if (nodename=="parameter") {
		InitializeReadParameter(node);
	} else if (nodename=="label") {
	} else if (nodename=="title") {
	} else if (nodename=="line") {
	} else {
		std::ostringstream oss;
		oss << "Invalid node '" << nodename << "' in 'configuration'. Allowed types are 'line', 'label', 'title' and 'parameter'.";
		mInitializationErrors.Add(oss.str());
	}
}

void THISCLASS::InitializeReadParameter(wxXmlNode *node) {
	if (! node) {return;}

	// Read name and default value
	ConfigurationXML config(node);
	std::string name=config.ReadString("name", "").c_str();
	std::string defaultvalue=config.ReadString("default", "").c_str();
	mConfigurationDefault[name]=defaultvalue;
}
