#include "ComponentColorSwapper.h"
#define THISCLASS ComponentColorSwapper

#include "DisplayEditor.h"
#include "Utility.h"

THISCLASS::ComponentColorSwapper(SwisTrackCore * stc):
	Component(stc, wxT("ColorSwapper")),
	mSourceColor(*wxWHITE), mTargetColor(*wxBLACK),
	mDisplayOutput(wxT("Output"), wxT("After swapping colors")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
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
	mSourceColor = BGR(GetConfigurationColor(wxT("SourceColor"), *wxWHITE));
	mTargetColor = BGR(GetConfigurationColor(wxT("TargetColor"), *wxBLACK));
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	int count = 0;
	cv::Mat outputImage = inputImage.clone();
	for (int y = 0; y < outputImage.rows; y++) {
		unsigned char * line = outputImage.ptr(y);
		for (int x = 0; x < outputImage.cols; x++) {
			unsigned char b = line[3 * x + 0];
			unsigned char g = line[3 * x + 1];
			unsigned char r = line[3 * x + 2];
			if (b != mSourceColor.b) continue;
			if (g != mSourceColor.g) continue;
			if (r != mSourceColor.r) continue;
			line[3 * x + 0] = mTargetColor.b;
			line[3 * x + 1] = mTargetColor.g;
			line[3 * x + 2] = mTargetColor.r;
			count += 1;
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


