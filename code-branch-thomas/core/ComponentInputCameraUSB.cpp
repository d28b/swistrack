#include "ComponentInputCameraUSB.h"
#define THISCLASS ComponentInputCameraUSB

#include <sstream>

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

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	if (! mCapture) {return;}	

	// Read from camera
	int framenumber=(int)cvGetCaptureProperty(mCapture, CV_CAP_PROP_POS_FRAMES);
	mCurrentImage=cvQueryFrame(mCapture);
	if (! mCurrentImage) {
		AddError("Could not retrieve image from USB camera.");
		return;
	}

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage=mCurrentImage;
	mCore->mDataStructureInput.mFrameNumber=framenumber;

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=mCurrentImage;
	std::ostringstream oss;
	oss << "Frame " << framenumber << ", " << mCurrentImage->width << "x" << mCurrentImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mImage=0;
	//mCurrentImage should not be released here, as this is handled by the HighGUI library
}

void THISCLASS::OnStop() {
	if (! mCapture) {return;}

	cvReleaseCapture(&mCapture);
}

double THISCLASS::GetFPS() {
	if (! mCapture) {return 0;}	
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_FPS);
}
