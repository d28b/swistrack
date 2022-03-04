#include "ComponentColorSwapper.h"
#define THISCLASS ComponentColorSwapper

#include "DisplayEditor.h"
#include "Utility.h"
#include <limits>
#include <iostream>
#include <set>

using namespace std;

THISCLASS::ComponentColorSwapper(SwisTrackCore * stc):
	Component(stc, wxT("ColorSwapper")),
	mDisplayOutput(wxT("Output"), wxT("Color Swapper")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);
	Initialize();
}

THISCLASS::~ComponentColorSwapper() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mSourceColor = GetConfigurationColor(wxT("SourceColor"), cv::Scalar(255, 255, 255));
	mTargetColor = GetConfigurationColor(wxT("TargetColor"), cv::Scalar(0, 0, 0));
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage = inputImage.clone();
	for (int y = 0; y < outputImage.rows; y++) {
		unsigned char * line = outputImage.ptr(y);
		for (int x = 0; x < outputImage.cols; x++) {
			unsigned char r = line[3 * x + 0];
			unsigned char g = line[3 * x + 1];
			unsigned char b = line[3 * x + 2];
			if (r != mSourceColor.val[0]) continue;
			if (g != mSourceColor.val[1]) continue;
			if (b != mSourceColor.val[2]) continue;
			line[3 * x + 0] = mTargetColor.val[0];
			line[3 * x + 1] = mTargetColor.val[1];
			line[3 * x + 2] = mTargetColor.val[2];
		}
	}

	mCore->mDataStructureImageColor.mImage = outputImage;

	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}


