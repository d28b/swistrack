#include "ComponentAdaptiveBackgroundSubtractionGray.h"
#define THISCLASS ComponentAdaptiveBackgroundSubtractionGray

#include <highgui.h>
#include "DisplayEditor.h"

THISCLASS::ComponentAdaptiveBackgroundSubtractionGray(SwisTrackCore *stc):
		Component(stc, wxT("AdaptiveBackgroundSubtractionGray")),
		mBackgroundImageMean(cvScalarAll(0)), mCorrectMean(true), mUpdateProportion(0), mMode(sMode_AbsDiff),
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
	mBackgroundImage = NULL;
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
		cvReleaseImage(&mBackgroundImage);
		mBackgroundImage = NULL;
		mConfiguration[wxT("ResetBackgroundImage")] = wxT("false");
	}
}

void THISCLASS::OnStep() {
	// Get and check input image
	IplImage *inputimage = mCore->mDataStructureImageGray.mImage;
	if (! inputimage) {
		AddError(wxT("No input image."));
		return;
	}
	if (inputimage->nChannels != 1) {
		AddError(wxT("The input image is not a grayscale image."));
		return;
	}

	// Check and update the background
	if (! mBackgroundImage) {
		mBackgroundImage = cvCloneImage(inputimage);
	} else if (mUpdateProportion > 0) {
		if ((cvGetSize(inputimage).height != cvGetSize(mBackgroundImage).height) || (cvGetSize(inputimage).width != cvGetSize(mBackgroundImage).width)) {
			AddError(wxT("Input and background images do not have the same size."));
			return;
		}

		cvAddWeighted(inputimage, mUpdateProportion, mBackgroundImage, 1.0 - mUpdateProportion, 0, mBackgroundImage);
	}

	try {
		// Correct the tmpImage with the difference in image mean
		if (mCorrectMean) {
			mBackgroundImageMean = cvAvg(mBackgroundImage);
			CvScalar tmpScalar = cvAvg(inputimage);
			cvAddS(inputimage, cvScalar(mBackgroundImageMean.val[0] - tmpScalar.val[0]), inputimage);
		}

		// Background subtraction
		if (mMode == sMode_SubImageBackground) {
			cvSub(inputimage, mBackgroundImage, inputimage);
		} else if (mMode == sMode_SubBackgroundImage) {
			cvSub(mBackgroundImage, inputimage, inputimage);
		} else {
			cvAbsDiff(inputimage, mBackgroundImage, inputimage);
		}
	} catch (...) {
		AddError(wxT("Background subtraction failed."));
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(inputimage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	if (mBackgroundImage) {
		cvReleaseImage(&mBackgroundImage);
	}
}
