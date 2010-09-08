#include "SwisTrackCoreTrigger.h"
#define THISCLASS SwisTrackCoreTrigger

#include "SwisTrackCore.h"

DEFINE_EVENT_TYPE(wxEVT_SWISTRACKCORE_TRIGGER)
DEFINE_EVENT_TYPE(wxEVT_SWISTRACKCORE_TRIGGER_CLEAR)

THISCLASS::SwisTrackCoreTrigger(SwisTrackCore *stc):
		wxEvtHandler(), SwisTrackCoreInterface(),
		mSwisTrackCore(stc), mActive(false), mStepPerformed(false) {

	mSwisTrackCore->AddInterface(this);
	Connect(wxID_ANY, wxEVT_SWISTRACKCORE_TRIGGER, wxEventHandler(THISCLASS::OnTrigger));
	Connect(wxID_ANY, wxEVT_SWISTRACKCORE_TRIGGER_CLEAR, wxEventHandler(THISCLASS::OnTriggerClear));
}

THISCLASS::~SwisTrackCoreTrigger() {
	mSwisTrackCore->RemoveInterface(this);
	Disconnect(wxID_ANY, wxEVT_SWISTRACKCORE_TRIGGER, wxEventHandler(THISCLASS::OnTrigger));
	Disconnect(wxID_ANY, wxEVT_SWISTRACKCORE_TRIGGER_CLEAR, wxEventHandler(THISCLASS::OnTriggerClear));
}

void THISCLASS::OnTrigger(wxEvent &event) {
	if (mStepPerformed) {
		return;
	}
	if (! mActive) {
		return;
	}

	// Return if at least one trigger component isn't ready
	const SwisTrackCore::tComponentList *deployedcomponents = mSwisTrackCore->GetDeployedComponents();
	SwisTrackCore::tComponentList::const_iterator it = deployedcomponents->begin();
	while (it != deployedcomponents->end()) {
		ComponentTrigger *ct = (*it)->mTrigger;
		if (ct) {
			if (! ct->IsReady()) {
				return;
			}
		}
		it++;
	}

	// Clear the component trigger flags
	it = deployedcomponents->begin();
	while (it != deployedcomponents->end()) {
		ComponentTrigger *ct = (*it)->mTrigger;
		if (ct) {
			ct->ClearReady();
		}
		it++;
	}

	// Add a trigger clear event at the end of the event queue, to discard pending trigger events (for this step) in the queue
	mStepPerformed = true;
	wxCommandEvent newevent(wxEVT_SWISTRACKCORE_TRIGGER_CLEAR);
	AddPendingEvent(newevent);

	// Do a step
	mSwisTrackCore->Step();
}

void THISCLASS::OnTriggerClear(wxEvent &event) {
	mStepPerformed = false;
}

void THISCLASS::OnIdle() {
	// In case there is no trigger, we execute the steps as fast as possible.
	// Hence try to launch the next step.
	wxCommandEvent ev;
	OnTrigger(ev);
}

void THISCLASS::OnAfterStart(bool productionmode) {
	mStepPerformed = false;
}
