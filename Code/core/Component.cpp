#include "Component.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <wx/log.h>
#include "ConfigurationConversion.h"
#include "ConfigurationXML.h"
#include "ImageTools.h"
#include "SwisTrackCoreInterface.h"
#define THISCLASS Component

THISCLASS::Component(SwisTrackCore * stc, const wxString &name):
	mStatus(), mStatusHasError(false), mStatusHasWarning(false), mStarted(false),
	mName(name), mDisplayName(name), mDescription(), mHelpURL(), mCategory(0), mDefaultDisplay(), mTrigger(0),
	mInitializationErrors(), mStepDuration(-1),
	mDataStructureRead(), mDataStructureWrite(), mDisplays(),
	mCore(stc), mConfiguration(), mConfigurationDefault(), mEnabledInterval(1), mEditLocks(0) {

}

void THISCLASS::ClearStatus() {
	mStatus.clear();
	mStatusHasError = false;
	mStatusHasWarning = false;
}

void THISCLASS::AddError(const wxString &msg) {
	mStatus.push_back(StatusItem(StatusItem::sTypeError, msg));
	mStatusHasError = true;
}

void THISCLASS::AddWarning(const wxString &msg) {
	mStatus.push_back(StatusItem(StatusItem::sTypeWarning, msg));
	mStatusHasWarning = true;
}

void THISCLASS::AddInfo(const wxString &msg) {
	mStatus.push_back(StatusItem(StatusItem::sTypeInfo, msg));
}

void THISCLASS::AddDataStructureRead(DataStructure *ds) {
	mDataStructureRead.push_back(ds);
}

void THISCLASS::AddDataStructureWrite(DataStructure *ds) {
	mDataStructureWrite.push_back(ds);
}

void THISCLASS::AddDisplay(Display *di) {
	di->mComponent = this;
	mDisplays.push_back(di);
}

bool THISCLASS::HasDataStructureRead(DataStructure *ds) {
	tDataStructureList::iterator it = find(mDataStructureRead.begin(), mDataStructureRead.end(), ds);
	return (it != mDataStructureRead.end());
}

bool THISCLASS::HasDataStructureWrite(DataStructure *ds) {
	tDataStructureList::iterator it = find(mDataStructureWrite.begin(), mDataStructureWrite.end(), ds);
	return (it != mDataStructureWrite.end());
}

Display * THISCLASS::GetDisplayByName(const wxString &name) {
	tDisplayList::iterator it = mDisplays.begin();
	while (it != mDisplays.end()) {
		if ((*it)->mName == name) {
			return (*it);
		}
		it++;
	}

	return 0;
}

void THISCLASS::ConfigurationWriteXML(wxXmlNode *configuration, ErrorList *xmlerr) {
	// Write enabled flag
	wxXmlNode * node = new wxXmlNode(0, wxXML_ELEMENT_NODE, wxT("enabledinterval"));
	configuration->AddChild(node);
	node->AddAttribute(wxT("value"), ConfigurationConversion::Int(mEnabledInterval));

	// Write configuration
	tConfigurationMap::iterator it = mConfiguration.begin();
	while (it != mConfiguration.end()) {
		wxXmlNode * node = new wxXmlNode(0, wxXML_ELEMENT_NODE, wxT("parameter"));
		configuration->AddChild(node);
		node->AddAttribute(wxT("name"), it->first);
		node->AddAttribute(wxT("value"), it->second);
		it++;
	}
}

int THISCLASS::GetEnabledInterval() {
	return mEnabledInterval;
}

bool THISCLASS::GetConfigurationButton(const wxString &key) {
	tConfigurationMap::iterator it = mConfiguration.find(key);
	if (it == mConfiguration.end()) return false;

	if (! ConfigurationConversion::Bool(it->second, false)) return false;
	it->second = wxT("");
	return true;
}

bool THISCLASS::GetConfigurationBool(const wxString &key, bool defaultValue) const {
	return ConfigurationConversion::Bool(GetConfigurationString(key, wxT("")), defaultValue);
}

int THISCLASS::GetConfigurationInt(const wxString &key, int defaultValue) const {
	return ConfigurationConversion::Int(GetConfigurationString(key, wxT("")), defaultValue);
}

double THISCLASS::GetConfigurationDouble(const wxString &key, double defaultValue) const {
	return ConfigurationConversion::Double(GetConfigurationString(key, wxT("")), defaultValue);
}

wxString THISCLASS::GetConfigurationString(const wxString &key, const wxString &defaultValue) const {
	// If the key is available in the configuration, return its value
	tConfigurationMap::const_iterator it = mConfiguration.find(key);
	if (it != mConfiguration.end()) return it->second;

	// If the key is available in the default configuration, return that value
	it = mConfigurationDefault.find(key);
	if (it != mConfigurationDefault.end()) return it->second;

	// Otherwise, return the default value provided to this function
	return defaultValue;
}

wxDateTime THISCLASS::GetConfigurationDate(const wxString &key, const wxDateTime &defaultValue) const {
	return ConfigurationConversion::Date(GetConfigurationString(key, wxT("")), defaultValue);
}

wxColor THISCLASS::GetConfigurationColor(const wxString &key, const wxColor defaultValue) const {
	return ConfigurationConversion::Color(GetConfigurationString(key, wxT("")), defaultValue);
}

cv::Scalar THISCLASS::GetConfigurationColor(const wxString &key, const cv::Scalar defaultValue) const {
	return ConfigurationConversion::Color(GetConfigurationString(key, wxT("")), defaultValue);
}

cv::Mat THISCLASS::LoadConfigurationImage(const wxString &key, const wxString &label) {
	wxFileName filename = mCore->GetProjectFileName(GetConfigurationString(key, wxT("")));
	if (! filename.IsOk()) {
		AddError(label + wxT(": Invalid image filename."));
		return cv::Mat();
	}

	cv::Mat image = cv::imread(filename.GetFullPath().ToStdString(), cv::IMREAD_COLOR);
	if (image.empty()) {
		AddError(label + wxT(": Cannot load image from file '") + filename.GetFullPath() + wxT("'."));
		return cv::Mat();
	}

	return image;
}

cv::Mat THISCLASS::LoadConfigurationGrayscaleImage(const wxString &key, const wxString &label) {
	cv::Mat image = LoadConfigurationImage(key, label);
	if (image.empty()) return image;
	if (image.channels() == 1) return image;

	AddError(label + wxT(": Image is not a grayscale image."));
	return cv::Mat();
}

cv::Mat THISCLASS::LoadConfigurationColorImage(const wxString &key, const wxString &label) {
	cv::Mat image = LoadConfigurationImage(key, label);
	if (image.empty()) return image;
	if (image.channels() == 3) return image;

	AddError(label + wxT(": Image is not a color image."));
	return cv::Mat();
}

bool THISCLASS::IncrementEditLocks() {
	if (mEditLocks > 0) {
		mEditLocks++;
		return true;
	}

	// Notify the interfaces
	SwisTrackCore::tSwisTrackCoreInterfaceList::iterator it = mCore->mSwisTrackCoreInterfaces.begin();
	while (it != mCore->mSwisTrackCoreInterfaces.end()) {
		(*it)->OnBeforeEditComponent(this);
		it++;
	}

	mEditLocks = 1;
	return true;
}

void THISCLASS::DecrementEditLocks() {
	if (mEditLocks < 1) {
		return;
	}

	// Decrement and return if there are still other locks
	mEditLocks--;
	if (mEditLocks > 0) {
		return;
	}

	// Notify the interfaces
	SwisTrackCore::tSwisTrackCoreInterfaceList::iterator it = mCore->mSwisTrackCoreInterfaces.begin();
	while (it != mCore->mSwisTrackCoreInterfaces.end()) {
		(*it)->OnAfterEditComponent(this);
		it++;
	}
}

void THISCLASS::Initialize() {
	// Read the configuration and create the parameter panels
	wxString filename = mCore->mComponentConfigurationFolder + mName + wxT(".xml");

	// Open the file
	wxLogNull log;
	wxXmlDocument document;
	if (! document.Load(filename)) {
		mInitializationErrors.Add(wxT("Unable to open or parse the file \'") + filename + wxT("\'."));
		return;
	}

	// Prepare
	ConfigurationXML config(document.GetRoot());

	// Set title and basic information
	wxString friendlyname = config.ReadString(wxT("friendlyname"), wxT(""));
	if (friendlyname != wxT("")) {
		mDisplayName = friendlyname;
	}
	mDescription = config.ReadString(wxT("description"), wxT(""));
	mHelpURL = config.ReadString(wxT("url"), wxT(""));
	mDefaultDisplay = config.ReadString(wxT("defaultdisplay"), wxT(""));

	// Read the list of settings
	wxXmlNode *configurationnode = config.GetChildNode(wxT("configuration"));
	InitializeReadConfiguration(configurationnode);
}

void THISCLASS::InitializeReadConfiguration(wxXmlNode *configurationnode) {
	if (! configurationnode) {
		mInitializationErrors.Add(wxT("The node 'configuration' is missing."));
		return;
	}

	wxXmlNode *node = configurationnode->GetChildren();
	while (node) {
		InitializeReadConfigurationNode(node);
		node = node->GetNext();
	}

	return;
}

void THISCLASS::InitializeReadConfigurationNode(wxXmlNode *node) {
	wxString nodename = node->GetName();
	if (nodename == wxT("parameter")) {
		InitializeReadParameter(node);
	} else if (nodename == wxT("label")) {
	} else if (nodename == wxT("title")) {
	} else if (nodename == wxT("line")) {
	} else {
		mInitializationErrors.Add(wxT("Invalid node '") + nodename + wxT("' in 'configuration'. Allowed types are 'line', 'label', 'title' and 'parameter'."));
	}
}

void THISCLASS::InitializeReadParameter(wxXmlNode *node) {
	if (! node) {
		return;
	}

	// Read name and default value
	ConfigurationXML config(node);
	wxString name = config.ReadString(wxT("name"), wxT(""));
	wxString defaultvalue = config.ReadString(wxT("default"), wxT(""));
	mConfigurationDefault[name] = defaultvalue;
}
