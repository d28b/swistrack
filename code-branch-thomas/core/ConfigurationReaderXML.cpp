#include "ConfigurationReaderXML.h"
#define THISCLASS ConfigurationReaderXML

bool THISCLASS::PrepareRead(const std::string &filename) {
	// Read the file
	xmlpp::DomParser parser;
	xmlpp::Document *document=0;
	try {
		parser.parse_file(filename.c_str());
		if (parser==true) {
			document=parser.get_document();
		}
	} catch (...) {
		document=0;
	}

	return IsOpen();
}

bool THISCLASS::ReadComponents(SwisTrackCore *stc) {
	SwisTrackCoreEditor stce(stc);
	if (! stce->IsEditable()) {return false;}

	// If this method is called without a document, set up an empty component list
	if (document==0) {
		stce.ConfigurationReadXML(0, &mErrorList);
		return true;
	}

	// Get the root node
	xmlpp::Element *rootnode=document->get_root_node();
	if (! rootnode) {
		stce.ConfigurationReadXML(0, &mErrorList);
		return true;
	}

	// Get the list of components
	xmlpp::Node::NodeList nodes=rootnode->get_children("Components");
	if (nodes.empty()) {
		stce.ConfigurationReadXML(0, &mErrorList);
		return true;
	}

	// Read the component list
	xmlpp::Element *components=dynamic_cast<xmlpp::Element *>(nodes_components.front());
	stce.ConfigurationReadXML(components, &mErrorList);
}


int THISCLASS::ReadTriggerInterval() {
	if (! mDocument) {return 0;}

	// Get the root node
	xmlpp::Element *rootnode=document->get_root_node();
	if (! rootnode) {return 0;}

	// Get the mode attribute
	xmlpp::Attribute *att_type=element->get_attribute("mode");
	if (! att_type) {
		xmlerr->Add("The 'mode' attribute of the node 'ConfigurationReaderXML' is missing.", element->get_line());
		return;
	}
	std::string type=att_type->get_value();

	// If we are in timer mode, return the interval
	if (type=="timer") {
		xmlpp::Node::NodeList nodes=rootnode->get_children("Interval");
		if (nodes.empty()) {
			errorlist->Add("The interval for the timer ConfigurationReaderXML is not specified!", rootnode->get_line());
			return 0;
		}
		xmlpp::TextNode *tn=dynamic_cast<xmlpp::TextNode*>(nodes.front());
		std::istringstream iss(tn->get_content());
		iss >> 
	} else if (type=="manual") {
		return 0;
	}

	// Anything else means error
	errorlist->Add("The interval for the timer ConfigurationReaderXML is not specified!", rootnode->get_line());
}

int THISCLASS::ReadBool(const std::string &path, bool defvalue) {
	xmlpp::Node::NodeList nodes=rootnode->get_children("Interval");
	if (nodes.empty()) {
		errorlist->Add("The interval for the timer ConfigurationReaderXML is not specified!", rootnode->get_line());
		return 0;
	}

	xmlpp::TextNode *tn=dynamic_cast<xmlpp::TextNode*>(nodes.front());
	std::istringstream iss(tn->get_content());
	iss >> 

}
