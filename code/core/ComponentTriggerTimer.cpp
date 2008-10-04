#include "ComponentTriggerTimer.h"
#define THISCLASS ComponentTriggerTimer

THISCLASS::ComponentTriggerTimer(SwisTrackCore *stc):
		Component(stc, wxT("TriggerTimer")),
		mTimer(0) {

	// Data structure relations
	mCategory = &(mCore->mCategoryTrigger);

	// Trigger
	mTrigger = new ComponentTrigger(this);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentTriggerTimer() {
	delete mTrigger;
	delete mTimer;
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	if (! mTimer) {
		mTimer = new Timer(this);
	}

	double interval = GetConfigurationDouble(wxT("Interval"), 1);
	int interval_ms = (int)(interval * 1000);
	if (interval_ms < 1) {
		interval_ms = 1;
	}

	if (mTimer->IsRunning()) {
		mTimer->Stop();
	}
	mTimer->Start(interval_ms, false);
}

void THISCLASS::OnStep() {
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	if (! mTimer) {
		return;
	}
	mTimer->Stop();
}
