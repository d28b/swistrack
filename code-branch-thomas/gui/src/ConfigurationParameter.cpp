#include "ConfigurationParameter.h"
#define THISCLASS ConfigurationParameter

#include <wx/sizer.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_SET_FOCUS (THISCLASS::OnSetFocus)
	EVT_KILL_FOCUS (THISCLASS::OnKillFocus)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameter(wxWindow* parent):
		wxPanel(parent, -1), mSwisTrack(0), mComponent(0),
		mLabel(""), mDisplay(""), mReloadable(true),
		mUpdating(true), mFocusEvent(0) {


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
	OnUpdate();
	mUpdating=false;
}

void THISCLASS::CommitChanges() {
	if (mReloadable) {
		mSwisTrack->mSwisTrackCore->ReloadConfiguration();
	} else {
		if (mSwisTrack->mSwisTrackCore->IsStartedInProductiveMode()) {return;}
		mSwisTrack->mSwisTrackCore->Stop();
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

void THISCLASS::OnAfterEditComponent(Component *c) {
	if (mUpdating) {return;}
	if (mComponent!=c) {return;}

	mUpdating=true;
	OnUpdate();
	mUpdating=false;
}

void THISCLASS::OnSetFocus(wxFocusEvent &event) {
	mFocusWindow=(wxWindow *)event.GetEventObject();
}

void THISCLASS::OnKillFocus(wxFocusEvent &event) {
	if (mFocusWindow==(wxWindow *)event.GetEventObject()) {mFocusEvent=0;}
}
