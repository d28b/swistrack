#include "ComponentFlipInput.h"
#define THISCLASS ComponentFlipInput

#include "DisplayEditor.h"
#include "ImageTools.h"

THISCLASS::ComponentFlipInput(SwisTrackCore * stc):
	Component(stc, wxT("FlipInput")),
	mFlipHorizontally(false), mFlipVertically(false),
	mDisplayOutput(wxT("Output"), wxT("After flipping")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentFlipInput() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mFlipHorizontally = GetConfigurationBool(wxT("FlipHorizontally"), false);
	mFlipVertically = GetConfigurationBool(wxT("FlipVertically"), false);
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureInput.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Flip the image if desired
	cv::Mat flippedImage = ImageTools::Flip(inputImage, mFlipHorizontally, mFlipVertically);
	mCore->mDataStructureInput.mImage = flippedImage;

	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(flippedImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

