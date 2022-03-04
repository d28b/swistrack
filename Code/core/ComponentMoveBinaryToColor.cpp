#include "ComponentMoveBinaryToColor.h"
#define THISCLASS ComponentMoveBinaryToColor

#include "DisplayEditor.h"

THISCLASS::ComponentMoveBinaryToColor(SwisTrackCore * stc):
	Component(stc, wxT("MoveBinaryToColor")),
	mDisplayOutput(wxT("Output"), wxT("Move the binary image to the color image.")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentMoveBinaryToColor() {
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
