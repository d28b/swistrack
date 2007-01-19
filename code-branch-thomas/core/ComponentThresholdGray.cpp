#include "ComponentThresholdGray.h"
#define THISCLASS ComponentThresholdGray

THISCLASS::ComponentThresholdGray(SwisTrackCore *stc):
		Component(stc, "ThresholdGray"),
		mOutputImage(0), mThreshold(128) {

	// Data structure relations
	mDisplayName="Thresholding (grayscale)";
	mCategory=&(mCore->mCategoryThresholding);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
}

THISCLASS::~ComponentThresholdGray() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mThreshold=GetConfigurationInt("Threshold", 128);
}

void THISCLASS::OnStep() {
	IplImage *inputimage=mCore->mDataStructureImageGray.mImage;
	if (! inputimage) {return;}

	try {
		PrepareOutputImage(inputimage);
		cvThreshold(inputimage, mOutputImage, mThreshold, 255, CV_THRESH_BINARY);
		mCore->mDataStructureImageBinary.mImage=mOutputImage;
	} catch (...) {
		AddError("Thresholding failed.");
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageBinary.mImage=0;
}

void THISCLASS::OnStop() {
	if (mOutputImage) {cvReleaseImage(&mOutputImage);}
}
