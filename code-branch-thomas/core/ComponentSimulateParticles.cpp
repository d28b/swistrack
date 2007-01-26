#include "ComponentSimulateParticles.h"
#define THISCLASS ComponentSimulateParticles

THISCLASS::ComponentSimulateParticles(SwisTrackCore *stc):
		Component(stc, "ParticleFilter"),
		mMinArea(0), mMaxArea(1000000), mMaxNumber(10), mFirstDilate(1), mFirstErode(1), mSecondDilate(1) {

	// Data structure relations
	mDisplayName="Blob detection with min/max particle size";
	mCategory=&(mCore->mCategoryBlobDetection);
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
}

THISCLASS::~ComponentSimulateParticles() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
	mNumParticles=GetConfigurationDouble("NumParticles", 10);	

	// Check for stupid configurations
	if (mNumParticles<1) {
		AddError("You must choose at least one particle.");
	}

	// Create the robots at their initial position
	
	mParticles
}

void THISCLASS::OnReloadConfiguration() {
	mCameraOrigin.x=GetConfigurationDouble("CameraOrigin.x", 0);
	mCameraOrigin.y=GetConfigurationDouble("CameraOrigin.y", 0);
	mCameraRotation=GetConfigurationDouble("CameraRotation", 0);
	mCameraPixelSize=GetConfigurationDouble("CameraPixelSize", 1);
	mNoise=GetConfigurationDouble("Noise", 0);

	// Check for stupid configurations
	if (mCameraPixelSize<0) {
		AddError("The pixel size must be bigger than 0.");
	}
}

void THISCLASS::OnStep() {
	

	// Set these particles
	mCore->mDataStructureParticles.mParticles=&mParticles;
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles=0;
}

void THISCLASS::OnStop() {
}

double THISCLASS::GetContourCompactness(const void* contour) {
	double l = cvArcLength(contour, CV_WHOLE_SEQ, 1);
	return fabs(12.56*cvContourArea(contour)/(l*l));	
}
