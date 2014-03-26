#ifndef HEADER_Component
#define HEADER_Component

class Component;
#include <wx/wx.h>
#include <wx/string.h>
#include <wx/xml/xml.h>
#include <wx/colour.h>
#include <list>
#include <map>
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
	typedef std::map<wxString, wxString> tConfigurationMap;
	//! Display list type.
	typedef std::list<Display*> tDisplayList;
	//! Data structure list type.
	typedef std::list<DataStructure*> tDataStructureList;

	tStatusItemList mStatus;			//!< The status item list (mostly list of errors).
	bool mStatusHasError;				//!< Whether there is an error in the status item list. (This only exists for performance reasons. A program could also go through the status list and check if there is an error message.)
	bool mStatusHasWarning;				//!< Whether there is a warning in the status item list. (This only exists for performance reasons. A program could also go through the status list and check if there is an error message.)
	bool mStarted;						//!< Tells whether the component is started. This flag is set and reset by the SwisTrackCore class and should not be used by the components.
	wxString mName;					//!< The internal name of the component (used to save configuration).
	wxString mDisplayName;			//!< The name that is displayed to the user.
	wxString mDescription;			//!< The description.
	wxString mHelpURL;				//!< The url pointing to the website with more information about the component.
	ComponentCategory *mCategory;		//!< The category which this component belongs to.
	wxString mDefaultDisplay;		//!< The default view (not used).
	ComponentTrigger *mTrigger;			//!< The trigger of this component (if any).
	ErrorList mInitializationErrors;	//!< Error messages (during component initialization) are added here.
	double mStepDuration;				//!< The duration of the last step in milliseconds. This information is filled in by SwisTrackCore.

	tDataStructureList mDataStructureRead;		//! The data structures that are read by this component.
	tDataStructureList mDataStructureWrite;		//! The data structures that are written by this component.
	tDisplayList mDisplays;						//! The Display objects that this component provides.

	//! Constructor.
	Component(SwisTrackCore *stc, const wxString &name);
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

	//! Returns the enabled interval.
	int GetEnabledInterval();

	//! Returns a boolean from the configuration.
	bool GetConfigurationBool(const wxString &key, bool defvalue);
	//! Returns an integer value from the configuration.
	int GetConfigurationInt(const wxString &key, int defvalue);
	//! Returns a double from the configuration.
	double GetConfigurationDouble(const wxString &key, double defvalue);
	//! Returns a string from the configuration.
	wxString GetConfigurationString(const wxString &key, const wxString &defvalue);
	//! Returns a timestamp from the configuration.
	wxDateTime GetConfigurationDate(const wxString &key, const wxDateTime &defvalue);
	//! Returns a color from the configuration.
	wxColour GetConfigurationColor(const wxString &key, const wxColour &defvalue);

	//! Clears the status list
	void ClearStatus();

	//! Returns true if the component reads that data structure.
	bool HasDataStructureRead(DataStructure *ds);
	//! Returns true if the component writes that data structure.
	bool HasDataStructureWrite(DataStructure *ds);

	//! Returns a component by name.
	Display *GetDisplayByName(const wxString &name);

	//! Returns the SwisTrackCore object.
	SwisTrackCore *GetSwisTrackCore() {
		return mCore;
	}

protected:
	SwisTrackCore *mCore; 						//!< The associated SwisTrackCore object.
	tConfigurationMap mConfiguration;			//!< The configuration values.
	tConfigurationMap mConfigurationDefault;	//!< The default configuration values.
	int mEnabledInterval;						//!< Allows only every Nth frame to be processed by this component. During all other steps, the component's OnStep function is not called. Defaults to 1 (process every frame). If set to 0, the OnStep function is never called (but OnStart and OnStop are).
	int mEditLocks;								//!< The number of edit locks.

	//! Adds an error message to the status list.
	void AddError(const wxString &msg);
	//! Adds a warning to the status list.
	void AddWarning(const wxString &msg);
	//! Adds an informational message to the status list.
	void AddInfo(const wxString &msg);

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
