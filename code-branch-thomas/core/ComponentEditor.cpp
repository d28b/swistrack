#include "ComponentEditor.h"
#define THISCLASS ComponentEditor

#include <sstream>
#include <wx/xml/xml.h>

THISCLASS::ComponentEditor(Component *c): mComponent(0) {
	// Try to enter edit mode (this will always work for components)
	if (! c->IncrementEditLocks()) {return;}

	// Setup the editor; for safety reasons, the Component object is not set if we couldn't acquire an edit lock
	mComponent=c;
}

THISCLASS::~ComponentEditor() {
	if (! mComponent) {return;}
	mComponent->DecrementEditLocks();
}

void THISCLASS::ConfigurationReadXML(wxXmlNode *configuration, ErrorList *xmlerr) {
	if (! mComponent) {return;}
	mComponent->mConfiguration.clear();

	wxXmlNode *node=configuration->GetChildren();
	while (node) {
		if (node->GetName()=="parameter") {
			wxString name="";
			wxString value="";
			wxXmlProperty *prop=node->GetProperties();
			while (prop) {
				if (prop->GetName()=="name") {name=prop->GetValue();}
				if (prop->GetName()=="value") {value=prop->GetValue();}
				prop=prop->GetNext();
			}
			if ((name=="") && (value=="")) {
				std::ostringstream oss;
				oss << "A parameter of the component '" << mName << "' was ignored because either the attribute 'name' or 'value' are missing.";
				xmlerr->Add(oss.str(), 0);  // The current wxXml implementation doesn't care about line numbers.
			} else {
				mComponent->mConfiguration[name.c_str()]=value.c_str();
			}
		}

		node=node->GetNext();
	}
}

bool THISCLASS::SetConfigurationBool(const std::string &key, bool value) {
	if (! mComponent) {return;}
	std::ostringstream oss;
	oss << value;
	mComponent->mConfiguration[key]=oss.str();
	return true;
}

bool THISCLASS::SetConfigurationInt(const std::string &key, int value) {
	if (! mComponent) {return;}
	std::ostringstream oss;
	oss << value;
	mComponent->mConfiguration[key]=oss.str();
	return true;
}

bool THISCLASS::SetConfigurationDouble(const std::string &key, double value) {
	if (! mComponent) {return;}
	std::ostringstream oss;
	oss << value;
	mComponent->mConfiguration[key]=oss.str();
	return true;
}

bool THISCLASS::SetConfigurationString(const std::string &key, const std::string &value) {
	if (! mComponent) {return;}
	mComponent->mConfiguration[key]=value;
	return true;
}
