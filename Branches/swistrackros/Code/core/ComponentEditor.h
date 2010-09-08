#ifndef HEADER_ComponentEditor
#define HEADER_ComponentEditor

class ComponentEditor;

#include <wx/xml/xml.h>
#include "Component.h"

//! An editor for Component objects. If you want to modify the list of deployed components, you have to use this class.
class ComponentEditor {

public:
	//! Constructor.
	ComponentEditor(Component *stc);
	//! Destructor.
	~ComponentEditor();

	//! Returns whether the Component object is editable or not.
	bool IsEditable() {
		return (mComponent != 0);
	}

	//! Reads the configuration from an XML element.
	void ConfigurationReadXML(wxXmlNode *element, ErrorList *xmlerr);

	//! Sets the enabled interval of the component.
	bool SetEnabledInterval(int value);

	//! Sets a boolean value in the configuration.
	bool SetConfigurationBool(const wxString &key, bool value);
	//! Sets an integer value in the configuration.
	bool SetConfigurationInt(const wxString &key, int value);
	//! Sets a double in the configuration.
	bool SetConfigurationDouble(const wxString &key, double value);
	//! Sets a string in the configuration.
	bool SetConfigurationString(const wxString &key, const wxString &value);
	//! Sets a date in the configuration.
	bool SetConfigurationDate(const wxString &key, const wxDateTime &value);
	
	bool SetConfigurationColor(const wxString &key, const wxColor &value);

private:
	Component *mComponent;		//!< The associated Component object.

	//! Reads one component from the XML file.
	void ConfigurationReadXMLElement(wxXmlNode* element, ErrorList *xmlerr);

};

#endif
