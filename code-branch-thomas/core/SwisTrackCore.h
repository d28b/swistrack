#ifndef HEADER_SwisTrackCore
#define HEADER_SwisTrackCore

class SwisTrackCore;

#include <list>
#include "Component.h"
#include "DataStructureInput.h"
#include "DataStructureImage.h"
#include "DataStructureParticles.h"

class SwisTrackCore {

public:
	//! The list of all available components.
	typedef std::list<Component*> tComponents;
	tComponents mComponents;

	//! The list of all available data structures.
	typedef std::list<DataStructure*> tDataStructures;
	tDataStructures mDataStructures;

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

	bool Start();
	bool Step();
	bool Stop();
protected:
	xmlpp::Element* mConfigurationRoot;

};

#endif
