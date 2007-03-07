#include "ComponentBackgroundSubtractionGray.h"
#define THISCLASS ComponentBackgroundSubtractionGray

#include <highgui.h>
#include <sstream>

THISCLASS::ComponentBackgroundSubtractionGray(SwisTrackCore *stc):
		Component(stc, "BackgroundSubtraction"),
		mBackgroundImageMean(0), mBackgroundImage(0), mCorrectMean(true),
		mDisplayImageOutput("Output", "After background subtraction") {

	// Data structure relations
	mDisplayName="Background Subtraction (grayscale)";
	mCategory=&(mCore->mCategoryPreprocessing);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentBackgroundSubtractionGray() {
}

void THISCLASS::OnStart() {
	std::string filename=GetConfigurationString("BackgroundImage", "");
	if (filename!="") {
		mBackgroundImage=cvLoadImage(filename.c_str(), -1);
	}
	if (! mBackgroundImage) {
		AddError("Cannot open background image.");
		return;
	}

	if (mBackgroundImage->nChannels==3) {
		// BGR case, we convert to gray
		IplImage *img=cvCreateImage(cvSize(mBackgroundImage->width, mBackgroundImage->height), mBackgroundImage->depth, 1);
		cvCvtColor(mBackgroundImage, img, CV_BGR2GRAY);
		cvReleaseImage(&mBackgroundImage);
		mBackgroundImage=img;
	} else if (mBackgroundImage->nChannels==1) {
		// Already in gray, do nothing
	} else {
		// Other cases, we take the first channel
		IplImage *img=cvCreateImage(cvSize(mBackgroundImage->width, mBackgroundImage->height), mBackgroundImage->depth, 1);
		cvCvtPixToPlane(mBackgroundImage, img, NULL, NULL, NULL);
		cvReleaseImage(&mBackgroundImage);
		mBackgroundImage=img;
	}

	// Whether to correct the mean or not
	mCorrectMean=GetConfigurationBool("CorrectMean", true);

	// We always calculate the background average, so we can select if we use the moving threshold during the segmentation
	if (mCorrectMean) {
		mBackgroundImageMean=cvMean(mBackgroundImage);
	} else {
		mBackgroundImageMean=0;
	}
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	IplImage *inputimage=mCore->mDataStructureImageGray.mImage;
	if (! inputimage) {return;}
	if (! mBackgroundImage) {return;}

	try {
		// Correct the tmpImage with the difference in image mean
		if (mCorrectMean) {
			cvAddS(inputimage, cvScalar(mBackgroundImageMean-cvMean(inputimage)), inputimage);
		}

		// Background Substraction
		cvAbsDiff(inputimage, mBackgroundImage, inputimage);
	} catch(...) {
		AddError("Background subtraction failed.");
	}

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=inputimage;
	std::ostringstream oss;
	oss << "After background subtraction, " << inputimage->width << "x" << inputimage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	if (mBackgroundImage) {cvReleaseImage(&mBackgroundImage);}
}
