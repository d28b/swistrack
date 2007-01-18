#include "ComponentInputCameraUSB.h"
#define THISCLASS ComponentInputCameraUSB

THISCLASS::ComponentInputCameraUSB(SwisTrackCore *stc):
		Component(stc, "CameraUSB"),
		mCapture(0), mCurrentImage(0),
		mDisplayImageOutput("Output", "Output") {

	// Data structure relations
	mDisplayName="USB Camera";
	mCategory=&(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplayImage(&mDisplayImageOutput);
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

	mCurrentImage = cvQueryFrame(mCapture);
	if (! mCurrentImage) {
		AddError("Could not retrieve image from USB camera.");
		return;
	}
}

void THISCLASS::OnStep() {
	if (! mCapture) {return;}	

	// Read from camera
	mCurrentImage = cvQueryFrame(mCapture);
	if (! mCurrentImage) {
		AddError("Could not retrieve image from USB camera.");
		return;
	}

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage=mCurrentImage;
	mCore->mDataStructureInput.mFrameNumber++;

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mNewImage=mCurrentImage;
	//std::ostringstream oss;
	//oss << "Frame " << mCore->mDataStructureInput.mFrameNumber;
	//mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mImage=0;
	if (mCurrentImage) {cvReleaseImage(&mCurrentImage);}
}

void THISCLASS::OnStop() {
	if (! mCapture) {return;}

	cvReleaseCapture(&mCapture);
}

double THISCLASS::GetFPS() {
	if (! mCapture) {return 0;}	
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_FPS);
}
