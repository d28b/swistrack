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

wxString THISCLASS::ReadContent(const wxString &defvalue) {
	if (! mSelectedNode) {return defvalue;}
	return mSelectedNode->GetNodeContent();
}

wxString THISCLASS::ReadChildContent(const wxString &childname, const wxString &defvalue) {
	wxXmlNode *node=GetChildNode(childname);
	if (! node) {return defvalue;}
	return node->GetNodeContent();
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

wxString THISCLASS::ReadChildProperty(const wxString &childname, const wxString &name, const wxString &defvalue) {
	wxXmlNode *node=GetChildNode(childname);
	if (! node) {return defvalue;}
	wxXmlProperty *prop=node->GetProperties();
	while (prop) {
		if (prop->GetName()==name) {return prop->GetValue();}
		prop=prop->GetNext();
	}
	return defvalue;
}

void THISCLASS::WriteContent(const wxString &value) {
	if (mReadOnly) {return;}
	if (! mSelectedNode) {return;}
	wxXmlNode *newnode=new wxXmlNode(0, wxXML_TEXT_NODE, "", value);
	mSelectedNode->AddChild(newnode);
}

void THISCLASS::WriteChildContent(const wxString &childname, const wxString &value) {
	if (mReadOnly) {return;}
	wxXmlNode *childnode=GetChildNode(childname);
	if (! childnode) {return;}
	wxXmlNode *newnode=new wxXmlNode(0, wxXML_TEXT_NODE, "", value);
	childnode->AddChild(newnode);
}

void THISCLASS::WriteProperty(const wxString &name, const wxString &value) {
	if (mReadOnly) {return;}
	if (! mSelectedNode) {return;}
	mSelectedNode->AddProperty(name, value);
}

void THISCLASS::WriteChildProperty(const wxString &childname, const wxString &name, const wxString &value) {
	if (mReadOnly) {return;}
	wxXmlNode *childnode=GetChildNode(childname);
	if (! childnode) {return;}
	childnode->AddProperty(name, value);
}

bool THISCLASS::Bool(const wxString &str, bool defvalue) {
	if (str=="") {return defvalue;}

	wxString strlc=str.Lower();
	if (strlc=="true") {return true;}
	if (strlc=="false") {return false;}
	long value=(defvalue ? 1 : 0);
	str.ToLong(&value);
	return (value!=0);
}

int THISCLASS::Int(const wxString &str, int defvalue) {
	if (str=="") {return defvalue;}

	long value=(long)defvalue;
	str.ToLong(&value);
	return value;
}

double THISCLASS::Double(const wxString &str, double defvalue) {
	if (str=="") {return defvalue;}

	str.ToDouble(&defvalue);
	return defvalue;
}

wxString THISCLASS::Bool(bool value) {
	if (value) {
		return "true";
	} else {
		return "false";
	}
}

wxString THISCLASS::Int(int value) {
	return wxString::Format("%d", value);
}

wxString THISCLASS::Double(double value) {
	return wxString::Format("%f", value);
}

