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
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputFileAVI() {
}

void THISCLASS::OnStart() {
	mFilename=GetConfigurationString("File", "");
	mFrameRate=GetConfigurationInt("FrameRate", 15);
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mInputSelection=GetConfigurationInt("InputImage", 0);
}

void THISCLASS::OnStep() {
	// Get the input image
	IplImage* inputimage;
	switch (mInputSelection) {
	case 0:
		// Gray image
		inputimage=mCore->mDataStructureImageGray.mImage;
		break;
	case 1:
		// Color image
		inputimage=mCore->mDataStructureImageColor.mImage;
		break;
	case 2:
		// Binary image
		inputimage=mCore->mDataStructureImageBinary.mImage;
		break;
	default:
		AddError("Invalid input image");
		return;
	}

	if (! inputimage) {
		AddError("No image on selected input.");
		return;
	}

	// Prepare the output image
	if (! mOutputImage) {
		mOutputImage=cvCreateImage(cvGetSize(inputimage), 8, inputimage->nChannels);
	} else if (inputimage->nChannels != mOutputImage->nChannels) {
		cvReleaseImage(&mOutputImage);
		mOutputImage=cvCreateImage(cvGetSize(inputimage), 8, inputimage->nChannels);
	}

	// Create the Writer
	if (! mWriter) {
		mWriter = cvCreateVideoWriter(mFilename.c_str(), -1, mFrameRate, cvGetSize(inputimage));
		if (! mWriter) {
			AddError("Error while creating the AVI file.");
			return;
		}
	}

	// AVI files are flipped
	cvFlip(inputimage, mOutputImage);

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

void THISCLASS::OnStop() {
	if (mOutputImage) {cvReleaseImage(&mOutputImage);}
	if (mWriter) {cvReleaseVideoWriter(&mWriter);}
}