#ifndef HEADER_Component
#define HEADER_Component

class Component;

#include <string>
#include <list>
#include <libxml++/libxml++.h>
#include "StatusItem.h"
#include "SwisTrackCore.h"
#include "DataStructure.h"
#include "ComponentCategory.h"
#include "ErrorList.h"
#include "DisplayImage.h"

// The base class for all components.
class Component {

public:
	//! Status item list type.
	typedef std::list<StatusItem> tStatusItemList;
	//! The type for configuration values.
	typedef std::map<std::string, std::string> tConfigurationMap;
	//! DisplayImage list type.
	typedef std::list<DisplayImage*> tDisplayImageList;
	//! Data structure list type.
	typedef std::list<DataStructure*> tDataStructureList;

	tStatusItemList mStatus;		//!< The status item list (mostly list of errors).
	bool mStatusHasError;			//!< Whether there is an error in the status item list. (This only exists for performance reasons. A program could also go through the status list and check if there is an error message.)
	bool mStatusHasWarning;			//!< Whether there is a warning in the status item list. (This only exists for performance reasons. A program could also go through the status list and check if there is an error message.)
	bool mStarted;					//!< Tells whether the component is started. This flag is set and reset by the SwisTrackCore class and should not be used by the components.
	std::string mName;				//!< The internal name of the component (used to save configuration).
	std::string mDisplayName;		//!< The name that is displayed to the user.
	ComponentCategory *mCategory;	//!< The category which this component belongs to.
	std::string mDefaultView;		//!< The default view (not used).

	tDataStructureList mDataStructureRead;		//! The data structures that are read by this component.
	tDataStructureList mDataStructureWrite;		//! The data structures that are written by this component.
	tDisplayImageList mDisplayImages;			//! The DisplayImage objects that this component provides.

	//! Constructor.
	Component(SwisTrackCore *stc, const std::string &name);
	//! Destructor.
	virtual ~Component() {}
	
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

	//! Reads the configuration from an XML element.
	void ConfigurationReadXML(xmlpp::Element *element, ErrorList *xmlerr);
	//! Writes the configuration to an XML element.
	void ConfigurationWriteXML(xmlpp::Element *element, ErrorList *xmlerr);

	//! Returns a boolean from the configuration.
	bool GetConfigurationBool(const std::string &key, bool defvalue);
	//! Returns an integer value from the configuration.
	int GetConfigurationInt(const std::string &key, int defvalue);
	//! Returns a double from the configuration.
	double GetConfigurationDouble(const std::string &key, double defvalue);
	//! Returns a string from the configuration.
	std::string GetConfigurationString(const std::string &key, const std::string &defvalue);

	//! Sets a boolean value in the configuration.
	bool SetConfigurationBool(const std::string &key, bool value);
	//! Sets an integer value in the configuration.
	bool SetConfigurationInt(const std::string &key, int value);
	//! Sets a double in the configuration.
	bool SetConfigurationDouble(const std::string &key, double value);
	//! Sets a string in the configuration.
	bool SetConfigurationString(const std::string &key, const std::string &value);

	//! Clears the status list
	void ClearStatus();

	//! Returns true if the component reads that data structure.
	bool HasDataStructureRead(DataStructure *ds);
	//! Returns true if the component writes that data structure.
	bool HasDataStructureWrite(DataStructure *ds);

protected:
	SwisTrackCore *mCore; 				//!< The associated SwisTrackCore object.
	tConfigurationMap mConfiguration;	//!< The configuration values.

	//! Adds an error message to the status list.
	void AddError(const std::string &msg);
	//! Adds a warning to the status list.
	void AddWarning(const std::string &msg);
	//! Adds an informational message to the status list.
	void AddInfo(const std::string &msg);

	//! Registers a data structure that the component uses.
	void AddDataStructureRead(DataStructure *ds);
	//! Registers a data structure that the component modifies.
	void AddDataStructureWrite(DataStructure *ds);

	//! Registers a display image.
	void AddDisplayImage(DisplayImage *di);

	//! Returns a configuration element.
	xmlpp::Element* GetConfigurationElement(const std::string &name);
};

#endif
