#include "ComponentMoveGrayToColor.h"
#define THISCLASS ComponentMoveGrayToColor

#include "DisplayEditor.h"

THISCLASS::ComponentMoveGrayToColor(SwisTrackCore * stc):
	Component(stc, wxT("MoveGrayToColor")),
	mDisplayOutput(wxT("Output"), wxT("Move the gray image to the color image.")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentMoveGrayToColor() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageGray.mImage;
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
