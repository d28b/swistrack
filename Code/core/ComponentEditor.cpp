#include "ComponentEditor.h"
#define THISCLASS ComponentEditor

#include "ConfigurationConversion.h"
#include <wx/xml/xml.h>

THISCLASS::ComponentEditor(Component *c): mComponent(0) {
	// Try to enter edit mode (this will always work for components)
	if (! c->IncrementEditLocks()) {
		return;
	}

	// Setup the editor; for safety reasons, the Component object is not set if we couldn't acquire an edit lock
	mComponent = c;
}

THISCLASS::~ComponentEditor() {
	if (! mComponent) {
		return;
	}
	mComponent->DecrementEditLocks();
}

void THISCLASS::ConfigurationReadXML(wxXmlNode *configuration, ErrorList *xmlerr) {
	if (! mComponent) {
		return;
	}

	// Default settings
	mComponent->mConfiguration.clear();
	mComponent->mEnabledInterval = 1;
	
	// Read all nodes of the XML node belonging to the component
	wxXmlNode *node = configuration->GetChildren();
	while (node) {
		if (node->GetName() == wxT("enabledinterval")) {
			wxXmlProperty *prop = node->GetProperties();
			while (prop) {
				if (prop->GetName() == wxT("value")) {
					mComponent->mEnabledInterval = ConfigurationConversion::Int(prop->GetValue(), mComponent->mEnabledInterval);
				}
				prop = prop->GetNext();
			}
		} else if (node->GetName() == wxT("parameter")) {
			wxString name = wxT("");
			wxString value = wxT("");
			wxXmlProperty *prop = node->GetProperties();
			while (prop) {
				if (prop->GetName() == wxT("name")) {
					name = prop->GetValue();
				}
				if (prop->GetName() == wxT("value")) {
					value = prop->GetValue();
				}
				prop = prop->GetNext();
			}
			if ((name.Len() == 0) && (value.Len() == 0)) {
				xmlerr->Add(wxT("A parameter of the component \'") + mComponent->mName + wxT("\' was ignored because either the attribute \'name\' or \'value\' are missing."), 0);  // The current wxXml implementation doesn\'t care about line numbers.
			} else {
				mComponent->mConfiguration[name] = value;
			}
		}

		node = node->GetNext();
	}
}

bool THISCLASS::SetEnabledInterval(int value) {
	if (! mComponent) {
		return false;
	}

	mComponent->mEnabledInterval = value;
	return true;
}

bool THISCLASS::SetConfigurationBool(const wxString &key, bool value) {
	if (! mComponent) {
		return false;
	}
	mComponent->mConfiguration[key] = ConfigurationConversion::Bool(value);
	return true;
}

bool THISCLASS::SetConfigurationInt(const wxString &key, int value) {
	if (! mComponent) {
		return false;
	}
	mComponent->mConfiguration[key] = ConfigurationConversion::Int(value);
	return true;
}

bool THISCLASS::SetConfigurationDouble(const wxString &key, double value) {
	if (! mComponent) {
		return false;
	}
	mComponent->mConfiguration[key] = ConfigurationConversion::Double(value);
	return true;
}

bool THISCLASS::SetConfigurationString(const wxString &key, const wxString &value) {
	if (! mComponent) {
		return false;
	}
	mComponent->mConfiguration[key] = value;
	return true;
}

bool THISCLASS::SetConfigurationDate(const wxString &key, const wxDateTime &value) {
	if (! mComponent) {
		return false;
	}
	mComponent->mConfiguration[key] = ConfigurationConversion::Date(value);
	return true;
}


bool THISCLASS::SetConfigurationColor(const wxString &key, const wxColor &value) {
	if (! mComponent) {
		return false;
	}
	mComponent->mConfiguration[key] = ConfigurationConversion::Color(value);
	return true;
}
