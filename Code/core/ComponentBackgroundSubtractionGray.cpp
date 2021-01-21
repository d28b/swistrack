#include "ComponentBackgroundSubtractionGray.h"
#define THISCLASS ComponentBackgroundSubtractionGray

#include <highgui.h>
#include "DisplayEditor.h"

THISCLASS::ComponentBackgroundSubtractionGray(SwisTrackCore *stc):
		Component(stc, wxT("BackgroundSubtractionGray")),
		mBackgroundImage(0), mBackgroundImageMean(cvScalarAll(0)),
		mCorrectMean(true), mMode(sMode_AbsDiff),
		mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBackgroundSubtractionGray() {
}

void THISCLASS::OnStart() {
	// Load the background image
	wxString filename_string = GetConfigurationString(wxT("BackgroundImage"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filename_string);
	if (filename.IsOk()) {
		mBackgroundImage = cvLoadImage(filename.GetFullPath().mb_str(wxConvFile), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	}
	if (! mBackgroundImage) {
		AddError(wxT("Cannot open background image."));
		return;
	}

	// Check the background image
	// We explicitely do not convert image to grayscale automatically, as this is likely to be a configuration error of the user (e.g. wrong background selected). In addition, the user can easily convert a file to grayscale.
	if (mBackgroundImage->nChannels != 1) {
		AddError(wxT("Background image is not grayscale."));
		return;
	}

	// Read the reloadable parameters
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
		mBackgroundImageMean = cvAvg(mBackgroundImage);
	} else {
		mBackgroundImageMean = cvScalar(0);
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

	// Check the background image
	if (! mBackgroundImage) {
		AddError(wxT("No background image loaded."));
		return;
	}
	if ((cvGetSize(inputimage).height != cvGetSize(mBackgroundImage).height) || (cvGetSize(inputimage).width != cvGetSize(mBackgroundImage).width)) {
		AddError(wxT("Input and background images don't have the same size."));
		return;
	}

	try {
		// Correct the inputimage with the difference in image mean
		if (mCorrectMean) {
			cvAddS(inputimage, cvScalar(mBackgroundImageMean.val[0] - cvAvg(inputimage).val[0]), inputimage);
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
