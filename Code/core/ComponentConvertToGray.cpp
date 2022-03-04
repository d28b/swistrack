#include "ComponentConvertToGray.h"
#define THISCLASS ComponentConvertToGray

#include "DisplayEditor.h"

THISCLASS::ComponentConvertToGray(SwisTrackCore * stc):
	Component(stc, wxT("ConvertToGray")),
	mDisplayOutput(wxT("Output"), wxT("After conversion to grayscale")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertToGray() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mChannel = GetConfigurationInt(wxT("Channel"), 0);
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureInput.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input Image"));
		return;
	}

	// Convert the input image to grayscale
	cv::Mat outputImage;
	if (inputImage.channels() == 1) {
		outputImage = inputImage;
	} else if (inputImage.channels() == 3) {
		outputImage = cv::Mat(inputImage.size(), CV_8UC1);
		if (mChannel >= 0 && mChannel <= 2) {
			cv::Mat out[] = {outputImage};
			int fromTo[] = {mChannel, 0};
			cv::mixChannels(&inputImage, 1, out, 1, fromTo, 1);
		} else {
			cv::cvtColor(inputImage, outputImage, cv::COLOR_BGR2GRAY);
		}
	} else {
		AddError(wxT("Invalid input image."));
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
