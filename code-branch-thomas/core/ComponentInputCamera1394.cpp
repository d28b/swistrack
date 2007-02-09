#include "ComponentInputCamera1394.h"
#define THISCLASS ComponentInputCamera1394

#ifdef _1394
#include <sstream>

THISCLASS::ComponentInputCamera1394(SwisTrackCore *stc):
		Component(stc, "Camera1394"),
		mCamera(), mCurrentImage(0), mFrameNumber(0),
		mDisplayImageOutput("Output", "Output") {

	// Data structure relations
	mDisplayName="1394 Camera";
	mCategory=&(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentInputCamera1394() {
}

void THISCLASS::OnStart() {
	if (mCamera.CheckLink() != CAM_SUCCESS) {
		AddError("Cannot access 1394 Camera (Make sure CMU driver is installed).");
		return;
	}

	if (mCamera.InitCamera() != CAM_SUCCESS) {
		AddError("Cannot initialize 1394 Camera.");
		return;
	}

	mCamera.StatusControlRegisters();

	int videoformat=GetConfigurationInt("VideoFormat", 0);
	if ((videoformat<0) || (videoformat>2)) {AddError("VideoMode must be 0, 1 or 2."); return;}
	mCamera.SetVideoFormat(videoformat);

	int videomode=GetConfigurationInt("VideoMode", 5);
	if ((videomode<0) || (videomode>5)) {AddError("VideoMode must be in [0, 5]."); return;}
	mCamera.SetVideoMode(videomode);
	
	mCamera.SetVideoFrameRate(GetConfigurationInt("FrameRate", 2)); 
	mCamera.m_controlGain.SetAutoMode(GetConfigurationInt("AutoGain", 1)); 
	mCamera.m_controlAutoExposure.TurnOn(GetConfigurationInt("AutoExposure", 1)); 
	mCamera.m_controlWhiteBalance.SetAutoMode(GetConfigurationInt("AutoWhiteBalance", 0));
	mCamera.SetBrightness(GetConfigurationInt("Brightness", 0));
	mCamera.SetAutoExposure(GetConfigurationInt("Exposure", 0));
	mCamera.SetSharpness(GetConfigurationInt("Sharpness", 0));
	mCamera.SetWhiteBalance(GetConfigurationInt("WhileBalance1", 0), GetConfigurationInt("WhileBalance2", 0));
	mCamera.SetSaturation(GetConfigurationInt("Saturation", 0));
	
	if (mCamera.StartImageAcquisition() != 0) {
		AddError("Could not start image acquisition.");
		return;
	}

	mFrameNumber=0;
	mCurrentImage=cvCreateImage(cvSize(mCamera.m_width, mCamera.m_height), 8, 3);
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	// Get image from camera
	mFrameNumber++;
	int status=mCamera.AcquireImage();	
	switch (status) {
	case CAM_SUCCESS:
		break;
	case CAM_ERROR_NOT_INITIALIZED:
		AddError("Camera not initialized.");
		return;
	default:
		AddError("Camera acquisition error.");
		return;
	}

	// Point the input IplImage to the camera buffer
	mCurrentImage->imageData=(char*)mCamera.m_pData;  FIXME

	// Convert the input to the right format (RGB to BGR)
	cvCvtColor(mCurrentImage, mCurrentImage, CV_RGB2BGR);
	
	// Set this image in the DataStructureImage
	mCore->mDataStructureInput.mImage=mCurrentImage;
	mCore->mDataStructureInput.mFrameNumber=mFrameNumber;

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mNewImage=mCurrentImage;
	std::ostringstream oss;
	oss << "Frame " << mFrameNumber << ", " << mCurrentImage->width << "x" << mCurrentImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mImage=0;
}

void THISCLASS::OnStop() {
}

#endif
