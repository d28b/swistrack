#include "ConfigurationWriterXML.h"
#define THISCLASS ConfigurationWriterXML

#include <sstream>

THISCLASS::ConfigurationWriterXML():
		mErrorList(), mDocument(), mSelectedNode(0) {

	mSelectedNode=mDocument.create_root_node("swistrack");
}

void THISCLASS::WriteComponents(SwisTrackCore *stc) {
	SelectRootNode();
	xmlpp::Element *node=SelectNode("components");
	stc->ConfigurationWriteXML(node, &mErrorList);
	SelectRootNode();
}

xmlpp::Element *THISCLASS::SelectRootNode() {
	mSelectedNode=mDocument.get_root_node();
	return mSelectedNode;
}

xmlpp::Element *THISCLASS::SelectNode(const std::string &name) {
	// Return the first element (if any)
	xmlpp::Node::NodeList nodes=mSelectedNode->get_children(name);
	xmlpp::Node::NodeList::iterator it=nodes.begin();
	while (it!=nodes.end()) {
		xmlpp::Element *node=dynamic_cast<xmlpp::Element*>(*it);
		if (node) {mSelectedNode=node; return mSelectedNode;}
		it++;
	}

	// If no node was found, create one
	mSelectedNode=mSelectedNode->add_child(name);
	return mSelectedNode;
}

void THISCLASS::WriteString(const std::string &name, const std::string &value) {
	xmlpp::Element *node=mSelectedNode->add_child(name);
	node->set_child_text(value);
}

void THISCLASS::WriteBool(const std::string &name, bool value) {
	xmlpp::Element *node=mSelectedNode->add_child(name);
	std::ostringstream oss;
	oss << value;
	node->set_child_text(oss.str());
}

void THISCLASS::WriteInt(const std::string &name, int value) {
	xmlpp::Element *node=mSelectedNode->add_child(name);
	std::ostringstream oss;
	oss << value;
	node->set_child_text(oss.str());
}

void THISCLASS::WriteDouble(const std::string &name, double value) {
	xmlpp::Element *node=mSelectedNode->add_child(name);
	std::ostringstream oss;
	oss << value;
	node->set_child_text(oss.str());
}

bool THISCLASS::Save(const std::string &filename) {
	try {
		mDocument.write_to_file_formatted(filename);
	} catch (...) {
		return false;
	}

	return true;
}
