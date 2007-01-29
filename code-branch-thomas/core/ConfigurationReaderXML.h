#ifndef HEADER_ConfigurationReaderXML
#define HEADER_ConfigurationReaderXML

class ConfigurationReaderXML;

#include "SwisTrackCore.h"

//! Reads a SwisTrack configuration from an XML file.
/*!
	Create this an object of this class on the stack and call PrepareRead(). If this returns true, you can call ReadComponents(), ReadTriggerInterval(), ... to read the file. (If the latter methods are called without any open document, the default values are returned.)
*/
class ConfigurationReaderXML {

public:
	ErrorList mErrorList;	//!< Error messages are added here.

	//! Constructor.
	ConfigurationReaderXML(): mErrorList(), mParser(), mDocument(0) {}
	//! Destructor.
	~ConfigurationReaderXML() {}

	//! Opens an XML document. If this function returns true, the document can be read.
	bool Open(const std::string &filename);
	//! Returns whether a document is open.
	bool IsOpen() {return (document!=0);}

	//! Returns the XML document. (This function is not needed unless the application needs to access the XML document in a different way.)
	xmlpp::Document *GetDocument() {return mDocument;}
	//! Returns the first element that matches with the given xpath expression, or 0 if none was found.
	xmlpp::Element *GetElement(const std::string &xpath);

	//! Reads the components and uses the supplied SwisTrackCoreEditor to set them. If no document is open, an empty component list is created and the return value will be true. If the return value is false, the SwisTrackCore object was not editable.
	bool ReadComponents(SwisTrackCore *stc);
	//! Reads the trigger interval.
	int ReadTriggerInterval(int defvalue);

	//! Reads a string.
	std::string ReadString(const std::string &xpath, const std::string &defvalue);
	//! Reads a boolean value.
	bool ReadBool(const std::string &xpath, bool defvalue);
	//! Reads an integer.
	int ReadInt(const std::string &xpath, int defvalue);
	//! Reads a double.
	double ReadDouble(const std::string &xpath, double defvalue);

protected:
	xmlpp::DomParser mParser;		//!< The XML parser.
	xmlpp::Document *mDocument;		//!< The document.

};

#endif
