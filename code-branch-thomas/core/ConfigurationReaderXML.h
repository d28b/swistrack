#ifndef HEADER_ConfigurationReaderXML
#define HEADER_ConfigurationReaderXML

class ConfigurationReaderXML;

#include <wx/xml/xml.h>
#include "SwisTrackCore.h"

//! Reads a SwisTrack configuration from an XML file.
/*!
	Create this an object of this class on the stack and call PrepareRead(). If this returns true, you can call ReadComponents(), ReadTriggerInterval(), ... to read the file. (If the latter methods are called without any open document, the default values are returned.)
*/
class ConfigurationReaderXML {

public:
	ErrorList mErrorList;	//!< Error messages are added here.

	//! Constructor.
	ConfigurationReaderXML(): mErrorList(), mDocument(), mIsOpen(false), mSelectedNode(0) {}
	//! Destructor.
	~ConfigurationReaderXML() {}

	//! Opens an XML document. If this function returns true, the document can be read.
	bool Open(const wxString &filename);
	//! Returns whether a document is open.
	bool IsOpen() {return mIsOpen;}

	//! Selects the root node.
	wxXmlNode *SelectNode(wxXmlNode *node);
	//! Selects the root node.
	wxXmlNode *SelectRootNode() {return SelectNode(GetRootNode());}
	//! Selects a child node of the current node. If this node doesn't exist, it is created.
	wxXmlNode *SelectChildNode(const wxString &name) {return SelectNode(GetChildNode(name));}
	//! Selects the parent node of the current node.
	wxXmlNode *SelectParentNode() {return SelectNode(GetParentNode());}
	//! Returns the root node.
	wxXmlNode *GetRootNode();
	//! Returns a child node of the current node or 0 if no such child exists.
	wxXmlNode *HasChildNode(const wxString &name);
	//! Returns a child node of the current node. If this node doesn't exist, it is created.
	wxXmlNode *GetChildNode(const wxString &name);
	//! Returns the parent node of the current node.
	wxXmlNode *GetParentNode();

	//! Reads the components and uses the supplied SwisTrackCoreEditor to set them. If no document is open, an empty component list is created and the return value will be true. If the return value is false, the SwisTrackCore object was not editable.
	bool ReadComponents(SwisTrackCore *stc);

	//! Reads a string.
	wxString ReadString(const wxString &xpath, const wxString &defvalue);
	//! Reads a boolean value.
	bool ReadBool(const wxString &xpath, bool defvalue);
	//! Reads an integer.
	int ReadInt(const wxString &xpath, int defvalue);
	//! Reads a double.
	double ReadDouble(const wxString &xpath, double defvalue);

protected:
	wxXmlDocument mDocument;		//!< The document.
	bool mIsOpen;					//!< Whether the document is open or not.
	wxXmlNode *mSelectedNode;		//!< The currently selected node.

};

#endif
