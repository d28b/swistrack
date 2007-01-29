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
	xmlpp::Node::NodeList nodes=rootnode->get_children("components");
	if (nodes.empty()) {
		stce.ConfigurationReadXML(0, &mErrorList);
		return true;
	}

	// Read the component list
	xmlpp::Element *components=dynamic_cast<xmlpp::Element *>(nodes_components.front());
	stce.ConfigurationReadXML(components, &mErrorList);
}

int THISCLASS::ReadTriggerInterval() {
	// Get the trigger node
	xmlpp::Element *node=GetElement("trigger");
	if (! node) {return 0;}

	// Get the mode attribute
	xmlpp::Attribute *att_type=node->get_attribute("mode");
	if (! att_type) {
		errorlist->Add("The trigger node requires a 'mode' attribute ('timer' or 'manual')!", node->get_line());
		return 0;
	}
	std::string type=att_type->get_value();

	// If we are in timer mode, return the interval
	if (type=="timer") {
		int interval=ReadInt("trigger/interval", 0);
		if (interval==0) {
			errorlist->Add("The timer trigger requires a node 'interval' specifying the interval in milliseconds!", node->get_line());
		}
		return interval;
	} else if (type=="manual") {
		return 0;
	}

	// Anything else means error
	errorlist->Add("Invalid trigger mode: valid modes are 'timer' and 'manual'!", node->get_line());
	return 0;
}

xmlpp::Element *THISCLASS::GetElement(const std::string &path) {
	if (! mDocument) {return 0;}

	// Get the root node
	xmlpp::Element *rootnode=document->get_root_node();
	if (! rootnode) {return 0;}

	// Find the element in the tree
	xmlpp::Node::NodeList nodes=rootnode->find(path);

	// Return the first element (if any)
	xmlpp::Node::NodeList::iterator it=nodes.begin();
	while (it!=nodes.end()) {
		xmlpp::Element *node=dynamic_cast<xmlpp::Element*>(*it);
		if (node) {return node;}
		it++;
	}

	return 0;
}

std::string THISCLASS::ReadString(const std::string &xpath, const std::string &defvalue) {
	xmlpp::Element *node=GetElement(xpath);
	if (! node) {return defvalue;}

	xmlpp::TextNode *tn=node.get_child_text();
	if (! tn) {return defvalue;}
	return tn->get_content();
}

bool THISCLASS::ReadBool(const std::string &xpath, bool defvalue) {
	std::string str=ReadString(xpath, "");
	if (str=="") {return defvalue;}

	std::istringstream iss(str);
	iss >> defvalue;
	return defvalue;
}

int THISCLASS::ReadInt(const std::string &xpath, int defvalue) {
	std::string str=ReadString(xpath, "");
	if (str=="") {return defvalue;}

	std::istringstream iss(str);
	iss >> defvalue;
	return defvalue;
}

double THISCLASS::ReadDouble(const std::string &xpath, double defvalue) {
	std::string str=ReadString(xpath, "");
	if (str=="") {return defvalue;}

	std::istringstream iss(str);
	iss >> defvalue;
	return defvalue;
}
