#include "ComponentInputCameraUSB.h"
#define THISCLASS ComponentInputCameraUSB

THISCLASS::ComponentInputCameraUSB(SwisTrackCore *stc):
		Component(stc, "CameraUSB"),
		mCapture(0), mLastImage(0) {

	// Data structure relations
	mDisplayName="USB Camera";
	mCategory=mCore->mCategoryInput;
	AddDataStructureWrite(&(mCore->mDataStructureInput));
}

THISCLASS::~ComponentInputCameraUSB() {
	if (! mCapture) {return;}
	cvReleaseCapture(&mCapture);
}

void THISCLASS::OnStart() {
	mCapture = cvCaptureFromCAM(-1);
	if (! mCapture) {
		AddError("Could not open USB camera.");
		return;
	}

	mLastImage = cvQueryFrame(mCapture);
	if (! mLastImage) {
		AddError("Could not retrieve image from USB camera.");
		return;
	}
}

void THISCLASS::OnStep() {
	if (! mCapture) {return;}	

	// Read from camera
	mLastImage = cvQueryFrame(mCapture);
	if (! mLastImage) {
		AddError("Could not retrieve image from USB camera.");
		return;
	}

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage=mLastImage;	
	mCore->mDataStructureInput.mFrameNumber++;
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mImage=0;
	if (mLastImage) {cvReleaseImage(&mLastImage);}
}

void THISCLASS::OnStop() {
	if (! mCapture) {return;}

	cvReleaseCapture(&mCapture);
}

double THISCLASS::GetFPS() {
	if (! mCapture) {return 0;}	
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_FPS);
}
