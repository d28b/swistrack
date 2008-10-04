#include "SwisTrackCoreEditor.h"
#define THISCLASS SwisTrackCoreEditor

#include <wx/xml/xml.h>
#include "ComponentEditor.h"

THISCLASS::SwisTrackCoreEditor(SwisTrackCore *stc): mSwisTrackCore(0) {
	// Try to enter edit mode
	if (! stc->IncrementEditLocks()) {
		return;
	}

	// Setup the editor; for safety reasons, the SwisTrackCore object is not set if we couldn't acquire an edit lock
	mSwisTrackCore = stc;
}

THISCLASS::~SwisTrackCoreEditor() {
	if (! mSwisTrackCore) {
		return;
	}
	mSwisTrackCore->DecrementEditLocks();
}

SwisTrackCore::tComponentList *THISCLASS::GetDeployedComponents() {
	if (! mSwisTrackCore) {
		return 0;
	}
	return &(mSwisTrackCore->mDeployedComponents);
}

void THISCLASS::Clear() {
	if (! mSwisTrackCore) {
		return;
	}

	SwisTrackCore::tComponentList::iterator it = mSwisTrackCore->mDeployedComponents.begin();
	while (it != mSwisTrackCore->mDeployedComponents.end()) {
		delete (*it);
		it++;
	}

	mSwisTrackCore->mDeployedComponents.clear();
}

void THISCLASS::ConfigurationReadXML(wxXmlNode* configuration, ErrorList *xmlerr) {
	if (! mSwisTrackCore) {
		return;
	}

	// Clear the current list of components
	Clear();

	// Traverse the list and create a Component object for each "component" tag
	if (! configuration) {
		return;
	}
	wxXmlNode *node = configuration->GetChildren();
	while (node) {
		if (node->GetName() == wxT("component")) {
			ConfigurationReadXMLElement(node, xmlerr);
		}
		node = node->GetNext();
	}
}

void THISCLASS::ConfigurationReadXMLElement(wxXmlNode* node, ErrorList *xmlerr) {
	if (! node) {
		return;
	}

	// Get the type attribute
	wxString type;
	wxXmlProperty *prop = node->GetProperties();
	while (prop) {
		if (prop->GetName() == wxT("type")) {
			type = prop->GetValue();
		}
		prop = prop->GetNext();
	}
	if (type.Len() == 0) {
		xmlerr->Add(wxT("A component was ignored because it does not have a 'type' attribute."), 0);
		return;
	}

	// Search for the component
	Component *component = mSwisTrackCore->GetComponentByName(type);
	if (! component) {
		xmlerr->Add(wxT("The component '") + type + wxT("' was ignored because there is no component with this name."), 0);
		return;
	}

	// Add it to the list
	Component *newcomponent = component->Create();
	mSwisTrackCore->mDeployedComponents.push_back(newcomponent);
	ComponentEditor ce(newcomponent);
	ce.ConfigurationReadXML(node, xmlerr);
}
