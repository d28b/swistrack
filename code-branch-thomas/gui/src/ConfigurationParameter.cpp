#include "ConfigurationParameter.h"
#define THISCLASS ConfigurationParameter

#include <wx/sizer.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"

static void THISCLASS::Create(const wxString &type, wxWindow* parent) {
	switch (type) {
	case "integer": return new ConfigurationParameterInteger(parent, node);
	case "double": return new ConfigurationParameterDouble(parent, node);
	case "string": return new ConfigurationParameterString(parent, node);
	case "angle": return new ConfigurationParameterAngle(parent, node);
	}

	return 0;
}

THISCLASS::ConfigurationParameter(wxWindow* parent):
		wxPanel(parent, -1), SwisTrack(0), SwisTrackCore(0),
		mName(""), mDisplay(""), mReloadable(true) {


}

THISCLASS::~ConfigurationParameter() {

}

void THISCLASS::Initialize(SwisTrack *st, Component *c, ConfigurationXML *config) {
	// Set the associated objects
	mSwisTrack=st;
	mSwisTrackCore=st->mSwisTrackCore;
	mComponent=c;

	// Add SwisTrackCoreInterface
	mSwisTrackCore->AddInterface(this);

	// Read general configuration
	config->SelectRootNode();
	mName=config->ReadString("name", "");
	mDisplay=config->ReadString("display", "");
	mReloadable=config->ReadBool("reloadable", true);
	
	// Initializes the parameter
	OnInitialize(config);
}

void THISCLASS::OnBeforeStart(bool productivemode) {
	if (! productivemode) {return;}

	if (! mReloadable) {
		this->Hide();
		//mText->SetEnabled(false);
	}
}

void THISCLASS::OnAfterStop() {
	this->Show();
}
