#include "ConfigurationWriterXML.h"
#define THISCLASS ConfigurationWriterXML

#include <sstream>

THISCLASS::ConfigurationWriterXML():
		mErrorList(), mDocument(), mSelectedNode(0) {

	mSelectedNode=new wxXmlNode(0, wxXML_ELEMENT_NODE, "swistrack");
	mDocument.SetRoot(mSelectedNode);
}

THISCLASS::~ConfigurationWriterXML() {
}

wxXmlNode *THISCLASS::SelectNode(wxXmlNode *node) {
	mSelectedNode=node;
	return mSelectedNode;
}

wxXmlNode *THISCLASS::GetRootNode() {
	return mDocument.GetRoot();
}

wxXmlNode *THISCLASS::HasChildNode(const wxString &name) {
	// Return the first element (if any)
	wxXmlNode *node=mSelectedNode->GetChildren();
	while (node) {
		if (node->GetName()==name) {
			return node;
		}
		node=node->GetNext();
	}
	return 0;
}

wxXmlNode *THISCLASS::GetChildNode(const wxString &name) {
	// Return the first element (if any)
	wxXmlNode *lastsibling=0;
	wxXmlNode *node=mSelectedNode->GetChildren();
	while (node) {
		if (node->GetName()==name) {
			return node;
		}
		lastsibling=node;
		node=node->GetNext();
	}

	// If no node was found, create one
	wxXmlNode *newnode=new wxXmlNode(0, wxXML_ELEMENT_NODE, name);
	if (lastsibling) {
		lastsibling->SetNext(newnode);
	} else {
		mSelectedNode->AddChild(newnode);
	}
	return newnode;
}

wxXmlNode *THISCLASS::GetParentNode() {
	wxXmlNode *node=mSelectedNode->GetParent();
	if (node) {return node;}
	return mSelectedNode;
}

void THISCLASS::WriteComponents(SwisTrackCore *stc) {
	SelectRootNode();
	wxXmlNode *node=GetChildNode("components");
	stc->ConfigurationWriteXML(node, &mErrorList);
}

void THISCLASS::WriteString(const wxString &name, const wxString &value) {
	wxXmlNode *newnode=new wxXmlNode(0, wxXML_TEXT_NODE, "", value);
	GetChildNode(name)->AddChild(newnode);
}

void THISCLASS::WriteBool(const wxString &name, bool value) {
	if (value) {
		WriteString(name, "true");
	} else {
		WriteString(name, "false");
	}
}

void THISCLASS::WriteInt(const wxString &name, int value) {
	WriteString(name, wxString::Format("%d", value));
}

void THISCLASS::WriteDouble(const wxString &name, double value) {
	WriteString(name, wxString::Format("%f", value));
}

bool THISCLASS::Save(const wxString &filename) {
	return mDocument.Save(filename);
}
