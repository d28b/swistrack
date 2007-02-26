#include "ConfigurationReaderXML.h"
#define THISCLASS ConfigurationReaderXML

#include <sstream>
#include <wx/log.h>
#include "SwisTrackCoreEditor.h"

THISCLASS::ConfigurationReaderXML(): 
		ConfigurationXML(0, true),
		mErrorList(), mDocument(), mIsOpen(false) {

}

THISCLASS::~ConfigurationReaderXML() {
}

bool THISCLASS::Open(const wxString &filename) {
	// Read the file
	wxLogNull log;
	mIsOpen=mDocument.Load(filename);
	if (! mIsOpen) {
		mDocument.SetRoot(new wxXmlNode(0, wxXML_ELEMENT_NODE, "swistrack"));
		SelectRootNode();
		return false;
	}

	// Select the root element and check its name
	SetRootNode(mDocument.GetRoot());
	if (GetRootNode()->GetName() != "swistrack") {
		mIsOpen=false;
		return false;
	}

	return true;
}

bool THISCLASS::ReadComponents(SwisTrackCore *stc) {
	SwisTrackCoreEditor stce(stc);
	if (! stce.IsEditable()) {return false;}

	// Read the component list
	SelectRootNode();
	stce.ConfigurationReadXML(GetChildNode("components"), &mErrorList);
	return true;
}
