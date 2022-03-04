#include "ComponentThresholdColorIndependent.h"
#define THISCLASS ComponentThresholdColorIndependent

#include "DisplayEditor.h"

THISCLASS::ComponentThresholdColorIndependent(SwisTrackCore * stc):
	Component(stc, wxT("ThresholdColorIndependent")),
	mBlueThreshold(128), mGreenThreshold(128), mRedThreshold(128), mOrBool(true), mInvertThreshold(false),
	mDisplayOutput(wxT("Output"), wxT("After thresholding")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryThresholdingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentThresholdColorIndependent() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mBlueThreshold = GetConfigurationInt(wxT("BlueThreshold"), 128);
	mGreenThreshold = GetConfigurationInt(wxT("GreenThreshold"), 128);
	mRedThreshold = GetConfigurationInt(wxT("RedThreshold"), 128);
	mOrBool = GetConfigurationBool(wxT("OrBool"), true);
	mInvertThreshold = GetConfigurationBool(wxT("InvertThreshold"), false);
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("Cannot access input image."));
		return;
	}

	if (inputImage.channels() != 3) {
		AddError(wxT("Input image must have 3 channels."));
		return;
	}

	cv::Mat channels[3];
	cv::split(inputImage, channels);
	cv::threshold(channels[0], channels[0], mRedThreshold, 255, mInvertThreshold ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY);
	cv::threshold(channels[1], channels[1], mGreenThreshold, 255, mInvertThreshold ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY);
	cv::threshold(channels[2], channels[2], mBlueThreshold, 255, mInvertThreshold ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY);

	cv::Mat outputImage = mOrBool ?
	                      channels[0] | channels[1] | channels[2] :
	                      channels[0] & channels[1] & channels[2];

	mCore->mDataStructureImageBinary.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageBinary.mImage = 0;
}

void THISCLASS::OnStop() {
}
