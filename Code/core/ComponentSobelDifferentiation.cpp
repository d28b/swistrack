#include "ComponentSobelDifferentiation.h"
#define THISCLASS ComponentSobelDifferentiation

#include "DisplayEditor.h"
#include <algorithm>

THISCLASS::ComponentSobelDifferentiation(SwisTrackCore * stc):
	Component(stc, wxT("SobelDifferentiation")),
	mDisplayOutput(wxT("Output"), wxT("Differentiation")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentSobelDifferentiation() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	// Differentiation Order for x
	xorder = GetConfigurationInt(wxT("xorder"), 1);
	// Differentiation Order for y
	yorder = GetConfigurationInt(wxT("yorder"), 1);
	// Size of the extended Sobel kernel
	apertureSize = GetConfigurationInt(wxT("apertureSize"), 3);

	// Check for errors in configurations
	if (xorder < 0 ) xorder = 0;
	if (xorder > 3 ) xorder = 3;
	if (yorder < 0 ) yorder = 0;
	if (yorder > 3 ) yorder = 3;

	// Ensuring the differentiation order is less than the kernel size
	if (apertureSize == 1 || apertureSize == 3) {
		if (xorder > 2)
			xorder = 2;
		if (yorder > 2)
			yorder = 2;
	}

	// Ensuring that at least 1 differentiation will be made
	if (xorder == 0 && yorder == 0) xorder = 1;
}

void THISCLASS::OnStep() {
	// Get and check input image
	cv::Mat inputImage = mCore->mDataStructureImageGray.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	cv::Mat outputImage(inputImage.rows, inputImage.cols, CV_8UC1);
	cv::Sobel(inputImage, outputImage, xorder, yorder, apertureSize);
	// I : Source image.
	// J : Destination image.
	// dx : Order of the derivative x .
	// dy : Order of the derivative y .
	// apertureSize : Size of the extended Sobel kernel, must be 1, 3, 5 or 7
	mCore->mDataStructureImageGray.mImage = outputImage;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

