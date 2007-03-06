#include "ComponentBlobDetectionCircle.h"
#define THISCLASS ComponentBlobDetectionCircle

THISCLASS::ComponentBlobDetectionCircle(SwisTrackCore *stc):
		Component(stc, "BlobDetectionCircle"),
		mParticles(0) {

	// Data structure relations
	mDisplayName="Blob detection for circular blobs";
	mCategory=&(mCore->mCategoryBlobDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
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
