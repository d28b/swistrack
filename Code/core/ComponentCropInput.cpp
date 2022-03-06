#include "ComponentCropInput.h"
#define THISCLASS ComponentCropInput

#include "DisplayEditor.h"

THISCLASS::ComponentCropInput(SwisTrackCore * stc):
	Component(stc, wxT("CropInput")),
	mDisplayOutput(wxT("Output"), wxT("After cropping")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentCropInput() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mCropRect.x = GetConfigurationInt(wxT("Left"), 0);
	mCropRect.y = GetConfigurationInt(wxT("Top"), 0);
	mCropRect.width = GetConfigurationInt(wxT("Width"), 10000);
	mCropRect.height = GetConfigurationInt(wxT("Height"), 10000);

	if (mCropRect.x < 0) mCropRect.x = 0;
	if (mCropRect.y < 0) mCropRect.y = 0;
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureInput.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	int left = std::min(inputImage.cols - 1, mCropRect.x);
	int top = std::min(inputImage.rows - 1, mCropRect.y);
	int right = std::min(inputImage.cols, left + mCropRect.width);
	int bottom = std::min(inputImage.rows, top + mCropRect.height);
	cv::Mat croppedImage = inputImage(cv::Range(top, bottom), cv::Range(left, right));
	mCore->mDataStructureInput.mImage = croppedImage;

	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(croppedImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

