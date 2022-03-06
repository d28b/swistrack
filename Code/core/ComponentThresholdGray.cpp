#include "ComponentThresholdGray.h"
#define THISCLASS ComponentThresholdGray

#include "DisplayEditor.h"

THISCLASS::ComponentThresholdGray(SwisTrackCore * stc):
	Component(stc, wxT("ThresholdGray")),
	mThreshold(128), mInvertThreshold(false),
	mDisplayOutput(wxT("Output"), wxT("After thresholding")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentThresholdGray() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mThreshold = GetConfigurationInt(wxT("Threshold"), 128);
	mInvertThreshold = GetConfigurationBool(wxT("InvertThreshold"), false);
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageGray.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage;
	cv::threshold(inputImage, outputImage, mThreshold, 255, mInvertThreshold ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY);

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
