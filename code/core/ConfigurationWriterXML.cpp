#include "ConfigurationWriterXML.h"
#define THISCLASS ConfigurationWriterXML

#include <sstream>

THISCLASS::ConfigurationWriterXML():
		ConfigurationXML(new wxXmlNode(0, wxXML_ELEMENT_NODE, "swistrack"), false), mErrorList(), mDocument() {

	mDocument.SetRoot(GetRootNode());
}

THISCLASS::~ConfigurationWriterXML() {
}

void THISCLASS::WriteComponents(SwisTrackCore *stc) {
	SelectRootNode();
	wxXmlNode *node=GetChildNode("components");
	stc->ConfigurationWriteXML(node, &mErrorList);
}

bool THISCLASS::Save(const wxString &filename) {
	return mDocument.Save(filename);
}
