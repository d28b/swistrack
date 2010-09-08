#include "ComponentOutputMarkFrameManual.h"
#define THISCLASS ComponentOutputMarkFrameManual

THISCLASS::ComponentOutputMarkFrameManual(SwisTrackCore *stc):
		Component(stc, wxT("OutputMarkFrameManual")), mMarkerName(wxT("")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputMarkFrameManual() {
}

void THISCLASS::OnStart() {
	if (! mCore->mCommunicationInterface) {
		AddError(wxT("No communication interface set."));
		return;
	}

	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mMarkerName = GetConfigurationString(wxT("MarkerName"), wxT(""));
}

void THISCLASS::OnStep() {
	if (! mCore->mCommunicationInterface) {
		return;
	}

	// Get the marker (and return if there is none)
	wxString mark = GetConfigurationString(wxT("Mark"), wxT(""));
	if (mark.Len() == 0) {
		return;
	}
	mConfiguration[wxT("Mark")] = wxT("");

	// Write a marker
	CommunicationMessage m(wxT("MARK"));
	m.AddString(mMarkerName);
	m.AddString(mark);
	mCore->mCommunicationInterface->Send(&m);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
