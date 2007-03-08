#include "ComponentBinaryMask.h"
#define THISCLASS ComponentBinaryMask

#include <sstream>
#include <highgui.h>

THISCLASS::ComponentBinaryMask(SwisTrackCore *stc):
		Component(stc, "BinaryMask"),
		mMaskImage(0),
		mDisplayImageOutput("Output", "After applying mask") {

	// Data structure relations
	mDisplayName="Binary mask";
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
		IplImage *img=cvCreateImage(cvSize(mMaskImage->width, mMaskImage->height), mMaskImage->depth, 1);
		cvCvtColor(mMaskImage, img, CV_BGR2GRAY);
		cvReleaseImage(&mMaskImage);
		mMaskImage=img;
	} else if (mMaskImage->nChannels==1) {
		// Already in gray, do nothing
	} else {
		// Other cases, we take the first channel
		IplImage *img=cvCreateImage(cvSize(mMaskImage->width, mMaskImage->height), mMaskImage->depth, 1);
		cvCvtPixToPlane(mMaskImage, img, NULL, NULL, NULL);
		cvReleaseImage(&mMaskImage);
		mMaskImage=img;
	}
}

void THISCLASS::OnStep() {
	if (! mCore->mDataStructureImageBinary.mImage) {
		AddError("No input image.");
		return;
	}

	if (mMaskImage) {
		if ((mCore->mDataStructureImageBinary.mImage->width!=mMaskImage->width) || (mCore->mDataStructureImageBinary.mImage->height!=mMaskImage->height)) {
			AddError("Wrong mask size.");
			return;
		}
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
