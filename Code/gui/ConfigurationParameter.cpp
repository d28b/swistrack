#include "ConfigurationParameter.h"
#define THISCLASS ConfigurationParameter

#include <wx/sizer.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"

THISCLASS::ConfigurationParameter(wxWindow * parent):
	wxPanel(parent, -1), mSwisTrack(0), mComponent(0),
	mLabel(wxT("")), mDisplay(wxT("")), mOnChange(Nothing),
	mUpdating(true), mUpdateProtection(0) {

}

THISCLASS::~ConfigurationParameter() {
	mSwisTrack->mSwisTrackCore->RemoveInterface(this);
}

void THISCLASS::Initialize(SwisTrack * st, Component * c, ConfigurationXML * config, ErrorList * errorlist) {
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
	wxString onChange = config->ReadString(wxT("onchange"), wxT(""));
	mOnChange =
		onChange == wxT("reload") ? Reload :
		onChange == wxT("restart") ? Restart :
			Nothing;

	// Initializes the parameter
	OnInitialize(config, errorlist);
	OnUpdate(0);
	mUpdating = false;
}


void THISCLASS::SetNewValue(wxWindow * updateProtection) {
	// If we are in OnUpdate(), do nothing
	if (mUpdating) return;

	// Set the new configuration values
	mUpdateProtection = updateProtection;
	OnSetNewValue();
	mUpdateProtection = 0;

	// Reload the configuration and perform a step
	if (mSwisTrack->mSwisTrackCore->IsStartedInProductionMode()) return;

	if (mOnChange == Reload) {
		mSwisTrack->mSwisTrackCore->ReloadConfigurationOfSingleComponent(mComponent);
		mSwisTrack->mSwisTrackCore->Step();
	} else if (mOnChange == Restart) {
		mSwisTrack->mSwisTrackCore->Stop();
		mSwisTrack->mSwisTrackCore->Start(false);
		mSwisTrack->mSwisTrackCore->Step();
	} else {
		mSwisTrack->mSwisTrackCore->Stop();
	}
}

void THISCLASS::OnBeforeStart(bool productionmode) {
	if (! productionmode) return;
	if (mOnChange == Restart) this->Hide();
}

void THISCLASS::OnAfterStop() {
	this->Show();
}

void THISCLASS::OnAfterEditComponent(Component * c) {
	if (mUpdating) return;
	if (mComponent != c) return;

	mUpdating = true;
	OnUpdate(mUpdateProtection);
	mUpdating = false;
}
