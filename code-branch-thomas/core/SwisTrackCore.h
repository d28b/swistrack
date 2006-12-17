#ifndef HEADER_SwisTrackCore
#define HEADER_SwisTrackCore

#include <list>
#include "Component.h"
#include "DataStructureImage.h"

class SwisTrackCore {

public:
	typedef std::list<Component*> tComponentList;
	DataStructureImage mDataStructureImage;
	tComponentList mComponents;

	SwisTrackCore(): mConfigurationRoot(0) {}
	SwisTrackCore(xmlpp::Element* cfgroot): mConfigurationRoot(cfgroot) {}
	virtual ~SwisTrackCore() {}

	virtual void SetParameters() = 0;
	virtual void Step() = 0;

protected:
	xmlpp::Element* mConfigurationRoot;

};

#endif
