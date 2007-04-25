#include "ComponentAdaptiveBackgroundSubtractionGray.h"
#define THISCLASS ComponentAdaptiveBackgroundSubtractionGray

#include <highgui.h>
#include <sstream>

THISCLASS::ComponentAdaptiveBackgroundSubtractionGray(SwisTrackCore *stc):
		Component(stc, "AdaptiveBackgroundSubtractionGray"),
		mBackgroundImageMean(cvScalarAll(0)), mUpdateProportion(0), mCorrectMean(true),
		mDisplayImageOutput("Output", "After background subtraction") {

	// Data structure relations
	mDisplayName="Adaptive Background Subtraction (Gray)";
	mCategory=&(mCore->mCategoryPreprocessing);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentAdaptiveBackgroundSubtractionGray() {
}

void THISCLASS::OnStart() {
	// Whether to correct the mean or not
	mCorrectMean=GetConfigurationBool("CorrectMean", true);
	mUpdateProportion=GetConfigurationDouble("UpdateProportion", true);
	//Force the copy of the input in the background in the first step
	mBackgroundImage=NULL;
}

void THISCLASS::OnReloadConfiguration() 
{
	// Whether to correct the mean or not
	mCorrectMean=GetConfigurationBool("CorrectMean", true);
	mUpdateProportion=GetConfigurationDouble("UpdateProportion", true);
	//Force reinitialization of the background image
	if (mBackgroundImage) 
	{
		cvReleaseImage(&mBackgroundImage);
		mBackgroundImage=NULL;
	}

	
}

void THISCLASS::OnStep() {
	IplImage *inputImage=mCore->mDataStructureImageGray.mImage;	
	//Check the images
	if (! inputImage) 
	{
		AddError("No input Image");
		return;
	}
	if (inputImage->nChannels !=1)
	{
		AddError("Input image has not 1 channel.");
		return;
	}
	if (! mBackgroundImage) 
	{
		//Create the background with the current image
		mBackgroundImage=cvCloneImage(inputImage);
	}
	else
	{
		//Update the background
		cvAddWeighted(inputImage,mUpdateProportion,mBackgroundImage,1.0-mUpdateProportion,0,mBackgroundImage);
	}
	if ((cvGetSize(inputImage).height!=cvGetSize(mBackgroundImage).height)||(cvGetSize(inputImage).width!=cvGetSize(mBackgroundImage).width))
	{
		AddError("Input and background images have not the same dimension");
		return;
	}	

	try {
		// Correct the tmpImage with the difference in image mean		
		if (mCorrectMean) 
		{					
			mBackgroundImageMean=cvAvg(mBackgroundImage);			
			CvScalar tmpScalar=cvAvg(inputImage);			
			cvAddS(inputImage, cvScalar(mBackgroundImageMean.val[0]-tmpScalar.val[0]),inputImage);
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
