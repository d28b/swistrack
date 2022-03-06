#include "ComponentWeightedSum.h"
#define THISCLASS ComponentWeightedSum

#include <opencv2/core.hpp>
#include "DisplayEditor.h"

THISCLASS::ComponentWeightedSum(SwisTrackCore * stc):
	Component(stc, wxT("WeightedSum")),
	base(0), R(0.5), G(0.5), B(0.5),
	mDisplayOutput(wxT("Output"), wxT("After weighted sum")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentWeightedSum() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	base = GetConfigurationDouble(wxT("Base"), 0.0);
	R = GetConfigurationDouble(wxT("R"), 0.5);
	G = GetConfigurationDouble(wxT("G"), 0.5);
	B = GetConfigurationDouble(wxT("B"), 0.5);
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Calculate weighted sum
	cv::Mat outputImage(inputImage.rows, inputImage.cols, CV_8UC1);
	for (int y = 0; y < inputImage.rows; y++) {
		unsigned char * inputLine = inputImage.ptr(y);
		unsigned char * outputLine = outputImage.ptr(y);
		for (int x = 0; x < inputImage.cols; x++) {
			int b = inputLine[x * 3 + 0];
			int g = inputLine[x * 3 + 1];
			int r = inputLine[x * 3 + 2];

			int output = (int) (base + R * r + G * g + B * b + 0.5);
			if (output < 0) output = 0;
			if (output > 255) output = 255;
			outputLine[x] = output;
		}
	}

	mCore->mDataStructureImageGray.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

