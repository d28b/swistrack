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

	//! Reads a string.
	wxString ReadString(const wxString &xpath, const wxString &defvalue);
	//! Reads a boolean value.
	bool ReadBool(const wxString &xpath, bool defvalue);
	//! Reads an integer.
	int ReadInt(const wxString &xpath, int defvalue);
	//! Reads a double.
	double ReadDouble(const wxString &xpath, double defvalue);

	//! Writes a string value in the currently selected node.
	void WriteString(const wxString &name, const wxString &value);
	//! Writes a boolean value in the currently selected node.
	void WriteBool(const wxString &name, bool value);
	//! Writes an integer value in the currently selected node.
	void WriteInt(const wxString &name, int value);
	//! Writes a double value in the currently selected node.
	void WriteDouble(const wxString &name, double value);

protected:
	wxXmlNode *mRootNode;		//!< The root node. Note that this doesn't need to be the root node of a document. It is simply the node that this object regards as its root.
	wxXmlNode *mSelectedNode;	//!< The currently selected node.
	bool mReadOnly;				//!< The readonly flag. If this flag is set, nonexisting nodes are not created.
};

#endif
