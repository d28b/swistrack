#include "ComponentParticleFilter.h"
#define THISCLASS ComponentParticleFilter

THISCLASS::ComponentParticleFilter(SwisTrackCore *stc):
		Component(stc, "ParticleFilter"), mCapture(0), mLastImage(0) {

	// User-friendly information about this component
	mDisplayName="Particle filter";
	AddDataStructureWrite(mCore->mDataStructureParticles);
	AddDataStructureWrite(mCore->mDataStructureParticles);
}

THISCLASS::~ComponentParticleFilter() {
}

bool THISCLASS::Start() {
	return true;
}

bool THISCLASS::Step() {
	return true;
}

bool THISCLASS::StepCleanup() {
	return true;
}

bool THISCLASS::Stop() {
	return true;
}
