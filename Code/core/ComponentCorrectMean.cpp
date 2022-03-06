#include "ComponentCorrectMean.h"
#define THISCLASS ComponentCorrectMean

#include <opencv2/core.hpp>
#include "DisplayEditor.h"

THISCLASS::ComponentCorrectMean(SwisTrackCore * stc):
	Component(stc, wxT("CorrectMean")),
	mColor(cv::Scalar(0, 0, 0)),
	mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentCorrectMean() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mColor = GetConfigurationColor(wxT("Color"), cv::Scalar(0, 0, 0));
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Scalar color = mColor - cv::mean(inputImage);
	cv::Mat outputImage = inputImage + color;
	mCore->mDataStructureImageColor.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}


