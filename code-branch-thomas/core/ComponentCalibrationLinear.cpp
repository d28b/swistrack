#include "ComponentCalibrationLinear.h"
#define THISCLASS ComponentCalibrationLinear

#include "DisplayEditor.h"
#include "DataStructureParticles.h"

THISCLASS::ComponentCalibrationLinear(SwisTrackCore *stc):
		Component(stc, "CalibrationLinear"),
		mWorldTopLeft(cvPoint2D32f(0, 0)), mWorldTopRight(cvPoint2D32f(1, 0)), 
		mWorldBottomLeft(cvPoint2D32f(0, 1)), mWorldBottomRight(cvPoint2D32f(1, 1)),
		mCameraTopLeft(cvPoint2D32f(0, 0)), mCameraBottomRight(cvPoint2D32f(1, 1)),
		mDisplayOutput("Output", "Linear Calibration: Output") {

	// Data structure relations
	mCategory=&(mCore->mCategoryCalibration);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentCalibrationLinear() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mWorldTopLeft.x=(float)GetConfigurationDouble("WorldTopLeft.x", 0);
	mWorldTopLeft.y=(float)GetConfigurationDouble("WorldTopLeft.y", 0);
	mWorldTopRight.x=(float)GetConfigurationDouble("WorldTopRight.x", 1);
	mWorldTopRight.y=(float)GetConfigurationDouble("WorldTopRight.y", 0);
	mWorldBottomLeft.x=(float)GetConfigurationDouble("WorldBottomLeft.x", 0);
	mWorldBottomLeft.y=(float)GetConfigurationDouble("WorldBottomLeft.y", 1);
	mWorldBottomRight.x=(float)GetConfigurationDouble("WorldBottomRight.x", 1);
	mWorldBottomRight.y=(float)GetConfigurationDouble("WorldBottomRight.y", 1);
	mCameraTopLeft.x=(float)GetConfigurationDouble("CameraTopLeft.x", 0);
	mCameraTopLeft.y=(float)GetConfigurationDouble("CameraTopLeft.y", 0);
	mCameraBottomRight.x=(float)GetConfigurationDouble("CameraBottomRight.x", 1);
	mCameraBottomRight.y=(float)GetConfigurationDouble("CameraBottomRight.y", 1);
}

void THISCLASS::OnStep() {
	// These are the particles to transform
	DataStructureParticles::tParticleVector *particles=mCore->mDataStructureParticles.mParticles;
	if (! particles) {return;}

	// Transform all particle positions
	DataStructureParticles::tParticleVector::iterator it=particles->begin();
	while (it!=particles->end()) {
		Transform(&*it);
		it++;
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		//de.SetMode(DisplayEditor::sModeWorldCoordinates);
		de.SetArea(mWorldTopLeft, mWorldBottomRight);
		de.SetParticles(particles);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

void THISCLASS::Transform(Particle *p) {
	CvPoint2D32f ratio;
	ratio.x=(p->mCenter.x-mCameraTopLeft.x)/(mCameraBottomRight.x-mCameraTopLeft.x);
	ratio.y=(p->mCenter.y-mCameraTopLeft.y)/(mCameraBottomRight.y-mCameraTopLeft.y);

	CvPoint2D32f ptop;
	ptop.x=ratio.x*(mWorldTopRight.x-mWorldTopLeft.x)+mWorldTopLeft.x;
	ptop.y=ratio.x*(mWorldTopRight.y-mWorldTopLeft.y)+mWorldTopLeft.y;

	CvPoint2D32f pbottom;
	pbottom.x=ratio.x*(mWorldBottomRight.x-mWorldBottomLeft.x)+mWorldBottomLeft.x;
	pbottom.y=ratio.x*(mWorldBottomRight.y-mWorldBottomLeft.y)+mWorldBottomLeft.y;

	p->mCenter.x=ratio.y*(pbottom.x-ptop.x)+ptop.x;
	p->mCenter.y=ratio.y*(pbottom.y-ptop.y)+ptop.y;
}
