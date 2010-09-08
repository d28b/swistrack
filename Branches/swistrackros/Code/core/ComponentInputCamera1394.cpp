#include "ComponentInputCamera1394.h"
#define THISCLASS ComponentInputCamera1394

#ifdef USE_CAMERA_CMU_1394
#include "DisplayEditor.h"

THISCLASS::ComponentInputCamera1394(SwisTrackCore *stc):
		Component(stc, wxT("InputCamera1394")),
		mCamera(), mOutputImage(0), mFrameNumber(0),
		mDisplayOutput(wxT("Output"), wxT("1394 Camera: Input Frame")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputCamera1394() {
}

void THISCLASS::OnStart() {
	if (mCamera.CheckLink() != CAM_SUCCESS) {
		AddError(wxT("Cannot access 1394 Camera. Make sure the CMU driver is installed."));
		return;
	}
	int num_cameras = mCamera.GetNumberCameras();
	if (mCamera.GetNumberCameras() == 0) {
		AddError(wxT("No camera detected"));
		return;
	}
	if (mCamera.SelectCamera(GetConfigurationInt(wxT("CameraNumber"), 0) != CAM_SUCCESS)) {
		AddError(wxT("The specified camera is not available"));
		return;
	}
	if (mCamera.InitCamera() != CAM_SUCCESS) {
		AddError(wxT("The specified camera cannot be initialized"));
		return;
	}
	if (mCamera.SetVideoFormat(GetConfigurationInt(wxT("VideoFormat"), 0)) != CAM_SUCCESS) {
		AddError(wxT("The specified format is not available"));
		return;
	}
	if (mCamera.SetVideoMode(GetConfigurationInt(wxT("VideoMode"), 5)) != CAM_SUCCESS) {
		AddError(wxT("The specified mode is not available"));
		return;
	}
	int toto = GetConfigurationInt(wxT("FrameRate"), 2);
	if (mCamera.SetVideoFrameRate(GetConfigurationInt(wxT("FrameRate"), 4)) != CAM_SUCCESS) {
		AddError(wxT("The specified frame rate is not available"));
		return;
	}
	if (mCamera.StartImageAcquisition() != 0) {
		AddError(wxT("Could not start image acquisition."));
		return;
	}

	// Getting loaded parameters
	int videoFormat = mCamera.GetVideoFormat();
	int videoMode = mCamera.GetVideoMode();
	int videoFrameRate = mCamera.GetVideoFrameRate();
	int nbChannels;
	int dataDepth;

	// Getting nbChannels
	if ((videoFormat == 0 && videoMode == 5) ||	// 8bits
	        (videoFormat == 1 && videoMode == 2) || (videoFormat == 1 && videoMode == 5) ||
	        (videoFormat == 2 && videoMode == 2) || (videoFormat == 2 && videoMode == 5) ||
	        (videoFormat == 0 && videoMode == 6) ||	// 16bits
	        (videoFormat == 1 && videoMode == 6) || (videoFormat == 1 && videoMode == 7) ||
	        (videoFormat == 2 && videoMode == 6) || (videoFormat == 2 && videoMode == 7)
	   ) {
		nbChannels = 1;
	} else {
		nbChannels = 3;
	}

	// Getting dataDepth (in byte)
	if ((videoFormat == 0 && videoMode == 6) ||	// 16bits
	        (videoFormat == 1 && videoMode == 6) || (videoFormat == 1 && videoMode == 7) ||
	        (videoFormat == 2 && videoMode == 6) || (videoFormat == 2 && videoMode == 7)
	   ) {
		dataDepth = 2;
	} else {
		dataDepth = 1;
	}

	// Drop first frame (usually blurred) and test about timeout problem
	if (mCamera.AcquireImage() == CAM_ERROR_FRAME_TIMEOUT) {
		AddError("Timeout Problem during Acquisition of First Image -> Restart Init");
		return;
	}

	unsigned long imageWidth, imageHeight;
	mCamera.GetVideoFrameDimensions(&imageWidth, &imageHeight);

	if (dataDepth == 1) {
		mOutputImage = cvCreateImage(cvSize((int)imageWidth, (int)imageHeight), IPL_DEPTH_8U, nbChannels);
	} else if (dataDepth == 2) {
		mOutputImage = cvCreateImage(cvSize((int)imageWidth, (int)imageHeight), IPL_DEPTH_16U, nbChannels);
	} else {
		AddError(wxT("Illegal depth"));
		return;
	}
	if (nbChannels == 3) {
		strcpy(mOutputImage->channelSeq, "RGB");
	}
	mFrameNumber = 0;
}

void THISCLASS::OnReloadConfiguration() {
	if (GetConfigurationBool(wxT("ConfigurationWindow"), false)) {
		CameraControlDialog(NULL, &mCamera, true);
		mConfiguration[wxT("ConfigurationWindow")] = wxT("false");  // reset value to false
	}
}

void THISCLASS::OnStep() {
	// Get image from camera
	int status = mCamera.AcquireImage();
	switch (status) {
	case CAM_SUCCESS:
		break;
	case CAM_ERROR_NOT_INITIALIZED:
		AddError(wxT("Camera not initialized."));
		return;
	default:
		AddError(wxT("Camera acquisition error."));
		return;
	}


	if (mOutputImage->nChannels == 1) {
		//Gray or Bayer color image !
		unsigned long rawDataLength;
		unsigned char *rawDataPtr;

		rawDataPtr = mCamera.GetRawData(&rawDataLength);
		memcpy(mOutputImage->imageData, rawDataPtr, rawDataLength);
	} else {
		// Point the input IplImage to the camera buffer
		mCamera.getRGB((unsigned char *)mOutputImage->imageData, mOutputImage->nChannels*mOutputImage->depth / 8*mOutputImage->height*mOutputImage->width);
	}

	// Set Timestamp for the current frame
	// this data is not read from the camera, so we will do the
	// best we can: take a timestamp now.
	mCore->mDataStructureInput.SetFrameTimestamp(wxDateTime::UNow());

	// Set this image in the DataStructureImage
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
}

void THISCLASS::OnStop() {
	if (mCamera.StopImageAcquisition() != 0) {
		AddError(wxT("Could not stop image acquisition."));
		return;
	}
	if (mOutputImage) {
		cvReleaseImage(&mOutputImage);
	}
}

#endif
