#include "ConfigurationWriterXML.h"
#define THISCLASS ConfigurationWriterXML

THISCLASS::ConfigurationWriterXML():
		ConfigurationXML(new wxXmlNode(0, wxXML_ELEMENT_NODE, wxT("swistrack")), false), mErrorList(), mDocument() {

	mDocument.SetRoot(GetRootNode());
}

THISCLASS::~ConfigurationWriterXML() {
}

void THISCLASS::WriteComponents(SwisTrackCore *stc) {
	SelectRootNode();
	wxXmlNode *node = GetChildNode(wxT("components"));
	stc->ConfigurationWriteXML(node, &mErrorList);
}

bool THISCLASS::Save(const wxFileName &filename) {
	return mDocument.Save(filename.GetFullPath());
}
