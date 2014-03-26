#include "ComponentAdaptiveThreshold.h"
#define THISCLASS ComponentAdaptiveThreshold

#include "DisplayEditor.h"
using namespace std;
#include <iostream>

THISCLASS::ComponentAdaptiveThreshold(SwisTrackCore *stc):
		Component(stc, wxT("AdaptiveThreshold")),
		mThisFrame(0), mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("After thresholding")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryThresholdingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentAdaptiveThreshold() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
  mThreshold = GetConfigurationInt(wxT("Threshold"), 255);
}

void THISCLASS::OnStep() {
	IplImage *inputImage = mCore->mDataStructureImageColor.mImage;
	if (! inputImage) {
		AddError(wxT("Cannot access Input image."));
		return;
	}
	if (mThisFrame == NULL) {
	  mThisFrame = cvCreateImage(cvSize(inputImage->width,
					    inputImage->height),
				     inputImage->depth, 1);
	}
	cvCvtColor(inputImage, mThisFrame, CV_BGR2GRAY);
	if (inputImage->nChannels != 3) {
		AddError(wxT("Input must be a color image (3 channels)."));
	}

	//Do the thresholding
	PrepareOutputImage(mThisFrame);
	cvAdaptiveThreshold(mThisFrame, mOutputImage, mThreshold);
	mCore->mDataStructureImageBinary.mImage = mOutputImage;


	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mOutputImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageBinary.mImage = 0;
}

void THISCLASS::OnStop()
{
	if (mOutputImage) {
		cvReleaseImage(&mOutputImage);
	}
}
