#include "ConfigurationParameter.h"
#define THISCLASS ConfigurationParameter

#include <wx/sizer.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"

THISCLASS::ConfigurationParameter(wxWindow* parent, wxXmlNode *node):
		wxPanel(parent, -1) {

	// Add SwisTrackCoreInterface
	mSwisTrackCore->AddInterface(this);

}

THISCLASS::~ConfigurationParameter() {

}

static void THISCLASS::Create(const wxString &type, wxWindow* parent, wxXmlNode *node) {
	switch (type) {
	case "integer": return new ConfigurationParameterInteger(parent, node);
	case "double": return new ConfigurationParameterDouble(parent, node);
	case "string": return new ConfigurationParameterString(parent, node);
	}

	return 0;
}

void THISCLASS::OnBeforeStart(bool productivemode) {
	
}

void THISCLASS::OnAfterStop() {
	
}
