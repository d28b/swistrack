#ifndef HEADER_SwisTrackCore
#define HEADER_SwisTrackCore

#include <list>
#include "Component.h"
#include "DataStructureInput.h"
#include "DataStructureImage.h"
#include "DataStructureParticles.h"

class SwisTrackCore {

public:
	//! The list of all available component factories.
	typedef std::list<ComponentFactory*> tComponents;
	tComponents mComponents;

	//! The list of the deployed components.
	typedef std::list<Component*> tComponentList;
	tComponentList mComponentList;

	//! Data structures
	DataStructureInput mDataStructureInput;
	DataStructureImage mDataStructureImageBGR;
	DataStructureImage mDataStructureBackgroundBGR;
	DataStructureImage mDataStructureImageGray;
	DataStructureImage mDataStructureBackgroundGray;
	DataStructureImage mDataStructureImageBinary;
	DataStructureImage mDataStructureMaskBinary;
	DataStructureParticles mDataStructureParticles;

	SwisTrackCore(xmlpp::Element* cfgroot=0);
	~SwisTrackCore() {}

protected:
	xmlpp::Element* mConfigurationRoot;

};

#endif
