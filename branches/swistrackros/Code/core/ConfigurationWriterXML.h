#ifndef HEADER_ConfigurationWriterXML
#define HEADER_ConfigurationWriterXML

class ConfigurationWriterXML;

#include <wx/xml/xml.h>
#include <wx/filename.h>
#include "SwisTrackCore.h"
#include "ErrorList.h"
#include "ConfigurationXML.h"

//! Read.
class ConfigurationWriterXML: public ConfigurationXML {

public:
	ErrorList mErrorList;	//!< Error messages are added here.

	//! Constructor.
	ConfigurationWriterXML();
	//! Destructor.
	~ConfigurationWriterXML();

	//! Opens an XML document. If this function returns true, the document can be read.
	bool Save(const wxFileName &filename);

	//! Returns the XML document. (This function is not needed unless the application needs to access the XML document in a different way.)
	wxXmlDocument *GetDocument() {
		return &mDocument;
	}

	//! Writes the components.
	void WriteComponents(SwisTrackCore *stc);

protected:
	wxXmlDocument mDocument;		//!< The document.

};

#endif
