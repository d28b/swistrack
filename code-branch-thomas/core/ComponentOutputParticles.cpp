#include "ComponentOutputParticles.h"
#define THISCLASS ComponentOutputParticles

THISCLASS::ComponentOutputParticles(SwisTrackCore *stc):
		Component(stc, "OutputParticles"),
		mCommunicationInterface(0) {

	// Data structure relations
	mDisplayName="Output particles";
	mCategory=mCore->mCategoryOutput;
	AddDataStructureRead(&(mCore->mDataStructureParticles));
}

THISCLASS::~ComponentOutputParticles() {
}

void THISCLASS::OnStart() {
	if (! mCommunicationInterface) {
		AddError("No communication interface set.");
		return;
	}
}

void THISCLASS::OnStep() {
	if (! mCommunicationInterface) {return;}

	DataStructureParticles::tParticleVector *p=mCore->mDataStructureParticles->mParticles;
	DataStructureParticles::tParticleVector::iterator it=p->begin();
	while (it!=p->end()) {
		CommunicationMessage m("PARTICLE");
		m.AddInt(it->mID);
		m.AddDouble(it->mCenter.x);
		m.AddDouble(it->mCenter.y);
		m.AddDouble(it->mOrientation);
		mCore->mCommunicationInterface->SendMessage(m);
	}

	CommunicationMessage m("ENDFRAME");
	mCore->mCommunicationInterface->SendMessage(m);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
