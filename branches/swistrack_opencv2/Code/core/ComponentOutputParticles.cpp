#include "ComponentOutputParticles.h"
#define THISCLASS ComponentOutputParticles

THISCLASS::ComponentOutputParticles(SwisTrackCore *stc):
		Component(stc, wxT("OutputParticles")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureRead(&(mCore->mDataStructureParticles));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputParticles() {
}

void THISCLASS::OnStart() {
	if (! mCore->mCommunicationInterface) {
		AddError(wxT("No communication interface set."));
		return;
	}
}

void THISCLASS::OnStep() {
	if (! mCore->mCommunicationInterface) {
		return;
	}

	CommunicationMessage mbegin(wxT("FRAMENUMBER"));
	mbegin.AddInt(mCore->mDataStructureInput.mFrameNumber);
	mCore->mCommunicationInterface->Send(&mbegin);

	DataStructureParticles::tParticleVector *p = mCore->mDataStructureParticles.mParticles;
	if (!p) {
		return;
	}
	DataStructureParticles::tParticleVector::iterator it = p->begin();
	while (it != p->end()) {
		CommunicationMessage m(wxT("PARTICLE"));
		m.AddInt(it->mID);
		m.AddDouble(it->mCenter.x);
		m.AddDouble(it->mCenter.y);
		m.AddDouble(it->mOrientation);
		m.AddDouble(it->mWorldCenter.x);
		m.AddDouble(it->mWorldCenter.y);
		mCore->mCommunicationInterface->Send(&m);
		it++;
	}
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
