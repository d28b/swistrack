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

THISCLASS::ConfigurationPanel(wxWindow *parent, SwisTrack *st, Component *c):
		wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
		mSwisTrack(st), mComponent(c) {

	// Prepare
	SetSizer(new wxBoxSizer(wxVERTICAL));
	wxStaticLine *line=new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(230, 2), wxLI_HORIZONTAL);
	GetSizer()->Add(line, 0, 0, 0);

	// Create a scrollable subpanel
	mPanel=new wxScrolledWindow(this, wxID_ANY);
	mPanel->SetScrollRate(0, 1);
	mPanelSizer=new wxBoxSizer(wxVERTICAL);
	mPanel->SetSizer(mPanelSizer);
	GetSizer()->Add(mPanel, 1, wxEXPAND|wxLEFT, 4);

	// If there is no component, we simply display a text
	if (! mComponent) {
		mLabelTitle=new wxStaticText(mPanel, wxID_ANY, "No component selected.");
		mPanelSizer->Add(mLabelTitle, 0, wxEXPAND|wxALL, 4);
		return;
	}

	// Add the title of the component
	mLabelTitle=new wxStaticText(mPanel, wxID_ANY, mComponent->mDisplayName.c_str());
	wxFont f=mLabelTitle->GetFont();
	f.SetWeight(wxFONTWEIGHT_BOLD);
	mLabelTitle->SetFont(f);
	mLabelTitle->Wrap(scParameterWidth);
	mPanelSizer->Add(mLabelTitle, 0, wxEXPAND|wxALL, 4);

	// Read the configuration and create the parameter panels
	wxFileName filename(wxGetApp().mApplicationFolder, mComponent->mName.c_str(), "xml");
	filename.AppendDir("Components");
	Read(filename);

	// In case of errors, show a dialog with the error messages and add a label to the panel
	if (! mErrorList.mList.empty()) {
		wxStaticText *label=new wxStaticText(mPanel, wxID_ANY, "An error occured while loading the configuration panel of this component.");
		label->Wrap(scParameterWidth);
		mPanelSizer->Add(label, 0, wxEXPAND|wxALL, 4);
	}
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

	// Add a description (if available)
	wxString description=config.ReadString("description", "");
	if (description!="") {
		wxStaticText *label=new wxStaticText(mPanel, wxID_ANY, description);
		label->Wrap(scParameterWidth);
		mPanelSizer->Add(label, 0, wxEXPAND|wxALL, 4);
	}

	// Read other properties
	mHelpURL=config.ReadString("url", "");
	mDisplayName=config.ReadString("displayimage", ""); // TODO rename this

	// Add a help button (if available)
	if (mHelpURL!="") {
		wxStaticText *label=new wxStaticText(mPanel, wxID_ANY, "More information ...");
		wxFont f=label->GetFont();
		f.SetUnderlined(true);
		label->SetFont(f);
		label->SetForegroundColour(*wxBLUE);
		label->Connect(wxID_ANY, wxEVT_LEFT_UP, wxMouseEventHandler(THISCLASS::OnHelpURLClick), 0, this);
		mPanelSizer->Add(label, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 4);
	}

	wxStaticLine *line=new wxStaticLine(mPanel, wxID_ANY, wxDefaultPosition, wxSize(200, 2), wxLI_HORIZONTAL);
	mPanelSizer->Add(line, 0, wxEXPAND|wxALL, 4);

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
		ReadConfigurationNode(node);
		node=node->GetNext();
	}

	return;
}

void THISCLASS::ReadConfigurationNode(wxXmlNode *node) {
	wxString nodename=node->GetName();
	if (nodename=="parameter") {
		ReadParameter(node);
	} else if (nodename=="label") {
		wxStaticText *label=new wxStaticText(mPanel, wxID_ANY, node->GetNodeContent());
		label->Wrap(scParameterWidth);
		mPanelSizer->Add(label, 0, wxEXPAND|wxALL, 4);
	} else if (nodename=="title") {
		wxStaticText *label=new wxStaticText(mPanel, wxID_ANY, node->GetNodeContent());
		wxFont f=label->GetFont();
		f.SetWeight(wxFONTWEIGHT_BOLD);
		label->SetFont(f);
		label->Wrap(scParameterWidth);
		mPanelSizer->Add(label, 0, wxEXPAND|wxALL, 4);
	} else if (nodename=="line") {
		wxStaticLine *line=new wxStaticLine(mPanel, wxID_ANY, wxDefaultPosition, wxSize(200, 2), wxLI_HORIZONTAL);
		mPanelSizer->Add(line, 0, wxEXPAND|wxALL, 4);
	} else {
		std::ostringstream oss;
		oss << "Invalid node '" << nodename << "' in 'configuration'. Allowed types are 'line', 'label', 'title' and 'parameter'.";
		mErrorList.Add(oss.str());
	}
}

void THISCLASS::ReadParameter(wxXmlNode *node) {
	if (! node) {return;}

	// Find out the type and create a new object
	wxString type=GetPropertyString(node, "type", "");
	ConfigurationParameterFactory cpf;
	ConfigurationParameter *cp=cpf.Create(type, mPanel);
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
	mPanelSizer->Add(cp, 0, wxEXPAND|wxALL, 4);
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

void THISCLASS::OnHelpURLClick(wxMouseEvent& event) {
	wxLaunchDefaultBrowser(mHelpURL);
}
