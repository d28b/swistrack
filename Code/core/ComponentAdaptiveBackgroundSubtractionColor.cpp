#include "ComponentAdaptiveBackgroundSubtractionColor.h"
#define THISCLASS ComponentAdaptiveBackgroundSubtractionColor

#include <opencv2/core.hpp>
#include "DisplayEditor.h"
#include "ImageTools.h"

THISCLASS::ComponentAdaptiveBackgroundSubtractionColor(SwisTrackCore * stc):
	Component(stc, wxT("AdaptiveBackgroundSubtractionColor")),
	mBackgroundImageMean(0, 0, 0, 0), mCorrectMean(true), mUpdateProportion(0), mMode(AbsDiff),
	mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentAdaptiveBackgroundSubtractionColor() {
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
	wxString modeString = GetConfigurationString(wxT("Mode"), wxT("AbsDiff"));
	mMode =
	    modeString == wxT("ImageMinusBackground") ? ImageMinusBackground :
	    modeString == wxT("BackgroundMinusImage") ? BackgroundMinusImage :
	    AbsDiff;

	// Whether to take the next image as background image
	if (GetConfigurationBool(wxT("ResetBackgroundImage"), false)) {
		mBackgroundImage.release();
		mConfiguration[wxT("ResetBackgroundImage")] = wxT("false");
	}
}

void THISCLASS::OnStep() {
	// Get and check input image
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Check and update the background
	if (mBackgroundImage.empty()) {
		mBackgroundImage = inputImage.clone();
	} else if (mUpdateProportion > 0) {
		if (! ImageTools::EqualSize(inputImage, mBackgroundImage)) {
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
		intermediateImage = inputImage + cv::Scalar(mBackgroundImageMean.val[0] - tmpScalar.val[0], mBackgroundImageMean.val[1] - tmpScalar.val[1], mBackgroundImageMean.val[2] - tmpScalar.val[2]);
	} else {
		intermediateImage = inputImage;
	}

	// Background subtraction
	cv::Mat outputImage;
	if (mMode == ImageMinusBackground) {
		outputImage = intermediateImage - mBackgroundImage;
	} else if (mMode == BackgroundMinusImage) {
		outputImage = mBackgroundImage - intermediateImage;
	} else {
		cv::absdiff(intermediateImage, mBackgroundImage, outputImage);
	}

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
