#ifndef HEADER_ConfigurationWriterXML
#define HEADER_ConfigurationWriterXML

class ConfigurationWriterXML;

#include <libxml++/libxml++.h>
#include "SwisTrackCore.h"
#include "ErrorList.h"

//! Read.
class ConfigurationWriterXML {

public:
	ErrorList mErrorList;	//!< Error messages are added here.

	//! Constructor.
	ConfigurationWriterXML();
	//! Destructor.
	~ConfigurationWriterXML() {}

	//! Opens an XML document. If this function returns true, the document can be read.
	bool Save(const std::string &filename);

	//! Returns the XML document. (This function is not needed unless the application needs to access the XML document in a different way.)
	xmlpp::Document *GetDocument() {return &mDocument;}

	//! Writes the trigger interval.
	void WriteComponents(SwisTrackCore *stc);
	//! Writes the trigger interval.
	void WriteTriggerInterval(int interval);

	//! Selects the root node.
	xmlpp::Element *SelectRootNode();
	//! Selects a subnode. If this node doesn't exist, it is created.
	xmlpp::Element *SelectNode(const std::string &name);

	//! Writes a string value in the currently selected node.
	void WriteString(const std::string &name, const std::string &value);
	//! Writes a boolean value in the currently selected node.
	void WriteBool(const std::string &name, bool value);
	//! Writes an integer value in the currently selected node.
	void WriteInt(const std::string &name, int value);
	//! Writes a double value in the currently selected node.
	void WriteDouble(const std::string &name, double value);

protected:
	xmlpp::Document mDocument;		//!< The document.
	xmlpp::Element *mSelectedNode;	//!< The currently selected node.

};

#endif
