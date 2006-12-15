#ifndef HEADER_Component
#define HEADER_Component

#include <string>
#include <list>
#include <libxml++/libxml++.h>
#include "XMLCfg.h"
#include "StatusItem.h"

class Component {

public:
	std::string mName;
	std::list<StatusItem> mStatusList;

	Component(): mCfgRoot(0) {}
	Component(xmlpp::Element* cfgroot): mCfgRoot(cfgroot) {}
	virtual ~Component() {}
	
	virtual void SetParameters() = 0;
	virtual void Step() = 0;

protected:
	xmlpp::Element* mCfgRoot;

	void ClearStatus();
	void AddError(const std::string &msg);
	void AddWarning(const std::string &msg);
	void AddInfo(const std::string &msg);
};

#endif