#include "ExecutionWaitForNextStep.h"
#define THISCLASS ExecutionWaitForNextStep

DEFINE_EVENT_TYPE(wxEVT_WAITFORNEXTSTEP_CONTINUE)

THISCLASS::ExecutionWaitForNextStep(SwisTrackCore *stc) {
	Connect(wxID_ANY, wxEVT_WAITFORNEXTSTEP_CONTINUE, wxCommandEventHandler(THISCLASS::OnContinue));
}

THISCLASS::~ExecutionWaitForNextStep() {
}

void THISCLASS::Execute() {
	//if (mSwisTrackCore->mExecution) {return;}
	if (! mSwisTrackCore->mStarted) {return;}

	// Set me to be executed
	//mSwisTrackCore->mExecution=this;

	// Initialize
	mComponentIterator=mSwisTrackCore->mDeployedComponents.begin();

	// Start the execution
	wxCommandEvent newevent(wxEVT_WAITFORNEXTSTEP_CONTINUE);
	AddPendingEvent(newevent);
}

void THISCLASS::OnContinue(wxCommandEvent &ev) {
	// Execute one step
	(*mComponentIterator)->ClearStatus();
	(*mComponentIterator)->OnWaitForNextStep();
	mComponentIterator++;

	wxCommandEvent newevent(wxEVT_WAITFORNEXTSTEP_CONTINUE);
	AddPendingEvent(newevent);
}
