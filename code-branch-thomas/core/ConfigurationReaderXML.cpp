#include "ConfigurationReaderXML.h"
#define THISCLASS ConfigurationReaderXML

#include <sstream>
#include <wx/log.h>
#include "SwisTrackCoreEditor.h"

bool THISCLASS::Open(const wxString &filename) {
	// Read the file
	wxLogNull log;
	mIsOpen=mDocument.Load(filename);
	if (! mIsOpen) {
		mDocument.SetRoot(new wxXmlNode(0, wxXML_ELEMENT_NODE, "swistrack"));
		SelectRootNode();
		return false;
	}

	// Select the root element and check its name
	SelectRootNode();
	if (mSelectedNode->GetName() != "swistrack") {
		mIsOpen=false;
		return false;
	}

	return true;
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

bool THISCLASS::ReadComponents(SwisTrackCore *stc) {
	SwisTrackCoreEditor stce(stc);
	if (! stce.IsEditable()) {return false;}

	// Read the component list
	SelectRootNode();
	stce.ConfigurationReadXML(GetChildNode("components"), &mErrorList);
	return true;
}

wxString THISCLASS::ReadString(const wxString &name, const wxString &defvalue) {
	wxXmlNode *node=HasChildNode(name);
	if (! node) {return defvalue;}
	return node->GetNodeContent();
}

bool THISCLASS::ReadBool(const wxString &name, bool defvalue) {
	wxString str=ReadString(name, "");
	if (str=="") {return defvalue;}

	str.MakeLower();
	if (str=="true") {return true;}
	if (str=="false") {return false;}
	long value=(defvalue ? 1 : 0);
	str.ToLong(&value);
	return (value!=0);
}

int THISCLASS::ReadInt(const wxString &name, int defvalue) {
	wxString str=ReadString(name, "");
	if (str=="") {return defvalue;}

	long value=(long)defvalue;
	str.ToLong(&value);
	return value;
}

double THISCLASS::ReadDouble(const wxString &name, double defvalue) {
	wxString str=ReadString(name, "");
	if (str=="") {return defvalue;}

	str.ToDouble(&defvalue);
	return defvalue;
}
