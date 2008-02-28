#include "ComponentBinaryMask.h"
#define THISCLASS ComponentBinaryMask

#include <highgui.h>
#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentBinaryMask(SwisTrackCore *stc):
		Component(stc, "BinaryMask"),
		mMaskImage(0),
		mDisplayOutput("Output", "After applying mask") {

	// Data structure relations
	mCategory=&(mCore->mCategoryPreprocessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
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

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageBinary.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	cvReleaseImage(&mMaskImage);
}
