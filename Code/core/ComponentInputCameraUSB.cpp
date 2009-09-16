#include "ComponentInputCameraUSB.h"
#define THISCLASS ComponentInputCameraUSB

#include "DisplayEditor.h"

THISCLASS::ComponentInputCameraUSB(SwisTrackCore *stc):
		Component(stc, wxT("InputCameraUSB")),
		mFlipVertically(false),
		mCapture(0), mOutputImage(0), mFrameNumber(0),
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
	int camera_number = GetConfigurationInt(wxT("CameraNumber"), 0);
	if (camera_number < -1) {
		camera_number = -1;
		AddWarning(wxT("Using -1 as camera number."));
	}

	mCapture = cvCaptureFromCAM(camera_number);
	if (! mCapture) {
		AddError(wxT("Could not open USB camera."));
		return;
	}
	mFrameNumber = 0;
}

void THISCLASS::OnReloadConfiguration() {
	mFlipVertically = GetConfigurationBool(wxT("FlipVertically"), false);
}

void THISCLASS::OnStep() {
	if (! mCapture) {
		return;
	}

	// Read from camera
	mOutputImage = cvQueryFrame(mCapture);
	if (! mOutputImage) {
		AddError(wxT("Could not retrieve image from USB camera."));
		return;
	}

	// Flip the image if desired
	if (mFlipVertically) {
		cvFlip(mOutputImage, 0, 0);
	}

	// Set Timestamp for the current frame
	// this data is not read from the camera, so we will do the
	// best we can: take a timestamp now.
	mCore->mDataStructureInput.SetFrameTimestamp(wxDateTime::UNow());

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage = mOutputImage;
	mCore->mDataStructureInput.mFrameNumber = mFrameNumber++;

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
