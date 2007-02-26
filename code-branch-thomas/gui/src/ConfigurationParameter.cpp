#include "ConfigurationParameter.h"
#define THISCLASS ConfigurationParameter

#include <wx/sizer.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"

THISCLASS::ConfigurationParameter(wxWindow* parent):
		wxPanel(parent, -1), mSwisTrack(0), mSwisTrackCore(0), mComponent(0),
		mName(""), mDisplay(""), mReloadable(true) {


}

THISCLASS::~ConfigurationParameter() {

}

void THISCLASS::Initialize(SwisTrack *st, ConfigurationXML *config) {
	// Set the associated objects
	mSwisTrack=st;
	mSwisTrackCore=st->mSwisTrackCore;

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

void THISCLASS::CommitChanges() {
	if (mReloadable) {
		mSwisTrackCore->ReloadConfiguration();
	} else {
		if (mSwisTrackCore->IsStartedInProductiveMode()) {return;}
		mSwisTrackCore->Stop();
	}
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
