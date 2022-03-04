#include "ComponentDoubleThresholdColorIndependent.h"
#define THISCLASS ComponentDoubleThresholdColorIndependent

#include "DisplayEditor.h"

THISCLASS::ComponentDoubleThresholdColorIndependent(SwisTrackCore * stc):
	Component(stc, wxT("DoubleThresholdColorIndependent")),
	mLowThreshold(0, 0, 0), mHighThreshold(0, 0, 0),
	mDisplayOutput(wxT("Output"), wxT("After thresholding")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryThresholdingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentDoubleThresholdColorIndependent() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mLowThreshold.val[0] = GetConfigurationInt(wxT("BlueLowThreshold"), 0);
	mLowThreshold.val[1] = GetConfigurationInt(wxT("GreenLowThreshold"), 0);
	mLowThreshold.val[2] = GetConfigurationInt(wxT("RedLowThreshold"), 0);
	//1 is added because of the way cvInRangeS makes the comparison
	mHighThreshold.val[0] = GetConfigurationInt(wxT("BlueHighThreshold"), 255) + 1;
	mHighThreshold.val[1] = GetConfigurationInt(wxT("GreenHighThreshold"), 255) + 1;
	mHighThreshold.val[2] = GetConfigurationInt(wxT("RedHighThreshold"), 255) + 1;
	strcpy(thresholdColorSeq, "BGR");
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("Cannot access Input image."));
		return;
	}

	cv::Mat outputImage(inputImage.size(), CV_8UC1);
	cv::inRange(inputImage, mLowThreshold, mHighThreshold, outputImage);

	mCore->mDataStructureImageBinary.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
