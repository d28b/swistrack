#include "ComponentColorBlur.h"
#define THISCLASS ComponentColorBlur

#include <opencv2/highgui.hpp>
#include <cctype>
#include <algorithm>
#include "DisplayEditor.h"

THISCLASS::ComponentColorBlur(SwisTrackCore * stc):
	Component(stc, wxT("ColorBlur")),
	mBlurType(wxT("mean")), mSize(1),
	mDisplayOutput(wxT("Output"), wxT("After blurring the image")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentColorBlur() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	// Type
	wxString mBlurType = GetConfigurationString(wxT("Type"), wxT("mean"));
	mBlurType.MakeLower();

	// Size
	mSize = GetConfigurationInt(wxT("Size"), 1);
	if (mSize < 1) mSize = 1;
	if (mSize % 2 == 0) mSize += 1;
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage(inputImage.size(), CV_8UC3);
	if (mBlurType == wxT("median")) {
		cv::medianBlur(inputImage, outputImage, mSize);
	} else {
		cv::blur(inputImage, outputImage, cv::Size(mSize, mSize));
	}

	mCore->mDataStructureImageColor.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
