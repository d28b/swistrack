#include "ComponentColorGaussianBlur.h"
#define THISCLASS ComponentColorGaussianBlur

#include <opencv2/highgui.hpp>
#include <cctype>
#include <algorithm>
#include "DisplayEditor.h"

THISCLASS::ComponentColorGaussianBlur(SwisTrackCore * stc):
	Component(stc, wxT("ColorGaussianBlur")),
	mSigmaX(2.0), mSigmaY(2.0), mSizeX(5), mSizeY(5),
	mDisplayOutput(wxT("Output"), wxT("After blurring the image")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentColorGaussianBlur() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	// Sigma
	mSigmaX = GetConfigurationDouble(wxT("SigmaX"), 2.0);
	mSigmaY = GetConfigurationDouble(wxT("SigmaY"), 2.0);

	// Size
	mSizeX = GetConfigurationInt(wxT("SizeX"), 5);
	if (mSizeX < 1) mSizeX = 1;
	if (mSizeX % 2 == 0) mSizeX += 1;

	mSizeY = GetConfigurationInt(wxT("SizeY"), 5);
	if (mSizeY < 1) mSizeY = 1;
	if (mSizeY % 2 == 0) mSizeY += 1;
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage(inputImage.size(), CV_8UC3);
	cv::GaussianBlur(inputImage, outputImage, cv::Size(mSizeX, mSizeY), mSigmaX, mSigmaY);
	mCore->mDataStructureImageColor.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
