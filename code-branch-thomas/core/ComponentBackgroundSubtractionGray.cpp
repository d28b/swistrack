#include "ComponentBackgroundSubtractionGray.h"
#define THISCLASS ComponentBackgroundSubtractionGray

#include <highgui.h>

THISCLASS::ComponentBackgroundSubtractionGray(SwisTrackCore *stc):
		Component(stc, "BackgroundSubtraction"),
		mBackgroundImageMean(0), mBackgroundImage(0), mCorrectMean(true) {

	// Data structure relations
	mDisplayName="Background Subtraction (grayscale)";
	mCategory=mCore->mCategoryPreprocessing;
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
}

THISCLASS::~ComponentBackgroundSubtractionGray() {
}

void THISCLASS::OnStart() {
	IplImage* bg = cvLoadImage(GetConfigurationString("BackgroundImage", "").c_str(), -1);
	if (! bg) {
		AddError("Cannot open background file.");
		return;
	}

	mBackgroundImage=cvCreateImage(cvSize(bg->width, bg->height), bg->depth, 1);
	if (! mBackgroundImage) {
		AddError("Cannot create background image.");
		return;
	}

	switch (bg->nChannels) {
	case 3:	// BGR case, we convert in gray
		cvCvtColor(bg, mBackgroundImage, CV_BGR2GRAY);
		break;
	case 1:	// Already in gray
		cvCopy(bg, mBackgroundImage);
		break;
	default:	// other cases, we take the first channel
		cvCvtPixToPlane(bg, mBackgroundImage, NULL, NULL, NULL);
		break;
	}

	// Whether to correct the mean or not
	mCorrectMean=GetConfigurationBool("CorrectMean", true);

	// We always calculate the background average, so we can select if we use the moving threshold during the segmentation
	if (mCorrectMean) {
		mBackgroundImageMean=cvMean(mBackgroundImage);
	} else {
		mBackgroundImageMean=0;
	}
	
	// Release the temporary image
	cvReleaseImage(&bg);
}

void THISCLASS::OnStep() {
	IplImage *inputimage=mCore->mDataStructureImageGray.mImage;
	if (! inputimage) {return;}
	if (! mBackgroundImage) {return;}

	try {
		// Correct the tmpImage with the difference in image mean
		if (mCorrectMean)
			cvAddS(inputimage, cvScalar(mBackgroundImageMean-cvMean(inputimage)), inputimage);

		// Background Substraction
		cvAbsDiff(inputimage, mBackgroundImage, inputimage);
	} catch(...) {
		AddError("Background subtraction failed.");
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	if (mBackgroundImage) {cvReleaseImage(&mBackgroundImage);}
}
