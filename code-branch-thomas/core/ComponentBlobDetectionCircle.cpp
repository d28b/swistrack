#include "ComponentBlobDetectionCircle.h"
#define THISCLASS ComponentBlobDetectionCircle

THISCLASS::ComponentBlobDetectionCircle(SwisTrackCore *stc):
		Component(stc, "ParticleFilter"),
		mParticles(0) {

	// Data structure relations
	mDisplayName="Blob detection";
	mCategory=&(mCore->mCategoryBlobDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
}

THISCLASS::~ComponentBlobDetectionCircle() {
}

void THISCLASS::OnStart() {
}

void THISCLASS::OnStep() {
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
