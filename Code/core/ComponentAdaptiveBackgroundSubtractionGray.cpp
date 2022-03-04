#include "ComponentAdaptiveBackgroundSubtractionGray.h"
#define THISCLASS ComponentAdaptiveBackgroundSubtractionGray

#include <opencv2/highgui.hpp>
#include "DisplayEditor.h"

THISCLASS::ComponentAdaptiveBackgroundSubtractionGray(SwisTrackCore * stc):
	Component(stc, wxT("AdaptiveBackgroundSubtractionGray")),
	mBackgroundImageMean(0, 0, 0, 0), mCorrectMean(true), mUpdateProportion(0), mMode(sMode_AbsDiff),
	mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentAdaptiveBackgroundSubtractionGray() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();

	// Force the copy of the input in the background in the first step
	mBackgroundImage.release();
}

void THISCLASS::OnReloadConfiguration() {
	// Whether to correct the mean or not
	mCorrectMean = GetConfigurationBool(wxT("CorrectMean"), true);
	mUpdateProportion = GetConfigurationDouble(wxT("UpdateProportion"), 0.1);

	// Mode
	wxString modestr = GetConfigurationString(wxT("Mode"), wxT("AbsDiff"));
	if (modestr == wxT("SubImageBackground")) {
		mMode = sMode_SubImageBackground;
	} else if (modestr == wxT("SubBackgroundImage")) {
		mMode = sMode_SubBackgroundImage;
	} else {
		mMode = sMode_AbsDiff;
	}

	// Whether to take the next image as background image
	if (GetConfigurationBool(wxT("ResetBackgroundImage"), false)) {
		mBackgroundImage.release();
		mConfiguration[wxT("ResetBackgroundImage")] = wxT("false");
	}
}

void THISCLASS::OnStep() {
	// Get and check input image
	cv::Mat inputImage = mCore->mDataStructureImageGray.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	if (inputImage.channels() != 1) {
		AddError(wxT("The input image is not a grayscale image."));
		return;
	}

	// Check and update the background
	if (mBackgroundImage.empty()) {
		mBackgroundImage = inputImage.clone();
	} else if (mUpdateProportion > 0) {
		if (inputImage.rows != mBackgroundImage.rows || inputImage.cols != mBackgroundImage.cols) {
			AddError(wxT("Input and background images do not have the same size."));
			return;
		}

		cv::addWeighted(inputImage, mUpdateProportion, mBackgroundImage, 1.0 - mUpdateProportion, 0, mBackgroundImage);
	}

	// Correct the tmpImage with the difference in image mean
	cv::Mat intermediateImage;
	if (mCorrectMean) {
		mBackgroundImageMean = cv::mean(mBackgroundImage);
		cv::Scalar tmpScalar = cv::mean(inputImage);
		intermediateImage = inputImage + cv::Scalar(mBackgroundImageMean.val[0] - tmpScalar.val[0]);
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
		cv::absdiff(intermediateImage, mBackgroundImage, outputImage);
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
