#include "ConfigurationParameter.h"
#define THISCLASS ConfigurationParameter

#include <wx/sizer.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"

THISCLASS::ConfigurationParameter(wxWindow* parent):
		wxPanel(parent, -1), mSwisTrack(0), mSwisTrackCore(0), mComponent(0),
		mLabel(""), mDisplay(""), mReloadable(true) {


}

THISCLASS::~ConfigurationParameter() {
	mSwisTrack->mSwisTrackCore->RemoveInterface(this);
}

void THISCLASS::Initialize(SwisTrack *st, Component *c, ConfigurationXML *config, ErrorList *errorlist) {
	// Set the associated objects
	mSwisTrack=st;
	mComponent=c;

	// Add SwisTrackCoreInterface
	mSwisTrack->mSwisTrackCore->AddInterface(this);

	// Read general configuration
	config->SelectRootNode();
	mName=config->ReadString("name", "");
	mLabel=config->ReadString("label", "");
	mDisplay=config->ReadString("display", "");
	mReloadable=config->ReadBool("reloadable", true);
	
	// Initializes the parameter
	OnInitialize(config, errorlist);
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

void OnAfterEditComponent(Component *c) {
	if (mComponent!=c) {return;}
	
	OnUpdate();
}
