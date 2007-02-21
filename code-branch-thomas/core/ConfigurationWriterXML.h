#ifndef HEADER_ConfigurationWriterXML
#define HEADER_ConfigurationWriterXML

class ConfigurationWriterXML;

#include <wx/xml/xml.h>
#include "SwisTrackCore.h"
#include "ErrorList.h"

//! Read.
class ConfigurationWriterXML {

public:
	ErrorList mErrorList;	//!< Error messages are added here.

	//! Constructor.
	ConfigurationWriterXML();
	//! Destructor.
	~ConfigurationWriterXML();

	//! Opens an XML document. If this function returns true, the document can be read.
	bool Save(const wxString &filename);

	//! Returns the XML document. (This function is not needed unless the application needs to access the XML document in a different way.)
	wxXmlDocument *GetDocument() {return &mDocument;}

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

	//! Writes the components.
	void WriteComponents(SwisTrackCore *stc);

	//! Writes a string value in the currently selected node.
	void WriteString(const wxString &name, const wxString &value);
	//! Writes a boolean value in the currently selected node.
	void WriteBool(const wxString &name, bool value);
	//! Writes an integer value in the currently selected node.
	void WriteInt(const wxString &name, int value);
	//! Writes a double value in the currently selected node.
	void WriteDouble(const wxString &name, double value);

protected:
	wxXmlDocument mDocument;		//!< The document.
	wxXmlNode *mSelectedNode;		//!< The currently selected node.

};

#endif
