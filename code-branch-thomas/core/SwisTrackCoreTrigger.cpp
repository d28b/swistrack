#include "SwisTrackCoreTrigger.h"
#define THISCLASS SwisTrackCoreTrigger

DEFINE_EVENT_TYPE(wxEVT_SWISTRACKCORE_TRIGGER)
DEFINE_EVENT_TYPE(wxEVT_SWISTRACKCORE_TRIGGER_CLEAR)

THISCLASS::SwisTrackCoreTrigger(SwisTrackCore *stc):
		mSwisTrackCore(stc), mActive(false), mStepPerformed(false) {

	Connect(wxID_ANY, wxEVT_SWISTRACKCORE_TRIGGER, wxCommandEventHandler(THISCLASS::OnSwisTrackCoreTrigger));
	Connect(wxID_ANY, wxEVT_SWISTRACKCORE_TRIGGER_CLEAR, wxCommandEventHandler(THISCLASS::OnSwisTrackCoreTriggerClear));
}

THISCLASS::~SwisTrackCoreTrigger() {
}

void THISCLASS::OnSwisTrackCoreTrigger(wxCommandEvent &event) {
	if (mStepPerformed) {return;}

	// Return if at least one trigger component isn't ready
	const SwisTrackCore::tComponentList *deployedcomponents=mSwisTrackCore->GetDeployedComponents();
	SwisTrackCore::tComponentList::const_iterator it=deployedcomponents->begin();
	while (it!=deployedcomponents->end()) {
		ComponentTrigger *ct=(*it)->mTrigger;
		if (ct) {
			if (! ct->IsReady()) {
				return;
			}
		}
		it++;
	}

	// Clear the component trigger flags
	it=deployedcomponents->begin();
	while (it!=deployedcomponents->end()) {
		ComponentTrigger *ct=(*it)->mTrigger;
		if (ct) {
			ct->ClearReady();
		}
		it++;
	}

	// Add a trigger clear event at the end of the event queue, to discard pending trigger events (for this step) in the queue
	mStepPerformed=true;
	wxCommandEvent newevent(wxEVT_SWISTRACKCORE_TRIGGER_CLEAR);
	AddPendingEvent(newevent);

	// If the automatic trigger is disabled, we return here
	if (! mActive) {return;}

	// Do a step
	mSwisTrackCore->Step();
}

void THISCLASS::OnSwisTrackCoreTriggerClear(wxCommandEvent &event) {
	mStepPerformed=false;
}
