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
#include "ErrorList.h"

class SwisTrackCore {

public:
	//! The list of all available components.
	typedef std::list<Component*> tComponentList;
	//! The list of all available components.
	tComponentList mAvailableComponents;
	//! The list of the deployed components.
	tComponentList mDeployedComponents;

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

	//! Constructor.
	SwisTrackCore();
	//! Destructor.
	~SwisTrackCore() {}

	//! Starts all the components.
	bool Start();
	//! Performs one complete step, including cleanup.
	bool Step();
	//! Stops all started components.
	bool Stop();
	//! Resets all started components.
	void Reset();

	//! Clears all deployed components.
	void Clear();
	//! Reads the configuration from a XML document.
	void ConfigurationReadXML(xmlpp::Element* configuration, ErrorList *xmlerr);
	//! Writes the configuration to a XML document.
	void ConfigurationWriteXML(xmlpp::Element* configuration, ErrorList *xmlerr);
	//! Returns a component by name.
	Component *GetComponentByName(const std::string &name);

	//! Returns whether the components have been started.
	bool IsStarted() {return mStarted;}

protected:
	//! Whether the components have been started or not.
	bool mStarted;

	//! Reads one component from the XML file.
	void ConfigurationReadXMLElement(xmlpp::Element* element, ErrorList *xmlerr);

};

#endif
