#include "ComponentCalibrationLinear.h"
#define THISCLASS ComponentCalibrationLinear

#include <sstream>
#include "Random.h"

THISCLASS::ComponentCalibrationLinear(SwisTrackCore *stc):
		Component(stc, "CalibrationLinear"),
		mTopLeft(cvPoint2D32f(0, 0)), mTopRight(cvPoint2D32f(0, 0)), 
		mBottomLeft(cvPoint2D32f(0, 0)), mBottomRight(cvPoint2D32f(0, 0)) {

	// Data structure relations
	mDisplayName="Calibration with a linear model";
	mCategory=&(mCore->mCategoryCalibration);
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
}

THISCLASS::~ComponentCalibrationLinear() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mWorldTopLeft.x=(float)GetConfigurationDouble("WorldTopLeft.x", 0);
	mWorldTopLeft.y=(float)GetConfigurationDouble("WorldTopLeft.y", 0);
	mWorldTopRight.x=(float)GetConfigurationDouble("WorldTopRight.x", 0);
	mWorldTopRight.y=(float)GetConfigurationDouble("WorldTopRight.y", 0);
	mWorldBottomLeft.x=(float)GetConfigurationDouble("WorldBottomLeft.x", 0);
	mWorldBottomLeft.y=(float)GetConfigurationDouble("WorldBottomLeft.y", 0);
	mWorldBottomRight.x=(float)GetConfigurationDouble("WorldBottomRight.x", 0);
	mWorldBottomRight.y=(float)GetConfigurationDouble("WorldBottomRight.y", 0);
	mCameraTopLeft.x=(float)GetConfigurationDouble("CameraTopLeft.x", 0);
	mCameraTopLeft.y=(float)GetConfigurationDouble("CameraTopLeft.y", 0);
	mCameraBottomRight.x=(float)GetConfigurationDouble("CameraBottomRight.x", 0);
	mCameraBottomRight.y=(float)GetConfigurationDouble("CameraBottomRight.y", 0);
}

void THISCLASS::OnStep() {
	// These are the particles to transform
	DataStructureParticles::tParticleVector *particles=&(mCore->mDataStructureParticles.mParticles);

	// Transform all particle positions
	DataStructureParticles::tParticleVector::iterator it=particles->begin();
	while (it!=particles->end()) {
		TransformPosition(&*it);
		it++;
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

void THISCLASS::Transform(Particle *p) {
	CvPoint2D32f ratio;
	ratio.x=(p.mCenter.x-mCameraTopLeft.x)/(mCameraBottomRight.x-mCameraTopLeft.x);
	ratio.y=(p.mCenter.y-mCameraTopLeft.y)/(mCameraBottomRight.y-mCameraTopLeft.y);

	CvPoint2D32f ptop=ratio.x*(mWorldTopRight-mWorldTopLeft)+mWorldTopLeft;
	CvPoint2D32f pbottom=ratio.x*(mWorldBottomRight-mWorldBottomLeft)+mWorldBottomLeft;
	p->mCenter=ratio.y*(pbottom-ptop)+ptop;
}
