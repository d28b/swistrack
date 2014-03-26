#include "ConfigurationParameter.h"
#define THISCLASS ConfigurationParameter

#include <wx/sizer.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"

THISCLASS::ConfigurationParameter(wxWindow* parent):
		wxPanel(parent, -1), mSwisTrack(0), mComponent(0),
		mLabel(wxT("")), mDisplay(wxT("")), mReloadable(true),
		mUpdating(true), mUpdateProtection(0) {

}

THISCLASS::~ConfigurationParameter() {
	mSwisTrack->mSwisTrackCore->RemoveInterface(this);
}

void THISCLASS::Initialize(SwisTrack *st, Component *c, ConfigurationXML *config, ErrorList *errorlist) {
	// Set the associated objects
	mSwisTrack = st;
	mComponent = c;

	// Add SwisTrackCoreInterface
	mSwisTrack->mSwisTrackCore->AddInterface(this);

	// Read general configuration
	config->SelectRootNode();
	mName = config->ReadString(wxT("name"), wxT(""));
	mLabel = config->ReadString(wxT("label"), wxT(""));
	mDisplay = config->ReadString(wxT("display"), wxT(""));
	mReloadable = config->ReadBool(wxT("reloadable"), false);

	// Initializes the parameter
	OnInitialize(config, errorlist);
	OnUpdate(0);
	mUpdating = false;
}


void THISCLASS::SetNewValue(wxWindow *updateprotection) {
	// If we are in OnUpdate(), do nothing
	if (mUpdating) {
		return;
	}

	// Set the new configuration values
	mUpdateProtection = updateprotection;
	OnSetNewValue();
	mUpdateProtection = 0;

	// Reload the configuration and perform a step
	if (mReloadable) {
		mSwisTrack->mSwisTrackCore->ReloadConfiguration();
		if (mSwisTrack->mSwisTrackCore->IsStartedInProductionMode()) {
			return;
		}
		mSwisTrack->mSwisTrackCore->Step();
	} else {
		if (mSwisTrack->mSwisTrackCore->IsStartedInProductionMode()) {
			return;
		}
		mSwisTrack->mSwisTrackCore->Stop();
		mSwisTrack->mSwisTrackCore->Start(false);
		mSwisTrack->mSwisTrackCore->Step();
	}
}

void THISCLASS::OnBeforeStart(bool productionmode) {
	if (! productionmode) {
		return;
	}

	if (! mReloadable) {
		this->Hide();
		//mText->SetEnabled(false);
	}
}

void THISCLASS::OnAfterStop() {
	this->Show();
}

void THISCLASS::OnAfterEditComponent(Component *c) {
	if (mUpdating) {
		return;
	}
	if (mComponent != c) {
		return;
	}

	mUpdating = true;
	OnUpdate(mUpdateProtection);
	mUpdating = false;
}
