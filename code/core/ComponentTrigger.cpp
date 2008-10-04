#include "ComponentTrigger.h"
#define THISCLASS ComponentTrigger

#include <wx/event.h>

THISCLASS::ComponentTrigger(Component *c):
		mComponent(c), mIsReady(false) {

}

void THISCLASS::SetReady(bool ready) {
	mIsReady = ready;

	if (mIsReady) {
		wxCommandEvent newevent(wxEVT_SWISTRACKCORE_TRIGGER);
		mComponent->GetSwisTrackCore()->mTrigger->AddPendingEvent(newevent);
	}
}

void THISCLASS::ClearReady() {
	mIsReady = false;
}
