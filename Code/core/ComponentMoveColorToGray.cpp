#include "ComponentMoveColorToGray.h"
#define THISCLASS ComponentMoveColorToGray

#include "DisplayEditor.h"

THISCLASS::ComponentMoveColorToGray(SwisTrackCore * stc):
	Component(stc, wxT("MoveColorToGray")),
	mDisplayOutput(wxT("Output"), wxT("Move the color image to the gray.")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentMoveColorToGray() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage(inputImage.size(), CV_8UC1);
	cv::cvtColor(inputImage, outputImage, cv::COLOR_BGR2GRAY);

	mCore->mDataStructureImageGray.mImage = outputImage;

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
