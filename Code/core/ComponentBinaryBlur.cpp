#include "ComponentBinaryBlur.h"
#define THISCLASS ComponentBinaryBlur

#include "DisplayEditor.h"
#include <algorithm>

THISCLASS::ComponentBinaryBlur(SwisTrackCore * stc):
	Component(stc, wxT("BinaryBlur")),
	mDisplayOutput(wxT("Output"), wxT("After blurring the image")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBinaryBlur() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mSize = GetConfigurationInt(wxT("size"), 3);
}

void THISCLASS::OnStep() {
	// Get and check input image
	cv::Mat inputImage = mCore->mDataStructureImageBinary.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage;
	cv::blur(inputImage, outputImage, cv::Size(mSize, mSize));
	mCore->mDataStructureImageGray.mImage = outputImage;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

