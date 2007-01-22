#ifndef HEADER_SwisTrackCoreEditor
#define HEADER_SwisTrackCoreEditor

class SwisTrackCoreEditor;

#include "SwisTrackCore.h"

//! An editor for SwisTrackCore objects. If you want to modify the list of deployed components, you have to use this class.
class SwisTrackCoreEditor {

public:
	//! Constructor.
	SwisTrackCoreEditor(SwisTrackCore *stc);
	//! Destructor.
	~SwisTrackCoreEditor();

	//! Returns whether the SwisTrackCore object is editable or not.
	bool IsEditable() {return (mSwisTrackCore!=0);}

	//! Returns a component by name.
	Component *GetComponentByName(const std::string &name);

	//! Clears all deployed components.
	void Clear();
	//! Clears all deployed components.
	void Clear();
	//! Reads the configuration from a XML document.
	void ConfigurationReadXML(xmlpp::Element* configuration, ErrorList *xmlerr);

private:
	SwisTrackCore *mSwisTrackCore;		//!< The associated SwisTrackCore object.

	//! Reads one component from the XML file.
	void ConfigurationReadXMLElement(xmlpp::Element* element, ErrorList *xmlerr);

};

#endif
