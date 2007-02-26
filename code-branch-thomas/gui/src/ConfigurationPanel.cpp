#include "ConfigurationPanel.h"
#define THISCLASS ConfigurationPanel

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ConfigurationParameterFactory.h"
#include "ConfigurationXML.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
END_EVENT_TABLE()

THISCLASS::ConfigurationPanel(wxWindow *parent, SwisTrackCore *stc):
		wxPanel(parent, -1),
		SwisTrackCoreInterface(),
		mSwisTrack(0), mSwisTrackCore(0), mComponent(0) {

	// Add SwisTrackCoreInterface
	mSwisTrackCore->AddInterface(this);

	// Open the file
	wxLogNull log;
	wxXmlDocument document;
	wxString filename="";  // FIXME
	if (! document.Load(filename)) {
		new wxStaticText(this, -1, "Unable to load the component settings.");
		return;
	}

	// Read the main component properties
	ConfigurationXML config(document.GetRoot());
	mComponentDisplayName=config.ReadString("name", mComponent->mDisplayName.c_str());		// FIXME
	mComponentDescription=config.ReadString("description", "");
	mComponentDefaultDisplay=config.ReadString("display", "");

	// Read the list of settings
	wxXmlNode *configurationnode=config.GetChildNode("configuration");
	ReadConfiguration(configurationnode);
}

THISCLASS::~ConfigurationPanel() {
}

void THISCLASS::ReadConfiguration(wxXmlNode *configurationnode) {
	if (! configurationnode) {return;}

	wxXmlNode *node=configurationnode->GetChildren();
	while (node) {
		ReadParameter(node);
		node=node->GetNext();
	}

	return;
}

void THISCLASS::ReadParameter(wxXmlNode *node) {
	if (! node) {return;}
	if (node->GetName()!="parameter") {return;}

	// Find out the type and create a new object
	wxString type=GetPropertyString(node, "type", "");
	ConfigurationParameterFactory cpf;
	ConfigurationParameter *cp=cpf.Create(type, this);
	if (! cp) {
		//mErrorList.
		return;
	}

	// Initialize this parameter
	ConfigurationXML config(node);
	cp->Initialize(mSwisTrack, &config);
}

wxString THISCLASS::GetPropertyString(wxXmlNode *node, const wxString &name, const wxString &defvalue) {
	if (! node) {return defvalue;}

	// Find out the type
	wxXmlProperty *prop=node->GetProperties();
	while (prop) {
		if (prop->GetName()==name) {return prop->GetValue();}
		prop=prop->GetNext();
	}

	// Return default value if not found
	return defvalue;
}
