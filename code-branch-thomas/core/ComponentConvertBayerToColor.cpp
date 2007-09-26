#include "ComponentConvertBayerToColor.h"
#define THISCLASS ComponentConvertBayerToColor

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentConvertBayerToColor(SwisTrackCore *stc):
		Component(stc, "ConvertBayerToColor"),
		mBayerType(0), mOutputImage(0),
		mDisplayOutput("Output", "After conversion from Bayer to BGR") {

	// Data structure relations
	mCategory=&(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertBayerToColor() {
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
	IplImage *inputimage=mCore->mDataStructureInput.mImage;
	if (! inputimage) {return;}
	if (!mOutputImage)
		mOutputImage=cvCreateImage(cvSize(inputimage->width,inputimage->height),inputimage->depth,3);
	if (inputimage->nChannels!=1)
	{
		AddError("This function require a Gray input Image");
	}
	
	try {
		switch (mBayerType)
		{
		case 0 :
			cvCvtColor(inputimage,mOutputImage,CV_BayerBG2BGR);
			break;
		case 1 :
			cvCvtColor(inputimage,mOutputImage,CV_BayerGB2BGR);
			break;
		case 2 :
			cvCvtColor(inputimage,mOutputImage,CV_BayerRG2BGR);
			break;
		case 3 :
			cvCvtColor(inputimage,mOutputImage,CV_BayerGR2BGR);
			break;
		default :
			AddError("Invalid Bayer Pattern Type");
			return;
		}
	} catch(...) {
		AddError("Conversion from Bayer to BGR failed.");
	}
	mCore->mDataStructureImageColor.mImage=mOutputImage;

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageColor.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageGray.mImage=0;
}

void THISCLASS::OnStop() {
	if (mOutputImage) {cvReleaseImage(&mOutputImage);}
}
