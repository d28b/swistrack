#include "ComponentBinaryMask.h"
#define THISCLASS ComponentBinaryMask

#include <sstream>

THISCLASS::ComponentBinaryMask(SwisTrackCore *stc):
		Component(stc, "BinaryDilation"),
		mNumber(10),
		mDisplayImageOutput("Output", "After dilation") {

	// Data structure relations
	mDisplayName="Binary image dilation";
	mCategory=&(mCore->mCategoryBinaryPreprocessing);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentBinaryMask() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	std::string filename=GetConfigurationString("MaskImage", "");
	if (filename!="") {
		mMaskImage=cvLoadImage(filename.c_str(), -1);
	}
	if (! mMaskImage) {
		AddError("Cannot open mask file.");
		return;
	}

	if (mMaskImage->nChannels==3) {
		// BGR case, we convert to gray
		IplImage *bg=cvCreateImage(cvSize(bg->width, bg->height), bg->depth, 1);
		cvCvtColor(mMaskImage, bg, CV_BGR2GRAY);
		cvReleaseImage(&mMaskImage);
		mMaskImage=bg;
	} else if (mMaskImage->nChannels==1) {
		// Already in gray, do nothing
	} else {
		// Other cases, we take the first channel
		IplImage *bg=cvCreateImage(cvSize(bg->width, bg->height), bg->depth, 1);
		cvCvtPixToPlane(mMaskImage, bg, NULL, NULL, NULL);
		cvReleaseImage(&mMaskImage);
		mMaskImage=bg;
	}
}

void THISCLASS::OnStep() {
	if (! mCore->mDataStructureImageBinary.mImage) {
		AddError("No input image.");
		return;
	}

	if (mNumber>0) {
		cvAnd(mCore->mDataStructureImageBinary.mImage, mMaskImage, mCore->mDataStructureImageBinary.mImage);
	}

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=mCore->mDataStructureImageBinary.mImage;
	std::ostringstream oss;
	oss << "Binary image, " << mCore->mDataStructureImageBinary.mImage->width << "x" << mCore->mDataStructureImageBinary.mImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	cvReleaseImage(&mMaskImage);
}
