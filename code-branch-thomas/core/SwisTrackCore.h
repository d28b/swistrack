#ifndef HEADER_SwisTrackCore
#define HEADER_SwisTrackCore

class SwisTrackCore;

#include <list>
#include <wx/xml/xml.h>
#include "Component.h"
#include "ComponentCategory.h"
#include "CommunicationInterface.h"
#include "DataStructureInput.h"
#include "DataStructureImage.h"
#include "DataStructureParticles.h"
#include "ErrorList.h"
#include "SwisTrackCoreInterface.h"
class SwisTrackCoreEditor;

//! The main class of the core part of SwisTrack. This class holds everything together.
class SwisTrackCore {
friend class SwisTrackCoreEditor;

public:
	//! A list of components type.
	typedef std::list<Component*> tComponentList;
	//! A list of data structures type.
	typedef std::list<DataStructure*> tDataStructureList;
	//! A list of SwisTrackCoreInterface objects.
	typedef std::list<SwisTrackCoreInterface*> tSwisTrackCoreInterfaceList;

	tComponentList mAvailableComponents;			//!< The list of all available components.
	tDataStructureList mDataStructures;				//!< The list of all available data structures.

	// Component categories
	ComponentCategory mCategoryInput;
	ComponentCategory mCategoryInputConversion;
	ComponentCategory mCategoryPreprocessing;
	ComponentCategory mCategoryThresholding;
	ComponentCategory mCategoryBlobDetection;
	ComponentCategory mCategoryCalibration;
	ComponentCategory mCategoryOutput;

	// Data structures
	DataStructureInput mDataStructureInput;
	DataStructureImage mDataStructureImageBGR;
	DataStructureImage mDataStructureImageGray;
	DataStructureImage mDataStructureImageBinary;
	DataStructureParticles mDataStructureParticles;

	CommunicationInterface *mCommunicationInterface;	//!< The associated communication interface.

	//! Constructor.
	SwisTrackCore();
	//! Destructor.
	~SwisTrackCore();

	//! Starts all the components. This may only be called if IsStarted()==false.
	bool Start(bool productivemode);
	//! Performs one complete step, including cleanup. This may only be called if IsStarted()==true.
	bool Step();
	//! Stops all started components. This may only be called if IsStarted()==true.
	bool Stop();
	//! Reloads the configuration while a session is runnning. This may only be called if IsStarted()==false. Note that not all configuration parameters may be reloaded. This function may also be called in productive mode.
	bool ReloadConfiguration();

	//! Writes the configuration to a XML document.
	void ConfigurationWriteXML(wxXmlNode* configuration, ErrorList *xmlerr);

	//! Returns the list of deployed components. Note that this list can only be read. To modify the list, you need to use a SwisTrackCoreEditor object.
	const SwisTrackCore::tComponentList *GetDeployedComponents() {return &mDeployedComponents;}
	//! Returns a component by name.
	Component *GetComponentByName(const std::string &name);

	//! Returns whether the components have been started.
	bool IsStarted() {return mStarted;}
	//! Returns whether the components have been started in productive mode.
	bool IsStartedInProductiveMode() {return (mStarted && mProductiveMode);}

	//! Adds an object to the list of interfaces. Objects on this list will be informed upon changes.
	void AddInterface(SwisTrackCoreInterface *stc);
	//! Removes an object from the list of interfaces.
	void RemoveInterface(SwisTrackCoreInterface *stc);

protected:
	bool mStarted;		//!< Whether the components have been started or not.
	bool mProductiveMode;	//!< Whether the components are running in productive mode or not. Note that this is only valid if mStarted=true.
	int mEditLocks;		//!< The number of edit locks.

	tComponentList mDeployedComponents;						//!< The list of deployed components.
	tSwisTrackCoreInterfaceList mSwisTrackCoreInterfaces;	//!< The list of SwisTrackCoreInterface objects.

	//! Increments the edit locks. If this is the first 
	bool IncrementEditLocks();
	//! Decrements the edit locks.
	void DecrementEditLocks();
};

#endif
