#include "ComponentInputCamera1394.h"
#define THISCLASS ComponentInputCamera1394

#ifdef _1394

THISCLASS::ComponentInputCamera1394(SwisTrackCore *stc):
		Component(stc, "Camera1394"), mCamera(0), mLastImage(0) {

	mDisplayName="1394 camera";
}

bool THISCLASS::Start() {
	if (mCamera.CheckLink() != CAM_SUCCESS) {
		AddError("Cannot access 1394 Camera (Make sure CMU driver is installed).");
		return;
	}

	if (mCamera.InitCamera() != CAM_SUCCESS) {
		AddError("Cannot initialize 1394 Camera.";
		return;
	}

	mCamera.StatusControlRegisters();

	int videoformat=GetConfigurationInt("VideoFormat", 0);
	if ((videoformat<0) || (videoformat>2)) {AddError("VideoMode must be 0, 1 or 2."); return false;}
	mCamera.SetVideoFormat(videoformat);

	int videomode=GetConfigurationInt("VideoMode", 0);
	if ((videomode<0) || (videomode>5)) {AddError("VideoMode must be in [0, 5]."); return false;}
	mCamera.SetVideoMode(videomode);
	
	mCamera.SetVideoFrameRate(GetConfigurationInt("FrameRate", 10)); 
	mCamera.m_controlGain.SetAutoMode(GetConfigurationInt("AutoGain", 1)); 
	mCamera.m_controlAutoExposure.TurnOn(GetConfigurationInt("AutoExposure", 1)); 
	mCamera.m_controlWhiteBalance.SetAutoMode(GetConfigurationInt("AutoWhiteBalance", 0));
	mCamera.SetBrightness(GetConfigurationInt("Brightness", 0));
	mCamera.SetAutoExposure(GetConfigurationInt("Exposure", 0));
	mCamera.SetSharpness(GetConfigurationInt("Sharpness", 0));
	mCamera.SetWhiteBalance(GetConfigurationInt("WhileBalance1", 0), GetConfigurationInt("WhileBalance2", 0));
	mCamera.SetSaturation(GetConfigurationInt("Saturation", 0));
	
	if (mCamera.StartImageAcquisition() != 0) {
		AddError("Could not start image acquisition.";
		return false;
	}

	mLastImage=cvCreateImage(cvSize(mCamera.m_width, mCamera.m_height), 8, 3);
	mCore->mDataStructureImage.mImage=mLastImage;
	mCore->mDataStructureImage.mIsInColor=true;
	return true;
}

bool THISCLASS::Step() {
	// Get image from camera
	int status=mCamera.AcquireImage();	
	switch (status) {
	case CAM_SUCCESS:
		break;
	case CAM_ERROR_NOT_INITIALIZED:
		AddError("Camera not initialized.");
		return false;
	default:
		AddError("Camera acquisition error.");
		return false;
	}

	// Point the input IplImage to the camera buffer
	mLastImage->imageData=(char*)mCamera.m_pData;

	// Convert the input in the right format (RGB to BGR)
	cvCvtColor(mLastImage, mLastImage, CV_RGB2BGR);
	
	// Set this image in the DataStructureImage
	mCore->mDataStructureImage.mImage=mLastImage;
	mCore->mDataStructureImage.mFrameNumber++;
	return true;
}

bool THISCLASS::Stop() {
	return true;
}

#endif
