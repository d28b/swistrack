#ifndef HEADER_ConfigurationReaderXML
#define HEADER_ConfigurationReaderXML

class ConfigurationReaderXML;

#include <wx/xml/xml.h>
#include <wx/filename.h>
#include "SwisTrackCore.h"
#include "ConfigurationXML.h"

//! Reads a SwisTrack configuration from an XML file.
/*!
	Create this an object of this class on the stack and call PrepareRead(). If this returns true, you can call ReadComponents(), ReadTriggerInterval(), ... to read the file. (If the latter methods are called without any open document, the default values are returned.)
*/
class ConfigurationReaderXML: public ConfigurationXML {

public:
	ErrorList mErrorList;	//!< Error messages are added here.

	//! Constructor.
	ConfigurationReaderXML();
	//! Destructor.
	~ConfigurationReaderXML();

	//! Opens an XML document. If this function returns true, the document can be read.
	bool Open(const wxFileName &filename);
	bool Open(const wxString &filename) {
	  return Open(wxFileName(filename));
	}
	//! Returns whether a document is open.
	bool IsOpen() {
		return mIsOpen;
	}

	//! Reads the components and uses the supplied SwisTrackCoreEditor to set them. If no document is open, an empty component list is created and the return value will be true. If the return value is false, the SwisTrackCore object was not editable.
	bool ReadComponents(SwisTrackCore *stc);

protected:
	wxXmlDocument mDocument;		//!< The document.
	bool mIsOpen;					//!< Whether the document is open or not.

};

#endif
