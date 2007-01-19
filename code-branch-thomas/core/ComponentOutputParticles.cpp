#include "ComponentOutputParticles.h"
#define THISCLASS ComponentOutputParticles

THISCLASS::ComponentOutputParticles(SwisTrackCore *stc):
		Component(stc, "OutputParticles") {

	// Data structure relations
	mDisplayName="Output particles";
	mCategory=&(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureRead(&(mCore->mDataStructureParticles));
}

THISCLASS::~ComponentOutputParticles() {
}

void THISCLASS::OnStart() {
	if (! mCore->mCommunicationInterface) {
		AddError("No communication interface set.");
		return;
	}
}

void THISCLASS::OnStep() {
	if (! mCore->mCommunicationInterface) {return;}

	CommunicationMessage mbegin("BEGINFRAME");
	mbegin.AddInt(mCore->mDataStructureInput.mFrameNumber);
	mCore->mCommunicationInterface->SendMessage(&mbegin);

	DataStructureParticles::tParticleVector *p=mCore->mDataStructureParticles.mParticles;
	DataStructureParticles::tParticleVector::iterator it=p->begin();
	while (it!=p->end()) {
		CommunicationMessage m("PARTICLE");
		m.AddInt(it->mID);
		m.AddDouble(it->mCenter.x);
		m.AddDouble(it->mCenter.y);
		m.AddDouble(it->mOrientation);
		mCore->mCommunicationInterface->SendMessage(&m);
	}

	CommunicationMessage mend("ENDFRAME");
	mCore->mCommunicationInterface->SendMessage(&mend);
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
