#include "ComponentConvertBinaryToColor.h"
#define THISCLASS ComponentConvertBinaryToColor

#include "DisplayEditor.h"

THISCLASS::ComponentConvertBinaryToColor(SwisTrackCore * stc):
	Component(stc, wxT("ConvertBinaryToColor")),
	mDisplayOutput(wxT("Output"), wxT("After conversion to color")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertBinaryToColor() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageBinary.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage(inputImage.size(), CV_8UC3);
	cv::cvtColor(inputImage, outputImage, cv::COLOR_GRAY2BGR);

	mCore->mDataStructureImageColor.mImage = outputImage;

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
