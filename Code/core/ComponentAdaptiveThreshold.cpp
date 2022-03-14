#include "ComponentAdaptiveThreshold.h"
#define THISCLASS ComponentAdaptiveThreshold

#include "DisplayEditor.h"
using namespace std;
#include <iostream>

THISCLASS::ComponentAdaptiveThreshold(SwisTrackCore * stc):
	Component(stc, wxT("AdaptiveThreshold")),
	mDisplayOutput(wxT("Output"), wxT("After thresholding")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentAdaptiveThreshold() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mThreshold = GetConfigurationInt(wxT("Threshold"), 255);
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Do the thresholding
	cv::Mat grayImage;
	cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);
	cv::Mat outputImage;
	cv::adaptiveThreshold(grayImage, outputImage, mThreshold, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 3, 5);
	mCore->mDataStructureImageBinary.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
