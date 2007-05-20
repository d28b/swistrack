#include "ComponentTriggerTimer.h"
#define THISCLASS ComponentTriggerTimer

#include <sstream>

THISCLASS::ComponentTriggerTimer(SwisTrackCore *stc):
		Component(stc, "TriggerTimer"),
		mTimer(0) {

	// Data structure relations
	mCategory=&(mCore->mCategoryTrigger);

	// Trigger
	mTrigger=new ComponentTrigger(this);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentTriggerTimer() {
}

void THISCLASS::OnStart() {
	if (! mTimer) {
		mTimer=new Timer(this);
	}

	double interval=GetConfigurationDouble("Interval", 1);
	if (interval<0.001) {interval=0.001;}

	mTimer.Start(interval, false);
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	mTimer.Stop();
}
