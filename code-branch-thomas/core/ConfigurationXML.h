#ifndef HEADER_ConfigurationXML
#define HEADER_ConfigurationXML

class ConfigurationXML;

#include <wx/xml/xml.h>
#include "SwisTrackCore.h"

//! Wrapper around an XML node to read and write configuration values.
/*!
	This class provides a very easy to use interface to read and write configuration values to an XML node.
*/
class ConfigurationXML {

public:
	//! Constructor.
	ConfigurationXML(wxXmlNode *rootnode=0, bool readonly=true): mRootNode(rootnode), mSelectedNode(rootnode), mReadOnly(readonly) {}
	//! Destructor.
	~ConfigurationXML() {}

	//! Sets (and selects) a new root node.
	void SetRootNode(wxXmlNode *rootnode);

	//! Selects the root node.
	void SelectRootNode() {mSelectedNode=mRootNode;}
	//! Selects a child node of the current node. If the node doesn't exist, it is created unless the readonly flag is set.
	void SelectChildNode(const wxString &name) {mSelectedNode=GetChildNode(name);}
	//! Returns the root node.
	wxXmlNode *GetSelectedNode() {return mSelectedNode;}
	//! Returns the root node.
	wxXmlNode *GetRootNode() {return mRootNode;}
	//! Returns a child node of the current node. If the node doesn't exist, it is created unless the readonly flag is set.
	wxXmlNode *GetChildNode(const wxString &name);

	//! Reads a string of the selected node.
	wxString ReadContent(const wxString &defvalue="");
	//! Reads a string of a child node.
	wxString ReadContent(const wxString &childname, const wxString &defvalue="");
	//! Reads a property of the selected node.
	wxString ReadProperty(const wxString &name, const wxString &defvalue="");
	//! Reads a property of a child node.
	wxString ReadProperty(const wxString &childname, const wxString &name, const wxString &defvalue="");

	//! Writes a string value in the currently selected node.
	void WriteContent(const wxString &value);
	//! Writes a string value to a child node.
	void WriteContent(const wxString &childname, const wxString &value);
	//! Sets a node property.
	void WriteProperty(const wxString &name, const wxString &value);
	//! Sets a child node property.
	void WriteProperty(const wxString &childname, const wxString &name, const wxString &value);

	// Simple configuration interface: read methods
	//! Reads a string.
	wxString ReadString(const wxString &name, const wxString &defvalue) {return ReadContent(name, defvalue);}
	//! Reads a boolean value.
	bool ReadBool(const wxString &name, bool defvalue) {return Bool(ReadContent(name), defvalue);}
	//! Reads an integer.
	int ReadInt(const wxString &name, int defvalue) {return Int(ReadContent(name), defvalue);}
	//! Reads a double.
	double ReadDouble(const wxString &name, double defvalue) {return Double(ReadContent(name), defvalue);}

	// Simple configuration interface: write methods
	//! Writes a string value in the currently selected node.
	void WriteString(const wxString &name, const wxString &value) {WriteContent(name, value);}
	//! Writes a boolean value in the currently selected node.
	void WriteBool(const wxString &name, bool value) {WriteContent(name, Bool(value));}
	//! Writes an integer value in the currently selected node.
	void WriteInt(const wxString &name, int value) {WriteContent(name, Int(value));}
	//! Writes a double value in the currently selected node.
	void WriteDouble(const wxString &name, double value) {WriteContent(name, Double(value));}

protected:
	wxXmlNode *mRootNode;		//!< The root node. Note that this doesn't need to be the root node of a document. It is simply the node that this object regards as its root.
	wxXmlNode *mSelectedNode;	//!< The currently selected node.
	bool mReadOnly;				//!< The readonly flag. If this flag is set, nonexisting nodes are not created.
};

#endif
