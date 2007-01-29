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
	ConfigurationReaderXML(SwisTrackCore *stc): mSwisTrackCore(stc), mSwisTrackCoreEditor(), mErrorList(), mParser(), mDocument(0) {}
	//! Destructor.
	~ConfigurationReaderXML() {}

	//! Opens an XML document. If this function returns true, the document can be read.
	bool Open(const std::string &filename);
	//! Returns whether a document is open.
	bool IsOpen() {return (document!=0);}

	//! Reads the components and uses the supplied SwisTrackCoreEditor to set them. If no document is open, an empty component list is created and the return value will be true. If the return value is false, the SwisTrackCore object was not editable.
	bool ReadComponents(SwisTrackCore *stc);
	//! Reads the trigger interval.
	int ReadTriggerInterval(int defvalue);

	//! Reads a boolean value.
	bool ReadBool(const std::string &path, bool defvalue);
	//! Reads an integer.
	int ReadInt(const std::string &path, int defvalue);
	//! Reads a double.
	double ReadDouble(const std::string &path, double defvalue);
	//! Reads a string.
	std::string ReadString(const std::string &path, const std::string &defvalue);

protected:
	SwisTrackCore *mSwisTrackCore;	//!< The attached SwisTrackCore object.
	xmlpp::DomParser mParser;		//!< The XML parser.
	xmlpp::Document *mDocument;		//!< The document.
	
};

#endif
