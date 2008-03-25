#include "ComponentDoubleThresholdColorIndependent.h"
#define THISCLASS ComponentDoubleThresholdColorIndependent

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentDoubleThresholdColorIndependent(SwisTrackCore *stc):
		Component(stc, "DoubleThresholdColorIndependent"),
		mOutputImage(0), mLowThreshold(cvScalarAll(0)), mHighThreshold(cvScalarAll(0)),
		mDisplayOutput("Output", "After thresholding") {

	// Data structure relations
	mCategory=&(mCore->mCategoryThresholding);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentDoubleThresholdColorIndependent() {
}

void THISCLASS::OnStart() {	
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() 
{
	mLowThreshold.val[0]=GetConfigurationInt("BlueLowThreshold", 0);
	mLowThreshold.val[1]=GetConfigurationInt("GreenLowThreshold", 0);
	mLowThreshold.val[2]=GetConfigurationInt("RedLowThreshold", 0);
	//1 is added because of the way cvInRangeS makes the comparison
	mHighThreshold.val[0]=GetConfigurationInt("BlueHighThreshold", 255)+1;
	mHighThreshold.val[1]=GetConfigurationInt("GreenHighThreshold", 255)+1;
	mHighThreshold.val[2]=GetConfigurationInt("RedHighThreshold", 255)+1;
	strcpy(thresholdColorSeq,"BGR");
}

void THISCLASS::OnStep() {
	IplImage *inputimage=mCore->mDataStructureImageColor.mImage;	
	if (! inputimage) {
		AddError("Cannot access Input image");
		return;
	}
	if (inputimage->nChannels!=3) {
		AddError("Input must be a color image (3 channels)");
	}

	//Do the thresholding
	//We compute the average value on the three channels
	try {
		PrepareOutputImage(inputimage);
		// Correct the channel sequence
		if (strncmp(inputimage->channelSeq,thresholdColorSeq,3))		
		{
			CvScalar tmpLowThreshold;
			CvScalar tmpHighThreshold;

			for (int i=0;i<3;i++)				
					for (int j=0;j<3;j++)
						if (inputimage->channelSeq[i]==thresholdColorSeq[j])
						{	
							tmpLowThreshold.val[i]=mLowThreshold.val[j];							
							tmpHighThreshold.val[i]=mLowThreshold.val[j];
						}
			strcpy(thresholdColorSeq,inputimage->channelSeq);
			for (int i=0;i<3;i++)
			{
				mLowThreshold.val[i]=tmpLowThreshold.val[i];
				mHighThreshold.val[i]=tmpHighThreshold.val[i];				
			}
				
		}		
		cvInRangeS(inputimage,mLowThreshold,mHighThreshold,mOutputImage);				
		mCore->mDataStructureImageBinary.mImage=mOutputImage;
	} catch (...) 
	{
		AddError("Thresholding failed.");
		return;
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mOutputImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageBinary.mImage=0;
}

void THISCLASS::OnStop() 
{
	if (mOutputImage) {cvReleaseImage(&mOutputImage);}
}
