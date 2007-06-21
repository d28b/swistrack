#include "ComponentAdaptiveBackgroundSubtractionGray.h"
#define THISCLASS ComponentAdaptiveBackgroundSubtractionGray

#include <highgui.h>
#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentAdaptiveBackgroundSubtractionGray(SwisTrackCore *stc):
		Component(stc, "AdaptiveBackgroundSubtractionGray"),
		mBackgroundImageMean(cvScalarAll(0)), mUpdateProportion(0), mCorrectMean(true),
		mDisplayOutput("Output", "After background subtraction") {

	// Data structure relations
	mCategory=&(mCore->mCategoryPreprocessing);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentAdaptiveBackgroundSubtractionGray() {
}

void THISCLASS::OnStart() {
	// Whether to correct the mean or not
	mCorrectMean=GetConfigurationBool("CorrectMean", true);
	mUpdateProportion=GetConfigurationDouble("UpdateProportion", 0.1);
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
	IplImage *inputimage=mCore->mDataStructureImageGray.mImage;	
	//Check the images
	if (! inputimage) 
	{
		AddError("No input Image");
		return;
	}
	if (inputimage->nChannels !=1)
	{
		AddError("Input image has not 1 channel.");
		return;
	}
	if (! mBackgroundImage) 
	{
		//Create the background with the current image
		mBackgroundImage=cvCloneImage(inputimage);
	}
	else
	{
		//Update the background
		cvAddWeighted(inputimage,mUpdateProportion,mBackgroundImage,1.0-mUpdateProportion,0,mBackgroundImage);
	}
	if ((cvGetSize(inputimage).height!=cvGetSize(mBackgroundImage).height)||(cvGetSize(inputimage).width!=cvGetSize(mBackgroundImage).width))
	{
		AddError("Input and background images have not the same dimension");
		return;
	}	

	try {
		// Correct the tmpImage with the difference in image mean		
		if (mCorrectMean) 
		{					
			mBackgroundImageMean=cvAvg(mBackgroundImage);			
			CvScalar tmpScalar=cvAvg(inputimage);			
			cvAddS(inputimage, cvScalar(mBackgroundImageMean.val[0]-tmpScalar.val[0]),inputimage);
		}

		// Background Substraction
		cvAbsDiff(inputimage, mBackgroundImage, inputimage);
	} catch(...) {
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
