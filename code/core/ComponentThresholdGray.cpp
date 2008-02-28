#include "ComponentThresholdGray.h"
#define THISCLASS ComponentThresholdGray

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentThresholdGray(SwisTrackCore *stc):
		Component(stc, "ThresholdGray"),
		mOutputImage(0), mThreshold(128), mInvertThreshold(false),
		mDisplayOutput("Output", "After thresholding") {

	// Data structure relations
	mCategory=&(mCore->mCategoryThresholding);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentThresholdGray() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mThreshold=GetConfigurationInt("Threshold", 128);
	mInvertThreshold=GetConfigurationBool("InvertThreshold", false);
}

void THISCLASS::OnStep() {
	IplImage *inputimage=mCore->mDataStructureImageGray.mImage;
	if (! inputimage) {return;}

	try {
		PrepareOutputImage(inputimage);
		if (mInvertThreshold)
			cvThreshold(inputimage, mOutputImage, mThreshold, 255, CV_THRESH_BINARY_INV);
		else
			cvThreshold(inputimage, mOutputImage, mThreshold, 255, CV_THRESH_BINARY);
		mCore->mDataStructureImageBinary.mImage=mOutputImage;
	} catch (...) {
		AddError("Thresholding failed.");
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

void THISCLASS::OnStop() {
	if (mOutputImage) {cvReleaseImage(&mOutputImage);}
}
