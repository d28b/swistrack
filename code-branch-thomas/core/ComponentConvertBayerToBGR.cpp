#include "ComponentConvertBayerToBGR.h"
#define THISCLASS ComponentConvertBayerToBGR

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentConvertBayerToBGR(SwisTrackCore *stc):
		Component(stc, "ConvertBayerToBGR"),
		mOutputImage(0), mBayerType(0),
		mDisplayOutput("Output", "After conversion from Bayer to BGR") {

	// Data structure relations
	mCategory=&(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageBGR));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertBayerToBGR() {
}

void THISCLASS::OnStart() 
{
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() 
{
	mBayerType=GetConfigurationInt("BayerType",0);

}

void THISCLASS::OnStep() 
{
	IplImage *inputImage=mCore->mDataStructureInput.mImage;
	if (! inputImage) {return;}
	if (!mOutputImage)
		mOutputImage=cvCreateImage(cvSize(inputImage->width,inputImage->height),inputImage->depth,3);
	if (inputImage->nChannels!=1)
	{
		AddError("This function require a Gray input Image");
	}
	
	try {
		switch (mBayerType)
		{
		case 0 :
			cvCvtColor(inputImage,mOutputImage,CV_BayerBG2BGR);
			break;
		case 1 :
			cvCvtColor(inputImage,mOutputImage,CV_BayerGB2BGR);
			break;
		case 2 :
			cvCvtColor(inputImage,mOutputImage,CV_BayerRG2BGR);
			break;
		case 3 :
			cvCvtColor(inputImage,mOutputImage,CV_BayerGR2BGR);
			break;
		default :
			AddError("Invalid Bayer Pattern Type");
			return;
		}
	} catch(...) {
		AddError("Conversion from Bayer to BGR failed.");
	}
	mCore->mDataStructureImageBGR.mImage=mOutputImage;

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageBGR.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageGray.mImage=0;
}

void THISCLASS::OnStop() {
	if (mOutputImage) {cvReleaseImage(&mOutputImage);}
}
