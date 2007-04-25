#include "ComponentBackgroundSubtractionGray.h"
#define THISCLASS ComponentBackgroundSubtractionGray

#include <highgui.h>
#include <sstream>

THISCLASS::ComponentBackgroundSubtractionGray(SwisTrackCore *stc):
		Component(stc, "BackgroundSubtractionGray"),
		mBackgroundImageMean(cvScalarAll(0)), mBackgroundImage(0), mCorrectMean(true),
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
		mBackgroundImage=cvLoadImage(filename.c_str(),CV_LOAD_IMAGE_UNCHANGED);
	}
	if (! mBackgroundImage) {
		AddError("Cannot open background image.");
		return;
	}

	if (mBackgroundImage->nChannels!=1) 	
	{
		//Throw an error because background image is not gray
		AddError("Background image has more than 1 channel.");
		return;
	} 
	// Whether to correct the mean or not
	mCorrectMean=GetConfigurationBool("CorrectMean", true);

	// We always calculate the background average, so we can select if we use the moving threshold during the segmentation
	if (mCorrectMean) {
		mBackgroundImageMean=cvAvg(mBackgroundImage);
	} else {
		mBackgroundImageMean=cvScalar(0);
	}
}

void THISCLASS::OnReloadConfiguration() 
{
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
	IplImage *inputImage=mCore->mDataStructureImageGray.mImage;
	if (! inputImage) 
	{
		AddError("No input Image");
		return;
	}
	if (inputImage->nChannels !=1)
	{
		AddError("Input image is not grayscale.");
		return;
	}

	if (! mBackgroundImage) 
	{
		AddError("Background image not accessible");
		return;
	}

	try {
		// Correct the tmpImage with the difference in image mean
		if (mCorrectMean) 
		{		
				cvAddS(inputImage, cvScalar(mBackgroundImageMean.val[0]-cvAvg(inputImage).val[0]), inputImage);
		}

		// Background Substraction
		cvAbsDiff(inputImage, mBackgroundImage, inputImage);
	} catch(...) {
		AddError("Background subtraction failed.");
	}

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=inputImage;
	std::ostringstream oss;
	oss << "After background subtraction, " << inputImage->width << "x" << inputImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	if (mBackgroundImage) {cvReleaseImage(&mBackgroundImage);}
}
