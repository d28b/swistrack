#include "ConfigurationReaderXML.h"
#define THISCLASS ConfigurationReaderXML

#include <sstream>
#include "SwisTrackCoreEditor.h"

bool THISCLASS::Open(const std::string &filename) {
	// Read the file
	mDocument=0;
	try {
		mParser.parse_file(filename.c_str());
		if (mParser==true) {
			mDocument=mParser.get_document();
		}
	} catch (...) {
	}

	return IsOpen();
}

bool THISCLASS::ReadComponents(SwisTrackCore *stc) {
	SwisTrackCoreEditor stce(stc);
	if (! stce.IsEditable()) {return false;}

	// If this method is called without a document, set up an empty component list
	if (mDocument==0) {
		stce.ConfigurationReadXML(0, &mErrorList);
		return true;
	}

	// Get the root node
	xmlpp::Element *rootnode=mDocument->get_root_node();
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
	xmlpp::Element *components=dynamic_cast<xmlpp::Element *>(nodes.front());
	stce.ConfigurationReadXML(components, &mErrorList);
	return true;
}

int THISCLASS::ReadTriggerInterval(int defvalue) {
	// Get the trigger node
	xmlpp::Element *node=GetElement("trigger");
	if (! node) {return defvalue;}

	// Get the mode attribute
	xmlpp::Attribute *att_type=node->get_attribute("mode");
	if (! att_type) {
		mErrorList.Add("The trigger node requires a 'mode' attribute ('timer' or 'manual')!", node->get_line());
		return defvalue;
	}
	std::string type=att_type->get_value();

	// If we are in timer mode, return the interval
	if (type=="timer") {
		int interval=ReadInt("trigger/interval", 0);
		if (interval==0) {
			mErrorList.Add("The timer trigger requires a node 'interval' specifying the interval in milliseconds!", node->get_line());
		}
		return interval;
	} else if (type=="manual") {
		return defvalue;
	}

	// Anything else means error
	mErrorList.Add("Invalid trigger mode: valid modes are 'timer' and 'manual'!", node->get_line());
	return defvalue;
}

xmlpp::Element *THISCLASS::GetElement(const std::string &path) {
	if (! mDocument) {return 0;}

	// Get the root node
	xmlpp::Element *rootnode=mDocument->get_root_node();
	if (! rootnode) {return 0;}

	// Find the element in the tree
	xmlpp::NodeSet nodes=rootnode->find(path);

	// Return the first element (if any)
	xmlpp::NodeSet::iterator it=nodes.begin();
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

	xmlpp::TextNode *tn=node->get_child_text();
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
