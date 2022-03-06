#include "ComponentBackgroundSubtractionGray.h"
#define THISCLASS ComponentBackgroundSubtractionGray

#include <opencv2/core.hpp>
#include "DisplayEditor.h"
#include "ImageTools.h"

THISCLASS::ComponentBackgroundSubtractionGray(SwisTrackCore * stc):
	Component(stc, wxT("BackgroundSubtractionGray")),
	mBackgroundImage(), mBackgroundImageMean(cv::Scalar::all(0)),
	mCorrectMean(true), mMode(sMode_AbsDiff),
	mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBackgroundSubtractionGray() {
}

void THISCLASS::OnStart() {
	mBackgroundImage = LoadConfigurationGrayscaleImage(wxT("BackgroundImage"), wxT("Background image"));
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	// Whether to correct the mean or not
	mCorrectMean = GetConfigurationBool(wxT("CorrectMean"), true);

	// Mode
	wxString modestr = GetConfigurationString(wxT("Mode"), wxT("AbsDiff"));
	if (modestr == wxT("SubImageBackground")) {
		mMode = sMode_SubImageBackground;
	} else if (modestr == wxT("SubBackgroundImage")) {
		mMode = sMode_SubBackgroundImage;
	} else {
		mMode = sMode_AbsDiff;
	}

	// We always calculate the background average, so we can select if we use the moving threshold during the segmentation
	if (mCorrectMean) {
		mBackgroundImageMean = cv::mean(mBackgroundImage);
	} else {
		mBackgroundImageMean = cv::Scalar::all(0);
	}
}

void THISCLASS::OnStep() {
	// Get and check input image
	cv::Mat inputImage = mCore->mDataStructureImageGray.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Check the background image
	if (mBackgroundImage.empty()) {
		AddError(wxT("No background image loaded."));
		return;
	}

	if (! ImageTools::EqualSize(inputImage, mBackgroundImage)) {
		AddError(wxT("Input and background images don't have the same size."));
		return;
	}

	// Correct the inputImage with the difference in image mean
	cv::Mat intermediateImage;
	if (mCorrectMean) {
		cv::Scalar mean = cv::mean(inputImage);
		intermediateImage = inputImage + (mBackgroundImageMean.val[0] - mean.val[0]);
	} else {
		intermediateImage = inputImage;
	}

	// Background subtraction
	cv::Mat outputImage;
	if (mMode == sMode_SubImageBackground) {
		outputImage = inputImage - mBackgroundImage;
	} else if (mMode == sMode_SubBackgroundImage) {
		outputImage = mBackgroundImage - inputImage;
	} else {
		cv::absdiff(inputImage, mBackgroundImage, outputImage);
	}

	mCore->mDataStructureImageGray.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	mBackgroundImage.release();
}
