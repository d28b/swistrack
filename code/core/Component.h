#ifndef HEADER_Component
#define HEADER_Component

class Component;

#include <string>
#include <list>
#include <map>
#include <wx/xml/xml.h>
#include "StatusItem.h"
#include "SwisTrackCore.h"
#include "DataStructure.h"
#include "ComponentCategory.h"
#include "ErrorList.h"
#include "Display.h"
#include "ComponentTrigger.h"

//! The base class for all components.
class Component {
friend class ComponentEditor;

public:
	//! Status item list type.
	typedef std::list<StatusItem> tStatusItemList;
	//! The type for configuration values.
	typedef std::map<std::string, std::string> tConfigurationMap;
	//! Display list type.
	typedef std::list<Display*> tDisplayList;
	//! Data structure list type.
	typedef std::list<DataStructure*> tDataStructureList;

	tStatusItemList mStatus;			//!< The status item list (mostly list of errors).
	bool mStatusHasError;				//!< Whether there is an error in the status item list. (This only exists for performance reasons. A program could also go through the status list and check if there is an error message.)
	bool mStatusHasWarning;				//!< Whether there is a warning in the status item list. (This only exists for performance reasons. A program could also go through the status list and check if there is an error message.)
	bool mStarted;						//!< Tells whether the component is started. This flag is set and reset by the SwisTrackCore class and should not be used by the components.
	std::string mName;					//!< The internal name of the component (used to save configuration).
	std::string mDisplayName;			//!< The name that is displayed to the user.
	std::string mDescription;			//!< The description.
	std::string mHelpURL;				//!< The url pointing to the website with more information about the component.
	ComponentCategory *mCategory;		//!< The category which this component belongs to.
	std::string mDefaultDisplay;		//!< The default view (not used).
	ComponentTrigger *mTrigger;			//!< The trigger of this component (if any).
	ErrorList mInitializationErrors;	//!< Error messages (during component initialization) are added here.
	double mStepDuration;				//!< The duration of the last step in milliseconds. This information is filled in by SwisTrackCore.

	tDataStructureList mDataStructureRead;		//! The data structures that are read by this component.
	tDataStructureList mDataStructureWrite;		//! The data structures that are written by this component.
	tDisplayList mDisplays;						//! The Display objects that this component provides.

	//! Constructor.
	Component(SwisTrackCore *stc, const std::string &name);
	//! Destructor.
	virtual ~Component() {}

	//! This event is called to initialize the environment for the component at the time the program is loaded. It is called exactly once at program runtime.
	virtual void OnInitializeStatic() {}
	//! This event is called to terminate the environment for the the component at the time the program quits. It is called exactly once at program runtime.
	virtual void OnTerminateStatic() {}

	//! This event is called to initialize the component before OnStep() is called.
	virtual void OnStart() = 0;
	//! This event is called to perform one step.
	virtual void OnStep() = 0;
	//! This event is called to cleanup a previously performed step.
	virtual void OnStepCleanup() = 0;
	//! This event is called to stop the component. No further OnStep() will be called before OnStart(). Note that this method does not necessarily need to reset everything - the next call to OnStart may continue at the same place unless the configuration has changed.
	virtual void OnStop() = 0;
	//! This event is called to reset the component. This should reset all internal variables.
	virtual void OnReloadConfiguration() = 0;
	//! This function must return an new object of its own class.
	virtual Component *Create() = 0;

	//! Writes the configuration to an XML element.
	void ConfigurationWriteXML(wxXmlNode *element, ErrorList *xmlerr);

	//! Returns whether the component is enabled or not.
	bool GetEnabled();

	//! Returns a boolean from the configuration.
	bool GetConfigurationBool(const std::string &key, bool defvalue);
	//! Returns an integer value from the configuration.
	int GetConfigurationInt(const std::string &key, int defvalue);
	//! Returns a double from the configuration.
	double GetConfigurationDouble(const std::string &key, double defvalue);
	//! Returns a string from the configuration.
	std::string GetConfigurationString(const std::string &key, const std::string &defvalue);

	//! Clears the status list
	void ClearStatus();

	//! Returns true if the component reads that data structure.
	bool HasDataStructureRead(DataStructure *ds);
	//! Returns true if the component writes that data structure.
	bool HasDataStructureWrite(DataStructure *ds);

	//! Returns a component by name.
	Display *GetDisplayByName(const std::string &name);

	//! Returns the SwisTrackCore object.
	SwisTrackCore *GetSwisTrackCore() {return mCore;}

protected:
	SwisTrackCore *mCore; 						//!< The associated SwisTrackCore object.
	tConfigurationMap mConfiguration;			//!< The configuration values.
	tConfigurationMap mConfigurationDefault;	//!< The default configuration values.
	bool mEnabled;								//!< Tells whether the component is enabled or not. During execution, disabled components act as if they were not in the list (i.e. none of OnStart, OnStep and OnStop is called).
	int mEditLocks;								//!< The number of edit locks.

	//! Adds an error message to the status list.
	void AddError(const std::string &msg);
	//! Adds a warning to the status list.
	void AddWarning(const std::string &msg);
	//! Adds an informational message to the status list.
	void AddInfo(const std::string &msg);

	//! Reads the XML file that belongs to the component.
	void Initialize();

	//! Registers a data structure that the component uses.
	void AddDataStructureRead(DataStructure *ds);
	//! Registers a data structure that the component modifies.
	void AddDataStructureWrite(DataStructure *ds);

	//! Registers a display image.
	void AddDisplay(Display *di);

	//! Increments the edit locks.
	bool IncrementEditLocks();
	//! Decrements the edit locks.
	void DecrementEditLocks();

	
private:
	//! Reads the configuration section of the XML file belonging to the component.
	void InitializeReadConfiguration(wxXmlNode *configurationnode);
	//! Reads a node within the configuration section of the XML file.
	void InitializeReadConfigurationNode(wxXmlNode *node);
	//! Reads a parameter node within the configuration section of the XML file.
	void InitializeReadParameter(wxXmlNode *node);
	
};

#endif
