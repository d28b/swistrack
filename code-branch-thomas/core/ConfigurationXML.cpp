#include "ConfigurationXML.h"
#define THISCLASS ConfigurationXML

#include <sstream>

void THISCLASS::SetRootNode(wxXmlNode *rootnode) {
	mRootNode=rootnode;
	mSelectedNode=rootnode;
}

wxXmlNode *THISCLASS::GetChildNode(const wxString &name) {
	if (! mSelectedNode) {return 0;}

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

	// No node was found: return 0 if the readonly flag is set
	if (mReadOnly) {return 0;}
	
	// Otherwise, create a node
	wxXmlNode *newnode=new wxXmlNode(0, wxXML_ELEMENT_NODE, name);
	if (lastsibling) {
		lastsibling->SetNext(newnode);
	} else {
		mSelectedNode->AddChild(newnode);
	}
	return newnode;
}

wxString THISCLASS::ReadString(const wxString &name, const wxString &defvalue) {
	wxXmlNode *node=GetChildNode(name);
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

wxString THISCLASS::ReadProperty(const wxString &name, const wxString &defvalue) {
	if (! mSelectedNode) {return defvalue;}
	wxXmlProperty *prop=mSelectedNode->GetProperties();
	while (prop) {
		if (prop->GetName()==name) {return prop->GetValue();}
		prop=prop->GetNext();
	}
	return defvalue;
}

wxString THISCLASS::ReadProperty(const wxString &childname, const wxString &name, const wxString &defvalue) {
	wxXmlNode *node=GetChildNode(childname);
	if (! node) {return defvalue;}
	wxXmlProperty *prop=node->GetProperties();
	while (prop) {
		if (prop->GetName()==name) {return prop->GetValue();}
		prop=prop->GetNext();
	}
	return defvalue;
}

bool THISCLASS::Bool() {
}

void THISCLASS::WriteString(const wxString &name, const wxString &value) {
	if (mReadOnly) {return;}
	wxXmlNode *childnode=GetChildNode(name);
	if (! childnode) {return;}
	wxXmlNode *newnode=new wxXmlNode(0, wxXML_TEXT_NODE, "", value);
	childnode->AddChild(newnode);
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
