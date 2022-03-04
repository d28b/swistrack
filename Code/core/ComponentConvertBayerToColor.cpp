#include "ComponentConvertBayerToColor.h"
#define THISCLASS ComponentConvertBayerToColor

#include "DisplayEditor.h"

THISCLASS::ComponentConvertBayerToColor(SwisTrackCore * stc):
	Component(stc, wxT("ConvertBayerToColor")),
	mBayerType(0),
	mDisplayOutput(wxT("Output"), wxT("After conversion from Bayer to BGR")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertBayerToColor() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mBayerType = GetConfigurationInt(wxT("BayerType"), 0);
}

void THISCLASS::OnStep() {
	// Check input image
	cv::Mat inputImage = mCore->mDataStructureInput.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Convert
	cv::Mat outputImage(inputImage.size(), CV_8UC3);
	if (mBayerType == 0)
		cv::cvtColor(inputImage, outputImage, cv::COLOR_BayerBG2BGR);
	else if (mBayerType == 1)
		cv::cvtColor(inputImage, outputImage, cv::COLOR_BayerGB2BGR);
	else if (mBayerType == 2)
		cv::cvtColor(inputImage, outputImage, cv::COLOR_BayerRG2BGR);
	else if (mBayerType == 3)
		cv::cvtColor(inputImage, outputImage, cv::COLOR_BayerGR2BGR);
	else
		AddError(wxT("Invalid Bayer Pattern Type"));

	// Set the output image on the color data structure
	mCore->mDataStructureImageColor.mImage = outputImage;

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
