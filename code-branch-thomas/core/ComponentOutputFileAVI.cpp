#include "ComponentOutputFileAVI.h"
#define THISCLASS ComponentOutputFileAVI

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentOutputFileAVI(SwisTrackCore *stc):
		Component(stc, "OutputFileAVI"),
		mWriter(0), mOutputImage(0), mFrameRate(15),mInputSelection(0),
		mDisplayOutput("Output", "AVI File: Unprocessed Frame") {

	// Data structure relations
	mCategory=&(mCore->mCategoryOutput);
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputFileAVI() 
{
}

void THISCLASS::OnStart() {
	// Open file
	filename=GetConfigurationString("File", "");
	mFrameRate=GetConfigurationInt("FrameRate",15);
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() 
{
	mInputSelection=GetConfigurationInt("InputImage",0);
}

void THISCLASS::OnStep() 
{
	IplImage* inputImage;
	switch (mInputSelection)
	{
	case 0:
		//Gray image
		inputImage=mCore->mDataStructureImageGray.mImage;
		break;
	case 1:
		//Color image
		inputImage=mCore->mDataStructureImageColor.mImage;
		break;
	case 2:
		//Binary image
		inputImage=mCore->mDataStructureImageBinary.mImage;
		break;
	default:
		AddError("Invalid input image");
		return;
	}
	if (!inputImage)
	{
		AddError("Selected input is invalid");
		return;
	}
	if (!mOutputImage)
	{
		mOutputImage=cvCreateImage(cvGetSize(inputImage),8,inputImage->nChannels);
	}
	else if (inputImage->nChannels!=mOutputImage->nChannels)
	{
		cvReleaseImage(&mOutputImage);
		mOutputImage=cvCreateImage(cvGetSize(inputImage),8,inputImage->nChannels);
	}

	//Create the Writer
	if (!mWriter)
	{
		mWriter = cvCreateVideoWriter(filename.c_str(),-1,mFrameRate,cvGetSize(inputImage));
		if (!mWriter)
		{
			AddError("Error when creating the avi file");
			return;
		}
	}

	// AVI files are flipped
	cvFlip(inputImage,mOutputImage);

	//Write the frame to the avi
	cvWriteFrame(mWriter,mOutputImage);

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mOutputImage);
	}
}

void THISCLASS::OnStepCleanup() 
{
}

void THISCLASS::OnStop() 
{
	if (mOutputImage)
		cvReleaseImage(&mOutputImage);
	if (mWriter) {cvReleaseVideoWriter(&mWriter);}
}