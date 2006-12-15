#include "ComponentInputCameraUSB.h"
#define THISCLASS ComponentInputCameraUSB

THISCLASS::ComponentInputCameraUSB(xmlpp::Element* cfgRoot):
		mCapture(0) {

	mCapture = cvCaptureFromCAM(-1);
	if (! mCapture) {return;}

	mLastImage = cvQueryFrame(mCapture);
	if (! mLastImage) {return;}

	mCore->mDataStructureImage.mIsInColor=(strcmp(mLastImage->colorModel, "GRAY")!=0);
}

THISCLASS::~ComponentInputCameraUSB() {
	if (! mCapture) {return;}
	cvReleaseCapture(&Capture);	
}

void THISCLASS::SetParameters() {
}

bool THISCLASS::Step() {
	if (! mCapture) {return 0;}	

	IplImage* inputimage = cvQueryFrame(Capture);
	if (! inputimage) {return false;}

	mCore->mDataStructureImage.mImage=inputimage;	
	mCore->mDataStructureImage.mFrameNumber++;
	return true;
}

void THISCLASS::UpdateStatus() {
	ClearStatus();

	if (! mCapture) {
		AddError("Could not open USB camera.");
		return;
	}

	if (! mLastAcquiredImage) {
		AddError("Could not retrieve image from USB camera.");
		return;
	}

	std::ostringstream oss;
	oss << "Sampling at " << cvGetCaptureProperty(mCapture, CV_CAP_PROP_FPS) << " frames per second.";
	AddInfo(oss.str);
}

double THISCLASS::GetFPS() {
	if (! mCapture) {return 0;}	
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_FPS);
}
