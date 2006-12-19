#include "ComponentInputCameraUSB.h"
#define THISCLASS ComponentInputCameraUSB

THISCLASS::ComponentInputCameraUSB(SwisTrackCore *stc):
		Component(stc, "CameraUSB"), mCapture(0), mLastImage(0) {

	// User-friendly information about this component
	mDisplayName="USB camera";
	AddDataStructureWrite(mCore->mDataStructureImage);
	AddDataStructureWrite(mCore->mDataStructureInput);
}

THISCLASS::~ComponentInputCameraUSB() {
	if (! mCapture) {return;}
	cvReleaseCapture(mCapture);
}

bool THISCLASS::Start() {
	mCapture = cvCaptureFromCAM(-1);
	if (! mCapture) {
		AddError("Could not open USB camera.");
		return false;
	}

	mLastImage = cvQueryFrame(mCapture);
	if (! mLastImage) {
		AddError("Could not retrieve image from USB camera.");
		return false;
	}

	mCore->mDataStructureImage.mIsInColor=(strcmp(mLastImage->colorModel, "GRAY")!=0);
}

bool THISCLASS::Step() {
	if (! mCapture) {return false;}	

	// Read from camera
	IplImage* inputimage = cvQueryFrame(Capture);
	if (! inputimage) {
		AddError("Could not retrieve image from USB camera.");
		return false;
	}

	// Set DataStructureImage
	mCore->mDataStructureImage.mImage=inputimage;	
	mCore->mDataStructureInput.mFrameNumber++;

	// Show status
	std::ostringstream oss;
	oss << "Frame " << mCore->mDataStructureImage.mFrameNumber;
	AddInfo(oss.str);
	return true;
}

bool THISCLASS::StepCleanup() {
	mCore->mDataStructureImage.mImage=0;
	if (mLastImage) {cvReleaseImage(mLastImage);}
	return true;
}

bool THISCLASS::Stop() {
	if (! mCapture) {return false;}	

	cvReleaseCapture(mCapture);
	return true;
}

double THISCLASS::GetFPS() {
	if (! mCapture) {return 0;}	
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_FPS);
}
