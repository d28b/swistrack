#include "ComponentInputCamera1394.h"
#define THISCLASS ComponentInputCamera1394

#ifdef USE_CAMERA_CMU_1394
#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentInputCamera1394(SwisTrackCore *stc):
		Component(stc, "InputCamera1394"),
		mCamera(), mOutputImage(0), mFrameNumber(0),
		mDisplayOutput("Output", "1394 Camera: Input Frame") {

	// Data structure relations
	mDisplayName="1394 Camera";
	mCategory=&(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplay(&mDisplayOutput);
}

THISCLASS::~ComponentInputCamera1394() {
}

void THISCLASS::OnStart() {
	if (mCamera.CheckLink() != CAM_SUCCESS) {
		AddError("Cannot access 1394 Camera. Make sure the CMU driver is installed.");
		return;
	}

	int res=mCamera.InitCamera();
	if (res != CAM_SUCCESS) {
		std::ostringstream oss;
		oss << "Cannot initialize the camera (Error " << res << ").";
		AddError(oss.str());
		return;
	}

	mCamera.StatusControlRegisters();

	int videoformat=GetConfigurationInt("VideoFormat", 1);
	if ((videoformat<0) || (videoformat>2)) {AddError("VideoFormat must be 0, 1 or 2."); return;}
	mCamera.SetVideoFormat(videoformat);

	int videomode=GetConfigurationInt("VideoMode", 4);
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
	mOutputImage=cvCreateImage(cvSize(mCamera.m_width, mCamera.m_height), 8, 3);
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
	mCamera.getRGB((unsigned char *)mOutputImage->imageData);
	//mOutputImage->imageData=(char*)mCamera.m_pData;  // doesn't work because the data doesn't have the same format

	// Convert the input to the right format (RGB to BGR)
	cvCvtColor(mOutputImage, mOutputImage, CV_RGB2BGR);

	// Set this image in the DataStructureImage
	mCore->mDataStructureInput.mImage=mOutputImage;
	mCore->mDataStructureInput.mFrameNumber=mFrameNumber;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mOutputImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mImage=0;
}

void THISCLASS::OnStop() {
	if (mCamera.StopImageAcquisition() != 0) {
		AddError("Could not stop image acquisition.");
		return;
	}
}

#endif
