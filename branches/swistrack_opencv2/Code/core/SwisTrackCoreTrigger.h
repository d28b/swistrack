#ifndef HEADER_SwisTrackCoreTrigger
#define HEADER_SwisTrackCoreTrigger

class SwisTrackCoreTrigger;
class SwisTrackCore;
#include "SwisTrackCoreInterface.h"
#include <wx/event.h>

//! The trigger. An object of this class is directly attached to the SwisTrackCore object and handles the automatic triggering of steps.
class SwisTrackCoreTrigger: public wxEvtHandler, public SwisTrackCoreInterface {

public:
	SwisTrackCore *mSwisTrackCore;		//!< The associated SwisTrackCore object.

	//! Constructor.
	SwisTrackCoreTrigger(SwisTrackCore *stc);
	//! Destructor.
	~SwisTrackCoreTrigger();

	//! Executes a step if all trigger components are ready.
	void OnTrigger(wxEvent &event);
	//! Clears the StepPerformed flag so that the a new step can be executed.
	void OnTriggerClear(wxEvent &event);
	//! Idle event handler.
	void OnIdle();

	//! Actives or deactivates the trigger.
	void SetActive(bool active) {
		mActive = active;
	}
	//! Actives or deactivates the trigger.
	bool GetActive() const {
		return mActive;
	}

	// SwisTrackCoreInterface methods
	void OnAfterStart(bool productionmode);

private:
	bool mActive;				//!< Whether the automatic trigger is activated or not.
	bool mStepPerformed;		//!< Whether the current step has been performed. This flag is used to discard additional events that might want to execute the same step.

};

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_SWISTRACKCORE_TRIGGER, -1)
DECLARE_EVENT_TYPE(wxEVT_SWISTRACKCORE_TRIGGER_CLEAR, -1)
END_DECLARE_EVENT_TYPES()

#endif
