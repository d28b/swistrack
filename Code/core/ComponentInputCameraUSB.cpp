#include "ComponentInputCameraUSB.h"
#define THISCLASS ComponentInputCameraUSB

#include "DisplayEditor.h"
#include "ImageTools.h"

THISCLASS::ComponentInputCameraUSB(SwisTrackCore * stc):
	Component(stc, wxT("InputCameraUSB")),
	mFlipHorizontally(false), mFlipVertically(false),
	mCapture(), mFrameNumber(0),
	mDisplayOutput(wxT("Output"), wxT("USB Camera: Input Frame")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputCameraUSB() {
}

void THISCLASS::OnStart() {
	int cameraNumber = GetConfigurationInt(wxT("CameraNumber"), 0);
	if (cameraNumber < -1) {
		cameraNumber = -1;
		AddWarning(wxT("Using -1 as camera number."));
	}

	if (! mCapture.open(cameraNumber)) {
		AddError(wxT("Could not open USB camera."));
		return;
	}

	int fps = GetConfigurationInt(wxT("FramesPerSecond"), 10);
	mCapture.set(cv::CAP_PROP_FPS, fps);

	mFrameNumber = 0;
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mFlipHorizontally = GetConfigurationBool(wxT("FlipHorizontally"), false);
	mFlipVertically = GetConfigurationBool(wxT("FlipVertically"), false);
}

void THISCLASS::OnStep() {
	// Read from camera
	cv::Mat image;
	if (! mCapture.read(image)) {
		AddError(wxT("Could not retrieve image from USB camera."));
		return;
	}

	// Flip the image if desired
	image = ImageTools::Flip(image, mFlipHorizontally, mFlipVertically);

	// Set Timestamp for the current frame
	// This data is not read from the camera, so we will do the best we can: take a timestamp now.
	mCore->mDataStructureInput.SetFrameTimestamp(wxDateTime::UNow());

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage = image;
	mCore->mDataStructureInput.mFrameNumber = mFrameNumber;
	mFrameNumber += 1;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(image);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	mCapture.release();
}
