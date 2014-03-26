#ifndef HEADER_SwisTrackCoreEditor
#define HEADER_SwisTrackCoreEditor

class SwisTrackCoreEditor;

#include <wx/xml/xml.h>
#include "SwisTrackCore.h"

//! An editor for SwisTrackCore objects. If you want to modify the list of deployed components, you have to use this class.
class SwisTrackCoreEditor {

public:
	//! Constructor.
	SwisTrackCoreEditor(SwisTrackCore *stc);
	//! Destructor.
	~SwisTrackCoreEditor();

	//! Returns whether the SwisTrackCore object is editable or not.
	bool IsEditable() {
		return (mSwisTrackCore != 0);
	}

	//! Returns the list of deployed components. Note that this list may only be edited as long as the SwisTrackCoreEditor object exists.
	SwisTrackCore::tComponentList *GetDeployedComponents();
	//! Clears all deployed components.
	void Clear();
	//! Reads the configuration from a XML document.
	void ConfigurationReadXML(wxXmlNode* configuration, ErrorList *xmlerr);

private:
	SwisTrackCore *mSwisTrackCore;		//!< The associated SwisTrackCore object.

	//! Reads one component from the XML file.
	void ConfigurationReadXMLElement(wxXmlNode* element, ErrorList *xmlerr);

};

#endif
