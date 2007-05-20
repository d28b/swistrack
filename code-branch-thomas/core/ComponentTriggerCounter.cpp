#include "ComponentTriggerCounter.h"
#define THISCLASS ComponentTriggerCounter

#include <sstream>

THISCLASS::ComponentTriggerCounter(SwisTrackCore *stc):
		Component(stc, "TriggerCounter"),
		mCount(1), mDone(0) {

	// Data structure relations
	mCategory=&(mCore->mCategoryTrigger);

	// Trigger
	mTrigger=new ComponentTrigger(this);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentTriggerCounter() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mCount=GetConfigurationInt("Count", 1);
	if (mCount<1) {
		mCount=1;
	}

	bool start=GetConfigurationBool("Start", false);
	if (start) {
		SetConfigurationBool("Start", false);
		mTodo=mCount;
		mTrigger->SetReady();
	}
}

void THISCLASS::OnStep() {
	mTodo--;
	if (mTodo>0) {
		mTrigger->SetReady();
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	mTodo=0;
}

#endif
