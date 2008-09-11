#include "ComponentAdaptiveBackgroundSubtractionColor.h"
#define THISCLASS ComponentAdaptiveBackgroundSubtractionColor

#include <highgui.h>
#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentAdaptiveBackgroundSubtractionColor(SwisTrackCore *stc):
		Component(stc, "AdaptiveBackgroundSubtractionColor"),
		mBackgroundImageMean(cvScalarAll(0)), mCorrectMean(true), mUpdateProportion(0), mMode(sMode_AbsDiff),
		mDisplayOutput("Output", "After background subtraction") {

	// Data structure relations
	mCategory=&(mCore->mCategoryPreprocessingColor);
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

	//Force the copy of the input in the background in the first step
	mBackgroundImage=NULL;
}

void THISCLASS::OnReloadConfiguration() {
	// Whether to correct the mean or not
	mCorrectMean=GetConfigurationBool("CorrectMean", true);
	mUpdateProportion=GetConfigurationDouble("UpdateProportion", 0.1);

	// Mode
	std::string modestr=GetConfigurationString("Mode", "AbsDiff");
	if (modestr=="SubImageBackground") {
		mMode=sMode_SubImageBackground;
	} else if (modestr=="SubBackgroundImage") {
		mMode=sMode_SubBackgroundImage;
	} else {
		mMode=sMode_AbsDiff;
	}

	// Whether to take the next image as background image
	if (GetConfigurationBool("ResetBackgroundImage", false)) {
		cvReleaseImage(&mBackgroundImage);
		mBackgroundImage=NULL;
		mConfiguration["ResetBackgroundImage"]="false";
	}
}

void THISCLASS::OnStep() {
	// Get and check input image
	IplImage *inputimage=mCore->mDataStructureImageColor.mImage;
	if (! inputimage) {
		AddError("No input image.");
		return;
	}
	if (inputimage->nChannels != 3) {
		AddError("The input image is not a color image.");
		return;
	}

	// Check and update the background
	if (! mBackgroundImage) {
		mBackgroundImage=cvCloneImage(inputimage);
	} else if (mUpdateProportion>0) {
		if ((cvGetSize(inputimage).height != cvGetSize(mBackgroundImage).height) || (cvGetSize(inputimage).width != cvGetSize(mBackgroundImage).width)) {
			AddError("Input and background images do not have the same size.");
			return;
		}

		cvAddWeighted(inputimage, mUpdateProportion, mBackgroundImage, 1.0-mUpdateProportion, 0, mBackgroundImage);
	}

	try {
		// Correct the tmpImage with the difference in image mean		
		if (mCorrectMean) {
			mBackgroundImageMean=cvAvg(mBackgroundImage);			
			CvScalar tmpScalar=cvAvg(inputimage);			
			cvAddS(inputimage, cvScalar(mBackgroundImageMean.val[0]-tmpScalar.val[0], mBackgroundImageMean.val[1]-tmpScalar.val[1], mBackgroundImageMean.val[2]-tmpScalar.val[2]),inputimage);
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
