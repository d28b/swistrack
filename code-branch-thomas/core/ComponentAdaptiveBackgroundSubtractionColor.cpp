#include "ComponentAdaptiveBackgroundSubtractionColor.h"
#define THISCLASS ComponentAdaptiveBackgroundSubtractionColor

#include <highgui.h>
#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentAdaptiveBackgroundSubtractionColor(SwisTrackCore *stc):
		Component(stc, "AdaptiveBackgroundSubtractionColor"),
		mBackgroundImageMean(cvScalarAll(0)), mUpdateProportion(0), mCorrectMean(true),
		mDisplayOutput("Output", "After background subtraction") {

	// Data structure relations
	mCategory=&(mCore->mCategoryPreprocessing);
	AddDataStructureRead(&(mCore->mDataStructureImageBGR));
	AddDataStructureWrite(&(mCore->mDataStructureImageBGR));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentAdaptiveBackgroundSubtractionColor() {
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
	mUpdateProportion=GetConfigurationDouble("UpdateProportion",0.1);
	//Force reinitialization of the background image
	if (mBackgroundImage) 
	{
		cvReleaseImage(&mBackgroundImage);
		mBackgroundImage=NULL;
	}

	
}

void THISCLASS::OnStep() {
	IplImage *inputImage=mCore->mDataStructureImageBGR.mImage;	
	//Check the images
	if (! inputImage) 
	{
		AddError("No input Image");
		return;
	}
	if (inputImage->nChannels !=3)
	{
		AddError("Input image has not 3 channels.");
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
			cvAddS(inputImage, cvScalar(mBackgroundImageMean.val[0]-tmpScalar.val[0],mBackgroundImageMean.val[1]-tmpScalar.val[1],mBackgroundImageMean.val[2]-tmpScalar.val[2]),inputImage);
		}

		// Background Substraction
		cvAbsDiff(inputImage, mBackgroundImage, inputImage);
	} catch(...) {
		AddError("Background subtraction failed.");
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(inputImage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	if (mBackgroundImage) {cvReleaseImage(&mBackgroundImage);}
}
