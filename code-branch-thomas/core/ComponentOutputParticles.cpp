#include "ComponentOutputParticles.h"
#define THISCLASS ComponentOutputParticles

THISCLASS::ComponentOutputParticles(SwisTrackCore *stc):
		Component(stc, "OutputParticles") {

	// Data structure relations
	mCategory=&(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureRead(&(mCore->mDataStructureParticles));

	// Read the XML configuration file
	Initialize();
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
	mCore->mCommunicationInterface->Send(&mbegin);

	DataStructureParticles::tParticleVector *p=mCore->mDataStructureParticles.mParticles;
	if (!p) {return;}
	DataStructureParticles::tParticleVector::iterator it=p->begin();
	while (it!=p->end()) {
		CommunicationMessage m("PARTICLE");
		m.AddInt(it->mID);
		m.AddDouble(it->mCenter.x);
		m.AddDouble(it->mCenter.y);
		m.AddDouble(it->mOrientation);
		mCore->mCommunicationInterface->Send(&m);
		it++;
	}

	CommunicationMessage mend("ENDFRAME");
	mCore->mCommunicationInterface->Send(&mend);
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
