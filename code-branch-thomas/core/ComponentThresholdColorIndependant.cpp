#include "ComponentThresholdColorIndependant.h"
#define THISCLASS ComponentThresholdColorIndependant

#include <sstream>

THISCLASS::ComponentThresholdColorIndependant(SwisTrackCore *stc):
		Component(stc, "ThresholdColorIndependant"),
		mOutputImage(0), mBlueThreshold(128), mGreenThreshold(128), mRedThreshold(128), mOrBool(true),
		mDisplayImageOutput("Output", "After thresholding") {

	// Data structure relations
	mDisplayName="Thresholding (Color independant)";
	mCategory=&(mCore->mCategoryThresholding);
	AddDataStructureRead(&(mCore->mDataStructureImageBGR));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentThresholdColorIndependant() {
}

void THISCLASS::OnStart() 
{
	tmpImage[0]=NULL;
	tmpImage[1]=NULL;
	tmpImage[2]=NULL;
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mBlueThreshold=GetConfigurationInt("BlueThreshold", 128);
	mGreenThreshold=GetConfigurationInt("GreenThreshold", 128);
	mRedThreshold=GetConfigurationInt("RedThreshold", 128);
	mOrBool=GetConfigurationBool("OrBool", true);
	mInvertThreshold=GetConfigurationBool("InvertThreshold", true);
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
	//We compute the average value on the three channels
	try 
	{
		PrepareOutputImage(inputImage);
		cvSplit(inputImage,tmpImage[0],tmpImage[1],tmpImage[2],NULL);
		for (int i=0;i<3;i++)
		{
			switch(inputImage->channelSeq[i])
			{
			case 'B':
				if (mInvertThreshold)
					cvThreshold(tmpImage[i], tmpImage[i],mBlueThreshold, 255, CV_THRESH_BINARY_INV);
				else
					cvThreshold(tmpImage[i], tmpImage[i],mBlueThreshold, 255, CV_THRESH_BINARY);

				break;
			case 'G':
				if (mInvertThreshold)
					cvThreshold(tmpImage[i], tmpImage[i],mGreenThreshold, 255, CV_THRESH_BINARY_INV);
				else
					cvThreshold(tmpImage[i], tmpImage[i],mGreenThreshold, 255, CV_THRESH_BINARY);
				break;
			case 'R':
				if (mInvertThreshold)
					cvThreshold(tmpImage[i], tmpImage[i],mRedThreshold, 255, CV_THRESH_BINARY_INV);
				else
					cvThreshold(tmpImage[i], tmpImage[i],mRedThreshold, 255, CV_THRESH_BINARY);
				break;
			default:
				AddError("Only Blue, Green and Red channels are accepted for this thresholding method.");
				return;
			}
		}
		if (mOrBool)
		{
			cvOr(tmpImage[0],tmpImage[1],tmpImage[0]);
			cvOr(tmpImage[0],tmpImage[2],mOutputImage);
		}
		else
		{
			cvAnd(tmpImage[0],tmpImage[1],tmpImage[0]);
			cvAnd(tmpImage[0],tmpImage[2],mOutputImage);
		}
		mCore->mDataStructureImageBinary.mImage=mOutputImage;
	} catch (...) 
	{
		AddError("Thresholding failed.");
		return;
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
