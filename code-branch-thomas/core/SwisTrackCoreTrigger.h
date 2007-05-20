#ifndef HEADER_SwisTrackCoreTrigger
#define HEADER_SwisTrackCoreTrigger

class SwisTrackCoreTrigger;
class SwisTrackCore;

#include <wx/event.h>

class SwisTrackCoreTrigger: public wxEvtHandler {

public:
	SwisTrackCore *mSwisTrackCore;		//!< The associated SwisTrackCore object.
	bool mActive;						//!< Whether the automatic trigger is activated or not.

	//! Constructor.
	SwisTrackCoreTrigger(SwisTrackCore *stc);
	//! Destructor.
	~SwisTrackCoreTrigger();

	//! Executes a step.
	void OnStep(wxCommandEvent &event);

private:
	bool mStepPerformed;

};

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_SWISTRACKCORE_TRIGGER, -1)
DECLARE_EVENT_TYPE(wxEVT_SWISTRACKCORE_TRIGGER_CLEAR, -1)
END_DECLARE_EVENT_TYPES()

#endif
