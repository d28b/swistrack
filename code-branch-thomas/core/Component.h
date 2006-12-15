#ifndef _COMPONENT_H
#define _COMPONENT_H

#include <libxml++/libxml++.h>
#include "XMLCfg.h"

class Component {

public:
	Component(): mCfgRoot(0) {}
	Component(xmlpp::Element* cfgroot): mCfgRoot(cfgroot) {}
	virtual ~Component() {}
	
	virtual void SetParameters() = 0;
	virtual void Step() = 0;

protected:
	xmlpp::Element* mCfgRoot;

};

#endif