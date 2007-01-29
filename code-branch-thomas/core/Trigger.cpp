#include "Trigger.h"
#define THISCLASS Trigger

void THISCLASS::ConfigurationReadXML(xmlpp::Element *element, ErrorList *errorlist) {
	if (! element) {return;}

	// Get the mode attribute
	xmlpp::Attribute *att_type=element->get_attribute("mode");
	if (! att_type) {
		xmlerr->Add("The 'mode' attribute of the node 'Trigger' is missing.", element->get_line());
		return;
	}
	std::string type=att_type->get_value();

	// Read the parameters
	if (type=="timer") {
		xmlpp::Node::NodeList nodes_interval=rootnode->get_children("Interval");
		if (nodes_trigger.empty()) {
			errorlist->Add("The interval for the timer trigger is not specified!", rootnode->get_line());
			return;
		}
	}

}

void THISCLASS::ConfigurationWriteXML(xmlpp::Element *element, ErrorList *xmlerr) {
	if (! element) {return;}

	// Write
	element->

	// Add an element for each component
	tComponentList::iterator it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		xmlpp::Element *element=configuration->add_child("component");
		element->set_attribute("type", (*it)->mName);
		(*it)->ConfigurationWriteXML(element, xmlerr);

		it++;
	}
}
