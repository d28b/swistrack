#include "ComponentInputCameraUSB.h"
#define THISCLASS ComponentInputCameraUSB

#include "DisplayEditor.h"

THISCLASS::ComponentInputCameraUSB(SwisTrackCore *stc):
		Component(stc, wxT("InputCameraUSB")),
		mCapture(0), mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("USB Camera: Input Frame")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputCameraUSB() {
	if (! mCapture) {
		return;
	}
	cvReleaseCapture(&mCapture);
}

void THISCLASS::OnStart() {
	mCapture = cvCaptureFromCAM(-1);
	if (! mCapture) {
		AddError(wxT("Could not open USB camera."));
		return;
	}

	mOutputImage = cvQueryFrame(mCapture);
	if (! mOutputImage) {
		AddError(wxT("Could not retrieve image from USB camera."));
		return;
	}
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	if (! mCapture) {
		return;
	}

	// Read from camera
	int framenumber = (int)cvGetCaptureProperty(mCapture, CV_CAP_PROP_POS_FRAMES);
	mOutputImage = cvQueryFrame(mCapture);
	if (! mOutputImage) {
		AddError(wxT("Could not retrieve image from USB camera."));
		return;
	}

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage = mOutputImage;
	mCore->mDataStructureInput.mFrameNumber = framenumber;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mOutputImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mImage = 0;
	//mOutputImage should not be released here, as this is handled by the HighGUI library
}

void THISCLASS::OnStop() {
	if (! mCapture) {
		return;
	}

	cvReleaseCapture(&mCapture);
}

double THISCLASS::GetFPS() {
	if (! mCapture) {
		return 0;
	}
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_FPS);
}
