#include "ConfigurationPanel.h"
#define THISCLASS ConfigurationPanel

#include <sstream>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include "Application.h"
#include "ErrorList.h"
#include "ConfigurationParameterFactory.h"
#include "ConfigurationXML.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
END_EVENT_TABLE()

THISCLASS::ConfigurationPanel(wxWindow *parent, SwisTrack *st, Component *c):
		wxPanel(parent, -1, wxDefaultPosition, wxSize(200, 20)),
		mSwisTrack(st), mComponent(c) {

	// Prepare
	SetSizer(new wxBoxSizer(wxVERTICAL));
	wxStaticLine *line=new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(200, 2), wxLI_HORIZONTAL);
	GetSizer()->Add(line, 0, wxEXPAND|wxALL, 0);

	// If there is no component, we simply display a text
	if (! mComponent) {
		mLabelTitle=new wxStaticText(this, wxID_ANY, "No component selected.");
		GetSizer()->Add(mLabelTitle, 0, wxEXPAND|wxALL, 4);
		return;
	}

	// Add the title of the component
	mLabelTitle=new wxStaticText(this, -1, mComponent->mDisplayName.c_str());
	wxFont f=mLabelTitle->GetFont();
	f.SetWeight(wxFONTWEIGHT_BOLD);
	mLabelTitle->SetFont(f);
	mLabelTitle->Wrap(190);
	GetSizer()->Add(mLabelTitle, 0, wxEXPAND|wxALL, 4);

	// Read the configuration and create the parameter panels
	wxFileName filename(wxGetApp().mApplicationFolder, mComponent->mName.c_str(), "swistrackcomponent");
	Read(filename);

	// In case of errors, show a dialog with the error messages and add a label to the panel
	if (! mErrorList.mList.empty()) {
		wxStaticText *label=new wxStaticText(this, -1, "An error occured while loading the configuration panel of this component.");
		label->Wrap(190);
		GetSizer()->Add(label, 0, wxEXPAND|wxALL, 4);
	}

	// Add a stretchable box
	GetSizer()->AddStretchSpacer();
	GetSizer()->Layout();
}

THISCLASS::~ConfigurationPanel() {
}

void THISCLASS::Read(const wxFileName &filename) {
	// Open the file
	wxLogNull log;
	wxXmlDocument document;
	if (! document.Load(filename.GetFullPath())) {
		std::ostringstream oss;
		oss << "Unable to open or parse the file '" << filename.GetFullPath() << "'.";
		mErrorList.Add(oss.str());
		return;
	}

	// Prepare
	ConfigurationXML config(document.GetRoot());

	// Set the title
	wxString friendlyname=config.ReadString("friendlyname", "");
	if (friendlyname!="") {
		mLabelTitle->SetLabel(friendlyname);
	}

	// Add a description
	wxString description=config.ReadString("description", "");
	if (description!="") {
		wxStaticText *label=new wxStaticText(this, -1, description);
		label->Wrap(190);
		GetSizer()->Add(label, 0, wxEXPAND|wxALL, 4);
	}

	// Set the display image (TODO)
	//wxString display=config.ReadString("display", "");

	wxStaticLine *line=new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	GetSizer()->Add(line, 0, wxEXPAND|wxALL, 4);

	// Read the list of settings
	wxXmlNode *configurationnode=config.GetChildNode("configuration");
	ReadConfiguration(configurationnode);
}

void THISCLASS::ReadConfiguration(wxXmlNode *configurationnode) {
	if (! configurationnode) {
		mErrorList.Add("The node 'configuration' is missing.");
		return;
	}

	wxXmlNode *node=configurationnode->GetChildren();
	while (node) {
		ReadParameter(node);
		node=node->GetNext();
	}

	return;
}

void THISCLASS::ReadParameter(wxXmlNode *node) {
	if (! node) {return;}
	if (node->GetName()!="parameter") {
		mErrorList.Add("All nodes in 'configuration' must be of type 'parameter'.");
		return;
	}

	// Find out the type and create a new object
	wxString type=GetPropertyString(node, "type", "");
	ConfigurationParameterFactory cpf;
	ConfigurationParameter *cp=cpf.Create(type, this);
	if (! cp) {
		std::ostringstream oss;
		oss << "There is no parameter of type '" << type << "'.";
		mErrorList.Add(oss.str());
		return;
	}

	// Initialize this parameter
	ConfigurationXML config(node);
	cp->Initialize(mSwisTrack, mComponent, &config, &mErrorList);

	// Add this panel to our sizer
	GetSizer()->Add(cp, 0, wxEXPAND|wxALL, 4);
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
