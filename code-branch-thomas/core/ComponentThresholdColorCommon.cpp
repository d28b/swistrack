#include "ComponentThresholdColorCommon.h"
#define THISCLASS ComponentThresholdColorCommon

#include <sstream>

THISCLASS::ComponentThresholdColorCommon(SwisTrackCore *stc):
		Component(stc, "ThresholdColorCommon"),
		mOutputImage(0), mThreshold(128),
		mDisplayImageOutput("Output", "After thresholding") {

	// Data structure relations
	mDisplayName="Thresholding (Color common)";
	mCategory=&(mCore->mCategoryThresholding);
	AddDataStructureRead(&(mCore->mDataStructureImageBGR));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentThresholdColorCommon() {
}

void THISCLASS::OnStart() 
{
	tmpImage[0]=NULL;
	tmpImage[1]=NULL;
	tmpImage[2]=NULL;
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mThreshold=GetConfigurationInt("Threshold", 128);
	mAverageBool=GetConfigurationBool("AverageBool", true);
}

void THISCLASS::OnStep() {
	IplImage *inputImage=mCore->mDataStructureImageBGR.mImage;	
	if (! inputImage) 
	{
		AddError("Cannot access Input image");
		return;
	}
	if (inputImage->nChannels!=3)
	{
		AddError("Input must be a color image (3 channels)");
	}
	//Create the images needed for the work if necessary
	for (int i=0;i<3;i++)
	{
		if(!tmpImage[i])
			tmpImage[i]=cvCreateImage(cvGetSize(inputImage),8,1);
	}
	
	//Do the thresholding
	if (mAverageBool)
	{
		//We compute the average value on the three channels
		try 
		{
			PrepareOutputImage(inputImage);
			cvSplit(inputImage,tmpImage[0],tmpImage[1],tmpImage[2],NULL);						
			cvAdd(tmpImage[0],tmpImage[1],tmpImage[0]);
			cvAdd(tmpImage[0],tmpImage[2],tmpImage[0]);			
			cvThreshold(tmpImage[0], mOutputImage, 3*mThreshold, 255, CV_THRESH_BINARY);			
			mCore->mDataStructureImageBinary.mImage=mOutputImage;
		} catch (...) 
		{
			AddError("Thresholding failed.");
		}
	}
	else
	{
		try 
		{
			PrepareOutputImage(inputImage);
			cvSplit(inputImage,tmpImage[0],tmpImage[1],tmpImage[2],NULL);			
			for (int i=0;i<3;i++)
				cvThreshold(tmpImage[i], tmpImage[i], mThreshold, 255, CV_THRESH_BINARY);
			cvOr(tmpImage[0],tmpImage[1],tmpImage[0]);
			cvOr(tmpImage[0],tmpImage[2],mOutputImage);			
			mCore->mDataStructureImageBinary.mImage=mOutputImage;
		} catch (...) 
		{
			AddError("Thresholding failed.");
		}

	}

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=mOutputImage;
	std::ostringstream oss;
	oss << "Binary image, " << mOutputImage->width << "x" << mOutputImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageBinary.mImage=0;
}

void THISCLASS::OnStop() {
	if (mOutputImage) {cvReleaseImage(&mOutputImage);}
	for (int i=0; i<3;i++)
		if (tmpImage[i])
			cvReleaseImage(&(tmpImage[i]));
}