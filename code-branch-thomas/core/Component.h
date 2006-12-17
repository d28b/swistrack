#ifndef HEADER_Component
#define HEADER_Component

#include <string>
#include <list>
#include <libxml++/libxml++.h>
#include "StatusItem.h"

class Component {

public:
	typedef std::list<StatusItem> tStatusItemList;
	std::string mName;
	std::string mDisplayName;
	tStatusItemList mStatus;

	Component(SwisTrackCore *stc, const std::string &nam);
	virtual ~Component() {}
	
	virtual bool Start() = 0;
	virtual bool Step() = 0;
	virtual bool Stop() = 0;

	int GetConfigurationInt(const std::string &path, int defvalue);
	double GetConfigurationDouble(const std::string &path, double defvalue);
	std::string GetConfigurationString(const std::string &path, const std::string &defvalue);

	bool SetConfigurationInt(const std::string &path, int value);
	bool SetConfigurationDouble(const std::string &path, double value);
	bool SetConfigurationString(const std::string &path, const std::string &value);

protected:
	SwisTrackCore *mSwisTrackCore;
	xmlpp::Element *mConfigurationRoot;

	void ClearStatus();
	void AddError(const std::string &msg);
	void AddWarning(const std::string &msg);
	void AddInfo(const std::string &msg);

	inline xmlpp::Element* THISCLASS::GetConfigurationElement(const std::string &path) {
		return dynamic_cast<const xmlpp::Element*>(mConfigurationRoot->find(path)).begin());
	}
};

#endif