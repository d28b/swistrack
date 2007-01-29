#ifndef HEADER_ConfigurationXML
#define HEADER_ConfigurationXML

class ConfigurationXML;

//! Read.
class ConfigurationXML {

public:
	int mInterval;	//!< The interval of the trigger in milliseconds. If this is 0, manual ConfigurationXML is chosen.

	//! Constructor.
	ConfigurationXML(): mInterval(0) {}
	//! Destructor.
	~ConfigurationXML() {}

	//! Reads the configuration from a XML document.
	bool Read(const std::string &filename);
	void Deploy(ErrorList *xmlerr);
	//! Writes the configuration to a XML document.
	void ConfigurationWriteXML(xmlpp::Element* configuration, ErrorList *xmlerr);

protected:
	SwisTrackCore *mSwisTrackCore;	//!< The attached SwisTrackCore object.
	xmlpp::DomParser mParser;		//!< The XML parser.
	xmlpp::Document *mDocument;		//!< The document.

};

#endif
