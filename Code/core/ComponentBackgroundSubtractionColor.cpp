#include "ComponentBackgroundSubtractionColor.h"
#define THISCLASS ComponentBackgroundSubtractionColor

#include <opencv2/core.hpp>
#include "DisplayEditor.h"
#include "ImageTools.h"

THISCLASS::ComponentBackgroundSubtractionColor(SwisTrackCore * stc):
	Component(stc, wxT("BackgroundSubtractionColor")),
	mBackgroundImage(), mBackgroundImageMean(cv::Scalar::all(0)),
	mCorrectMean(true),
	mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBackgroundSubtractionColor() {
}

void THISCLASS::OnStart() {
	mBackgroundImage = LoadConfigurationColorImage(wxT("BackgroundImage"), wxT("Background image"));
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	// Whether to correct the mean or not
	mCorrectMean = GetConfigurationBool(wxT("CorrectMean"), true);

	// We always calculate the background average, so we can select if we use the moving threshold during the segmentation
	if (mCorrectMean) {
		mBackgroundImageMean = cv::mean(mBackgroundImage);
	} else {
		mBackgroundImageMean = cv::Scalar::all(0);
	}
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	if (mBackgroundImage.empty()) {
		AddError(wxT("Background image not accessible"));
		return;
	}

	if (! ImageTools::EqualSize(inputImage, mBackgroundImage)) {
		AddError(wxT("Input and background images do not have the same size."));
		return;
	}

	// Correct the tmpImage with the difference in image mean
	cv::Mat intermediateImage;
	if (mCorrectMean) {
		cv::Scalar mean = cv::mean(inputImage);
		intermediateImage = inputImage + cv::Scalar(mBackgroundImageMean.val[0] - mean.val[0], mBackgroundImageMean.val[1] - mean.val[1], mBackgroundImageMean.val[2] - mean.val[2]);
	} else {
		intermediateImage = inputImage;
	}

	// Background Substraction
	cv::Mat outputImage;
	cv::absdiff(intermediateImage, mBackgroundImage, outputImage);
	mCore->mDataStructureImageColor.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	mBackgroundImage.release();
}
