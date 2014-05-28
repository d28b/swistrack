#ifndef HEADER_SwisTrackCore
#define HEADER_SwisTrackCore

class SwisTrackCore;

#include <list>
#include <wx/xml/xml.h>
#include <wx/filename.h>
#include <wx/ffile.h>
#include "Component.h"
#include "ComponentCategory.h"
#include "CommunicationInterface.h"
#include "DataStructureInput.h"
#include "DataStructureImage.h"
#include "DataStructureParticles.h"
#include "DataStructureTracks.h"
#include "DataStructureCommand.h"
#include "ErrorList.h"
#include "SwisTrackCoreInterface.h"
#include "SwisTrackCoreTrigger.h"
#include "SwisTrackCoreEventRecorder.h"
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
	//! A map of ComponentCategory objects.
	typedef std::map<wxString, ComponentCategory> tComponentCategoryMap;

	tComponentList mAvailableComponents;					//!< The list of all available components.
	tDataStructureList mDataStructures;						//!< The list of all available data structures.
	tSwisTrackCoreInterfaceList mSwisTrackCoreInterfaces;	//!< The list of SwisTrackCoreInterface objects.
	wxString mComponentConfigurationFolder;				//!< The path to the component configuration files, including the trailing slash.
	tComponentCategoryMap mComponentCategories;				//!< The component categories. (not used, TODO: read categories from XML file and put them here, then modify components to read categoryID from their XML file)
	CommunicationInterface *mCommunicationInterface;		//!< The associated communication interface.
	SwisTrackCoreTrigger *mTrigger;							//!< The associated trigger.
	SwisTrackCoreEventRecorder *mEventRecorder;				//!< The associated event recorder.
	bool mTimeCritical;										//!< Whether one or more components would like more time. (writable by components, readable by the UI)

	// Component categories
	ComponentCategory mCategoryTrigger;
	ComponentCategory mCategoryInput;
	ComponentCategory mCategoryInputConversion;
	ComponentCategory mCategoryPreprocessingColor;
	ComponentCategory mCategoryPreprocessingGray;
	ComponentCategory mCategoryThresholdingColor;
	ComponentCategory mCategoryThresholdingGray;
	ComponentCategory mCategoryPreprocessingBinary;
	ComponentCategory mCategoryParticleDetection;
	ComponentCategory mCategoryCalibration;
	ComponentCategory mCategoryTracking;
	ComponentCategory mCategoryOutput;

	// Data structures
	DataStructureInput mDataStructureInput;
	DataStructureImage mDataStructureImageColor;
	DataStructureImage mDataStructureImageGray;
	DataStructureImage mDataStructureImageFFT;        // used by FFT modules (F. Schill 2014)
	DataStructureImage mDataStructureImageBinary;
	DataStructureParticles mDataStructureParticles;
	DataStructureTracks mDataStructureTracks;
	DataStructureCommand mDataStructureCommands;


	//! Constructor.
	SwisTrackCore(wxString componentconfigurationfolder);
	//! Destructor.
	~SwisTrackCore();

	//! Sets the file. Note that this is used to determine the project and run folders only. The file itself is not opened or read (and does not even need to exist).
	void SetFileName(const wxFileName &filename);
	//! Sets the file. Note that this is used to determine the project and run folders only. The file itself is not opened or read (and does not even need to exist).
	wxFileName GetFileName() {
		return mFileName;
	}
	//! Returns the current run title.
	wxString GetRunTitle() {
		return mRunTitle;
	}
	//! Returns a path to a file in the project folder.
	wxFileName GetProjectFileName(const wxString &filename_str);
	//! Returns a path to a file in the run folder.
	wxFileName GetRunFileName(const wxString &filename_str);

	//! Starts all the components. This may only be called if IsStarted()==false.
	bool Start(bool productionmode);
	//! Performs one complete step, including cleanup. This may only be called if IsStarted()==true.
	bool Step();
	//! Stops all started components. This may only be called if IsStarted()==true.
	bool Stop();
	//! Reloads the configuration while a session is runnning. This may only be called if IsStarted()==false. Note that not all configuration parameters may be reloaded. This function may also be called in production mode.
	bool ReloadConfiguration();

	//! Starts the automatic trigger.
	void TriggerStart();
	//! Stops the automatic trigger.
	void TriggerStop();

	//! Writes the configuration to a XML document.
	void ConfigurationWriteXML(wxXmlNode* configuration, ErrorList *xmlerr);

	//! Returns the list of deployed components. Note that this list can only be read. To modify the list, you need to use a SwisTrackCoreEditor object.
	const SwisTrackCore::tComponentList *GetDeployedComponents() {
		return &mDeployedComponents;
	}
	//! Returns a component by name.
	Component *GetComponentByName(const wxString &name);

	//! Returns whether the components have been started.
	bool IsStarted() {
		return mStarted;
	}
	//! Returns whether the components have been started in production mode.
	bool IsStartedInProductionMode() {
		return (mStarted && mProductionMode);
	}
	//! Returns whether the automatic trigger is active.
	bool IsTriggerActive() {
		return mTrigger->GetActive();
	}
	//! Returns the value of the step counter.
	int GetStepCounter() {
		return mStepCounter;
	}

	//! Adds an object to the list of interfaces. Objects on this list will be informed upon changes.
	void AddInterface(SwisTrackCoreInterface *stc);
	//! Removes an object from the list of interfaces.
	void RemoveInterface(SwisTrackCoreInterface *stc);

	//! This method should be called whenever the system becomes idle. (Since wxWidgets 2.8 sends these events only to frames, we need to relay them from SwisTrack to SwisTrackCore.)
	void OnIdle();

protected:
	bool mStarted;				//!< Whether the components have been started or not.
	bool mProductionMode;		//!< Whether the components are running in production mode or not. Note that this is only valid if mStarted=true.
	int mStepCounter;			//!< The number of steps processed since the last start of the pipeline.
	int mEditLocks;				//!< The number of edit locks.
	wxFileName mFileName;		//!< The full path and name of the .swistrack file.
	wxString mFileTitle;		//!< The title of the file (usually derived from mFile).
	wxString mProjectFolder;	//!< The project folder (usually derived from mFile).
	wxString mRunTitle;			//!< The title of the run.

	tComponentList mDeployedComponents;			//!< The list of deployed components.

	//! Increments the edit locks.
	bool IncrementEditLocks();
	//! Decrements the edit locks.
	void DecrementEditLocks();
};

#endif
