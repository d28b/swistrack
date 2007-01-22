#ifndef HEADER_Trigger
#define HEADER_Trigger

class Trigger;

class Trigger {

public:
	//! The interval of the trigger in milliseconds. If this is 0, manual trigger is chosen.
	int mInterval;

	//! Constructor.
	Trigger(): mInterval(0) {}
	//! Destructor.
	~Trigger() {}

	//! Reads the configuration from a XML document.
	void ConfigurationReadXML(xmlpp::Element* configuration, ErrorList *xmlerr);
	//! Writes the configuration to a XML document.
	void ConfigurationWriteXML(xmlpp::Element* configuration, ErrorList *xmlerr);
};

#endif
