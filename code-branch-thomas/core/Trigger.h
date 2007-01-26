#ifndef HEADER_Trigger
#define HEADER_Trigger

class Trigger;

//! Holds information about the trigger.
class Trigger {

public:
	int mInterval;	//!< The interval of the trigger in milliseconds. If this is 0, manual trigger is chosen.

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
