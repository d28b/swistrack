#include "ComponentCrop.h"
#define THISCLASS ComponentCrop

#include <opencv2/highgui.hpp>
#include "DisplayEditor.h"

using namespace cv;
using namespace std;

THISCLASS::ComponentCrop(SwisTrackCore * stc):
	Component(stc, wxT("Resize")),
	mDisplayOutput(wxT("Output"), wxT("Resized")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();

}

THISCLASS::~ComponentCrop() {
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
	cv::Mat inputImage = mCore->mDataStructureImageGray.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	int left = min(inputImage.cols - 1, mCropRect.x);
	int top = min(inputImage.rows - 1, mCropRect.y);
	int right = min(inputImage.cols, left + mCropRect.width);
	int bottom = min(inputImage.rows, top + mCropRect.height);
	cv::Mat croppedImage = inputImage(cv::Range(left, right), cv::Range(top, bottom));
	mCore->mDataStructureImageGray.mImage = croppedImage;

	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(croppedImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

