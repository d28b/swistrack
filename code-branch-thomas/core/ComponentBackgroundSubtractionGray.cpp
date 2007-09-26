#include "ComponentBackgroundSubtractionGray.h"
#define THISCLASS ComponentBackgroundSubtractionGray

#include <highgui.h>
#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentBackgroundSubtractionGray(SwisTrackCore *stc):
		Component(stc, "BackgroundSubtractionGray"),
		mBackgroundImage(0), mBackgroundImageMean(cvScalarAll(0)), 
        mCorrectMean(true), mMode(sMode_AbsDiff),
		mDisplayOutput("Output", "After background subtraction") {

	// Data structure relations
	mCategory=&(mCore->mCategoryPreprocessingGray);
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
	std::string filename=GetConfigurationString("BackgroundImage", "");
	if (filename!="") {
		mBackgroundImage=cvLoadImage(filename.c_str(),CV_LOAD_IMAGE_UNCHANGED);
	}
	if (! mBackgroundImage) {
		AddError("Cannot open background image.");
		return;
	}

	// Check the background image
	// We explicitely do not convert image to grayscale automatically, as this is likely to be a configuration error of the user (e.g. wrong background selected). In addition, the user can easily convert a file to grayscale.
	if (mBackgroundImage->nChannels != 1) {
		AddError("Background image is not grayscale.");
		return;
	}

	// Read the reloadable parameters
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	// Whether to correct the mean or not
	mCorrectMean=GetConfigurationBool("CorrectMean", true);

	// Mode
	std::string modestr=GetConfigurationString("Mode", "AbsDiff");
	if (modestr=="SubImageBackground") {
		mMode=sMode_SubImageBackground;
	} else if (modestr=="SubBackgroundImage") {
		mMode=sMode_SubBackgroundImage;
	} else {
		mMode=sMode_AbsDiff;
	}

	// We always calculate the background average, so we can select if we use the moving threshold during the segmentation
	if (mCorrectMean) {
		mBackgroundImageMean=cvAvg(mBackgroundImage);
	} else {
		mBackgroundImageMean=cvScalar(0);
	}
}

void THISCLASS::OnStep() {
	// Get and check input image
	IplImage *inputimage=mCore->mDataStructureImageGray.mImage;
	if (! inputimage) {
		AddError("No input image.");
		return;
	}
	if (inputimage->nChannels != 1) {
		AddError("The input image is not a grayscale image.");
		return;
	}

	// Check the background image
	if (! mBackgroundImage) {
		AddError("No background image loaded.");
		return;
	}
	if ((cvGetSize(inputimage).height != cvGetSize(mBackgroundImage).height) || (cvGetSize(inputimage).width != cvGetSize(mBackgroundImage).width)) {
		AddError("Input and background images don't have the same size.");
		return;
	}

	try {
		// Correct the inputimage with the difference in image mean
		if (mCorrectMean) {
			cvAddS(inputimage, cvScalar(mBackgroundImageMean.val[0]-cvAvg(inputimage).val[0]), inputimage);
		}

		// Background subtraction
		if (mMode==sMode_SubImageBackground) {
			cvSub(inputimage, mBackgroundImage, inputimage);
		} else if (mMode==sMode_SubBackgroundImage) {
			cvSub(mBackgroundImage, inputimage, inputimage);
		} else {
			cvAbsDiff(inputimage, mBackgroundImage, inputimage);
		}
	} catch (...) {
		AddError("Background subtraction failed.");
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
	if (mBackgroundImage) {cvReleaseImage(&mBackgroundImage);}
}
