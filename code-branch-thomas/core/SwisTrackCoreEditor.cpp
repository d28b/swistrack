#include "SwisTrackCoreEditor.h"
#define THISCLASS SwisTrackCoreEditor

#include <sstream>
#include "ComponentInputCamera1394.h"
#include "ComponentInputCameraUSB.h"
#include "ComponentInputCameraGBit.h"
#include "ComponentInputFileAVI.h"
#include "ComponentConvertToGray.h"
#include "ComponentConvertToBGR.h"
#include "ComponentBackgroundSubtractionGray.h"
#include "ComponentThresholdGray.h"
#include "ComponentBlobDetectionMinMax.h"
#include "ComponentBlobDetectionCircle.h"

THISCLASS::SwisTrackCoreEditor(SwisTrackCore *stc): mSwisTrackCore(0) {
	// Try to enter edit mode
	if (! stc->IncrementEditLocks()) {return;}

	// Setup the editor; for safety reasons, the SwisTrackCore object is not set if we couldn't acquire an edit lock
	mSwisTrackCore=stc;
}

THISCLASS::~SwisTrackCoreEditor() {
	if (! mSwisTrackCore) {return;}
	mSwisTrackCore->DecrementEditLocks();
}

SwisTrackCore::tComponentList *THISCLASS::GetComponentList() {
	if (! mSwisTrackCore) {return;}
	return &(mSwisTrackCore.mDeployedComponents);
}

void THISCLASS::Clear() {
	if (! mSwisTrackCore) {return;}

	tComponentList::iterator it=mSwisTrackCore->mDeployedComponents.begin();
	while (it!=mSwisTrackCore->mDeployedComponents.end()) {
		delete (*it);
		it++;
	}

	mSwisTrackCore->mDeployedComponents.clear();
}

void THISCLASS::ConfigurationReadXML(xmlpp::Element* configuration, ErrorList *xmlerr) {
	if (! mSwisTrackCore) {return;}

	// Clear the current list of components
	Clear();

	// Traverse the list and create a Component object for each "component" tag
	if (! configuration) {return;}
	xmlpp::Node::NodeList list=configuration->get_children("component");
	xmlpp::Node::NodeList::iterator it=list.begin();
	while (it!=list.end()) {
		xmlpp::Element *element=dynamic_cast<xmlpp::Element *>(*it);
		ConfigurationReadXMLElement(element, xmlerr);
		it++;
	}
}

void THISCLASS::ConfigurationReadXMLElement(xmlpp::Element* element, ErrorList *xmlerr) {
	if (! element) {return;}

	// Get the type attribute
	xmlpp::Attribute *att_type=element->get_attribute("type");
	if (! att_type) {
		std::ostringstream oss;
		oss << "The component at line " << element->get_line() << " was ignored because it does not have a 'type' attribute.";
		xmlerr->Add(oss.str(), element->get_line());
	}

	// Search for the component
	std::string type=att_type->get_value();
	Component *component=GetComponentByName(type);
	if (! component) {
		std::ostringstream oss;
		oss << "The component at line " << element->get_line() << " was ignored because there is no component called '" << type << "'.";
		xmlerr->Add(oss.str(), element->get_line());
	}

	// Add it to the list
	Component *newcomponent=component->Create();
	mSwisTrackCore->mDeployedComponents.push_back(newcomponent);
	newcomponent->ConfigurationReadXML(element, xmlerr);
}
