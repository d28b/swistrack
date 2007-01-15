#ifndef HEADER_SwisTrackCore
#define HEADER_SwisTrackCore

class SwisTrackCore;

#include <list>
#include "Component.h"
#include "ComponentCategory.h"
#include "CommunicationInterface.h"
#include "DataStructureInput.h"
#include "DataStructureImage.h"
#include "DataStructureParticles.h"

class SwisTrackCore {

public:
	//! The list of all available components.
	typedef std::list<Component*> tComponents;
	//! The list of all available components.
	tComponents mAvailableComponents;
	//! The list of the deployed components.
	tComponents mDeployedComponents;

	//! The list type of all available data structures.
	typedef std::list<DataStructure*> tDataStructures;
	//! The list of all available data structures.
	tDataStructures mDataStructures;

	// Component categories.
	ComponentCategory mCategoryInput;
	ComponentCategory mCategoryInputConversion;
	ComponentCategory mCategoryPreprocessing;
	ComponentCategory mCategoryThresholding;
	ComponentCategory mCategoryBlobDetection;
	ComponentCategory mCategoryOutput;

	// Data structures.
	DataStructureInput mDataStructureInput;
	DataStructureImage mDataStructureImageBGR;
	DataStructureImage mDataStructureBackgroundBGR;
	DataStructureImage mDataStructureImageGray;
	DataStructureImage mDataStructureBackgroundGray;
	DataStructureImage mDataStructureImageBinary;
	DataStructureImage mDataStructureMaskBinary;
	DataStructureParticles mDataStructureParticles;

	//! The associated communication interface.
	CommunicationInterface *mCommunicationInterface;

	SwisTrackCore();
	~SwisTrackCore() {}

	//! Starts all the components.
	bool Start();
	//! Performs one step, including cleanup.
	bool Step();
	//! Stops all started components.
	bool Stop();

	//! Reads the configuration from a XML document.
	void ConfigurationReadXML(xmlpp::Element* configuration, ErrorList *xmlerr);
	//! Writes the configuration to a XML document.
	void ConfigurationWriteXML(xmlpp::Element* configuration, ErrorList *xmlerr);
	//! Returns a component by name.
	Component *GetComponentByName(const std::string &name);

protected:
	//! Reads one component from the XML file.
	void ConfigurationReadXMLElement(xmlpp::Element* element, ErrorList *xmlerr);

};

#endif
