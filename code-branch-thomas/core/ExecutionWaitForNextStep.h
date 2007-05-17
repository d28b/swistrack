#ifndef HEADER_ExecutionWaitForNextStep
#define HEADER_ExecutionWaitForNextStep

#include <wx/event.h>
#include "SwisTrackCore.h"

class ExecutionWaitForNextStep: public wxEvtHandler {

public:
	SwisTrackCore *mSwisTrackCore;

	//! Constructor.
	ExecutionWaitForNextStep(SwisTrackCore *stc);
	//! Destructor.
	~ExecutionWaitForNextStep();

	//! Executes the WaitForNextStep routine.
	void Execute();
	//! Executes the WaitForNextStep routine.
	void OnContinue(wxCommandEvent &ev);

private:
	SwisTrackCore::tComponentList::iterator mComponentIterator;

};

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_WAITFORNEXTSTEP_CONTINUE, -1)
END_DECLARE_EVENT_TYPES()

#endif
