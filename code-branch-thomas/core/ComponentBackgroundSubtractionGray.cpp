#include "ComponentBackgroundSubtractionGray.h"
#define THISCLASS ComponentBackgroundSubtractionGray

#include <highgui.h>
#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentBackgroundSubtractionGray(SwisTrackCore *stc):
		Component(stc, "BackgroundSubtractionGray"),
		mBackgroundImageMean(cvScalarAll(0)), mBackgroundImage(0), mCorrectMean(true),
		mDisplayOutput("Output", "After background subtraction") {

	// Data structure relations
	mCategory=&(mCore->mCategoryPreprocessing);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBackgroundSubtractionGray() {
}

void THISCLASS::OnStart() {
	std::string filename=GetConfigurationString("BackgroundImage", "");
	if (filename!="") {
		mBackgroundImage=cvLoadImage(filename.c_str(),CV_LOAD_IMAGE_UNCHANGED);
	}
	if (! mBackgroundImage) {
		AddError("Cannot open background image.");
		return;
	}

	if (mBackgroundImage->nChannels!=1) {
		// Throw an error because background image is not gray
		AddError("Background image has more than 1 channel.");
		return;
	}

	// Read the reloadable parameters
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	// Whether to take the next image as background image
	mNextImageAsBackgroundImage=GetConfigurationBool("NextImageAsBackgroundImage", false);
	mConfiguration["NextImageAsBackgroundImage"]="false";

	// Whether to correct the mean or not
	mCorrectMean=GetConfigurationBool("CorrectMean", true);

	// We always calculate the background average, so we can select if we use the moving threshold during the segmentation
	if (mCorrectMean) {
		mBackgroundImageMean=cvAvg(mBackgroundImage);
	} else {
		mBackgroundImageMean=cvScalar(0);
	}
}

void THISCLASS::OnStep() {
	// Get input image
	IplImage *inputimage=mCore->mDataStructureImageGray.mImage;
	if (! inputimage) {
		AddError("No input Image");
		return;
	}
	if (inputimage->nChannels != 1) {
		AddError("Input image is not grayscale.");
		return;
	}

	// Create a new background image if necessary
	if (mNextImageAsBackgroundImage) {
		cvReleaseImage(&mBackgroundImage);
		mBackgroundImage=cvCloneImage(inputimage);
		mNextImageAsBackgroundImage=false;
	}

	// Check background image
	if (! mBackgroundImage) {
		AddError("Background image not accessible");
		return;
	}
	if ((cvGetSize(inputimage).height!=cvGetSize(mBackgroundImage).height) || (cvGetSize(inputimage).width!=cvGetSize(mBackgroundImage).width)) {
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
