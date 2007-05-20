#include "ComponentTrigger.h"
#define THISCLASS ComponentTrigger

THISCLASS::ComponentTrigger(Component *c):
		mComponent(c), mIsReady(false) {

	mTrigger=mComponent->GetSwisTrackCore()->mTrigger;
}

void THISCLASS::SetReady(bool ready) {
	mIsReady=ready;

	if (mIsReady) {
		wxCommandEvent newevent(wxEVT_EXECUTION_CONTINUE);
		mExecution->AddPendingEvent(newevent);
	}
}
