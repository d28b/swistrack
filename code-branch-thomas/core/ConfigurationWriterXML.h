#ifndef HEADER_ConfigurationWriterXML
#define HEADER_ConfigurationWriterXML

class ConfigurationWriterXML;

//! Read.
class ConfigurationWriterXML {

public:
	ErrorList mErrorList;	//!< Error messages are added here.

	//! Constructor.
	ConfigurationWriterXML(SwisTrackCore *stc): mSwisTrackCore(stc), mInterval(0) {}
	//! Destructor.
	~ConfigurationWriterXML() {}

	//! Opens an XML document. If this function returns true, the document can be read.
	bool Save(const std::string &filename);

	//! Writes the trigger interval.
	void WriteComponents(SwisTrackCore *stc);
	//! Writes the trigger interval.
	void WriteTriggerInterval(int interval);

	//! Reads a boolean value.
	bool ReadBool(const std::string &path, bool defvalue);

protected:
	SwisTrackCore *mSwisTrackCore;	//!< The attached SwisTrackCore object.
	xmlpp::DomParser mParser;		//!< The XML parser.
	xmlpp::Document *mDocument;		//!< The document.

};

#endif
