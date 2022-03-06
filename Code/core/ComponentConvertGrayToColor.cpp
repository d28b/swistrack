#include "ComponentConvertGrayToColor.h"
#define THISCLASS ComponentConvertGrayToColor

#include "DisplayEditor.h"

THISCLASS::ComponentConvertGrayToColor(SwisTrackCore * stc):
	Component(stc, wxT("ConvertGrayToColor")),
	mDisplayOutput(wxT("Output"), wxT("After conversion to grayscale")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertGrayToColor() {
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
