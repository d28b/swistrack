#include "ComponentInputCameraUSB.h"
#define THISCLASS ComponentInputCameraUSB

#include "DisplayEditor.h"
#include "ImageTools.h"

THISCLASS::ComponentInputCameraUSB(SwisTrackCore * stc):
	Component(stc, wxT("InputCameraUSB")),
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

	int width = GetConfigurationInt(wxT("FrameWidth"), 1920);
	int height = GetConfigurationInt(wxT("FrameHeight"), 1080);
	int fps = GetConfigurationInt(wxT("FramesPerSecond"), 10);
	mCapture.set(cv::CAP_PROP_FRAME_WIDTH, width);
	mCapture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
	mCapture.set(cv::CAP_PROP_FPS, fps);

	/*
	double fpsProp = mCapture.get(cv::CAP_PROP_FPS);
	double w = mCapture.get(cv::CAP_PROP_FRAME_WIDTH);
	double h = mCapture.get(cv::CAP_PROP_FRAME_HEIGHT);
	double format = mCapture.get(cv::CAP_PROP_FORMAT);
	double mode = mCapture.get(cv::CAP_PROP_MODE);
	double exposure = mCapture.get(cv::CAP_PROP_EXPOSURE);
	double autoExposure = mCapture.get(cv::CAP_PROP_AUTO_EXPOSURE);
	double zoom = mCapture.get(cv::CAP_PROP_ZOOM);
	double focus = mCapture.get(cv::CAP_PROP_FOCUS);
	double autoFocus = mCapture.get(cv::CAP_PROP_AUTOFOCUS);
	double bufferSize = mCapture.get(cv::CAP_PROP_BUFFERSIZE);
	double bitRate = mCapture.get(cv::CAP_PROP_BITRATE);

	printf("fps %0.3f\n", fpsProp);
	printf("size %0.3f x %0.3f\n", w, h);
	printf("format %0.3f\n", format);
	printf("mode %0.3f\n", mode);
	printf("exposure %0.3f %0.3f\n", exposure, autoExposure);
	printf("zoom %0.3f\n", zoom);
	printf("focus %0.3f %0.3f\n", focus, autoFocus);
	printf("buffer %0.3f\n", bufferSize);
	printf("bitrate %0.3f\n\n", bitRate);
	*/

	mFrameNumber = 0;
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	// Read from camera
	cv::Mat image;
	if (! mCapture.read(image)) {
		AddError(wxT("Could not retrieve image from USB camera."));
		return;
	}

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
