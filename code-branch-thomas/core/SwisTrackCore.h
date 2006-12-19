#ifndef HEADER_SwisTrackCore
#define HEADER_SwisTrackCore

#include <list>
#include "Component.h"
#include "DataStructureImage.h"

class SwisTrackCore {

public:
	typedef std::list<Component*> tComponentList;
	tComponentList mComponents;
	
	DataStructureInput mDataStructureInput;
	DataStructureImage mDataStructureImageBGR;
	DataStructureImage mDataStructureBackgroundBGR;
	DataStructureImage mDataStructureImageGray;
	DataStructureImage mDataStructureBackgroundGray;
	DataStructureImage mDataStructureImageBinary;
	DataStructureImage mDataStructureMaskBinary;
	DataStructureParticles mDataStructureParticles;

	SwisTrackCore(xmlpp::Element* cfgroot=0): mConfigurationRoot(cfgroot), mDataStructureInput(), mDataStructureImageBGR("ImageBGR", "Color image (BGR)") {}
	virtual ~SwisTrackCore() {}

	virtual void SetParameters() = 0;
	virtual void Step() = 0;

protected:
	xmlpp::Element* mConfigurationRoot;

};

#endif
