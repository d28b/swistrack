#include "ComponentBlobDetectionCircle.h"
#define THISCLASS ComponentBlobDetectionCircle

THISCLASS::ComponentBlobDetectionCircle(SwisTrackCore *stc):
		Component(stc, "BlobDetectionCircle"),
		mParticles(0) {

	// Data structure relations
	mCategory=&(mCore->mCategoryBlobDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBlobDetectionCircle() {
}

void THISCLASS::OnStart() {
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
