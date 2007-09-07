#include "ComponentOutputMarkFrameManual.h"
#define THISCLASS ComponentOutputMarkFrameManual

THISCLASS::ComponentOutputMarkFrameManual(SwisTrackCore *stc):
		Component(stc, "OutputMarkFrameManual"), mMarkerName(""), mMark("") {

	// Data structure relations
	mCategory=&(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureRead(&(mCore->mDataStructureMarkFrameManual));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputMarkFrameManual() {
}

void THISCLASS::OnStart() {
	if (! mCore->mCommunicationInterface) {
		AddError("No communication interface set.");
		return;
	}

	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mMarkerName=GetConfigurationBool("MarkerName", false);
}

void THISCLASS::OnStep() {
	if (! mCore->mCommunicationInterface) {return;}

	// Get the marker (and return if there is none)
	std::string mark=GetConfigurationString("Mark", "");
	if (mark.length()==0) {return;}
	mConfiguration["Mark"]="";

	// Write a marker
	CommunicationMessage m("MARK");
	m.AddString(mMarkerName);
	m.AddString(mMark);
	mCore->mCommunicationInterface->Send(&m);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
