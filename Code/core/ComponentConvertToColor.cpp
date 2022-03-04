#include "ComponentConvertToColor.h"
#define THISCLASS ComponentConvertToColor

#include "DisplayEditor.h"

THISCLASS::ComponentConvertToColor(SwisTrackCore * stc):
	Component(stc, wxT("ConvertToColor")),
	mDisplayOutput(wxT("Output"), wxT("After conversion to color")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertToColor() {
}

void THISCLASS::OnStart() {
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureInput.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Convert the input image to a color image
	cv::Mat outputImage;
	if (inputImage.channels() == 3) {
		outputImage = inputImage;
	} else if (inputImage.channels() == 1) {
		outputImage = cv::Mat(inputImage.size(), CV_8UC3);
		cv::cvtColor(inputImage, outputImage, cv::COLOR_GRAY2BGR);
	} else {
		AddError(wxT("Invalid input image."));
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
