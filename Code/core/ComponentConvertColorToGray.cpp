#include "ComponentConvertColorToGray.h"
#define THISCLASS ComponentConvertColorToGray

#include "DisplayEditor.h"

THISCLASS::ComponentConvertColorToGray(SwisTrackCore * stc):
	Component(stc, wxT("ConvertColorToGray")),
	mChannel(0),
	mDisplayOutput(wxT("Output"), wxT("After conversion to grayscale")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertColorToGray() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mChannel = GetConfigurationInt(wxT("Channel"), 0);
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage(inputImage.size(), CV_8UC1);
	if (mChannel >= 0 && mChannel <= 2) {
		cv::Mat out[] = {outputImage};
		int fromTo[] = {mChannel, 0};
		cv::mixChannels(&inputImage, 1, out, 1, fromTo, 1);
	} else {
		cv::cvtColor(inputImage, outputImage, cv::COLOR_BGR2GRAY);
	}

	mCore->mDataStructureImageGray.mImage = outputImage;

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
