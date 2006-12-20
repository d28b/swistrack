#include "ComponentParticleFilter.h"
#define THISCLASS ComponentParticleFilter

THISCLASS::ComponentParticleFilter(SwisTrackCore *stc, const std::string &displayname):
		Component(stc, "ParticleFilter", displayname),
		mCapture(0), mLastImage(0) {

	// Data structure relations
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
