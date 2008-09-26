#include "ComponentColorMask.h"
#define THISCLASS ComponentColorMask

#include <highgui.h>
#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentColorMask(SwisTrackCore *stc):
		Component(stc, "ColorMask"),
		mMaskImage(0), mMode(cMode_BlackBlack),
		mDisplayOutput("Output", "After applying mask") {

	// Data structure relations
	mCategory=&(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentColorMask() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	// Load mask image
	std::string filename=GetConfigurationString("MaskImage", "");
	if (filename!="") {
		mMaskImage=cvLoadImage(filename.c_str(), -1);
	}
	if (! mMaskImage) {
		AddError("Cannot open mask file.");
		return;
	}

	// Convert mask image
	if (mMaskImage->nChannels==3) {
		// Already in BGR, do nothing
	} else if (mMaskImage->nChannels==1) {
		// Grayscale image, we convert to BGR
		IplImage *img=cvCreateImage(cvSize(mMaskImage->width, mMaskImage->height), mMaskImage->depth, 3);
		cvCvtColor(mMaskImage, img, CV_GRAY2BGR);
		cvReleaseImage(&mMaskImage);
		mMaskImage=img;
	} else {
		AddError("Invalid mask file. The mask file must be a grayscale or color image.");
		return;
	}
	
	// Mask mode
	std::string mode=GetConfigurationString("Mode", "black-black");
	if (mode=="white-white") {
		mMode=cMode_WhiteWhite;
	} else if (mode=="white-black") {
		mMode=cMode_WhiteBlack;
		cvNot(mMaskImage, mMaskImage);
	} else if (mode=="black-white") {
		mMode=cMode_BlackWhite;
		cvNot(mMaskImage, mMaskImage);
	} else {
		mMode=cMode_BlackBlack;
	}
}

void THISCLASS::OnStep() {
	if (! mCore->mDataStructureImageColor.mImage) {
		AddError("No input image.");
		return;
	}

	// Mask the image
	if (mMaskImage) {
		if ((mCore->mDataStructureImageColor.mImage->width!=mMaskImage->width) || (mCore->mDataStructureImageColor.mImage->height!=mMaskImage->height)) {
			AddError("Wrong mask size.");
			return;
		}

		if ((mMode==cMode_WhiteWhite) || (mMode==cMode_BlackWhite)) {
			cvOr(mCore->mDataStructureImageColor.mImage, mMaskImage, mCore->mDataStructureImageColor.mImage);
		} else {
			cvAnd(mCore->mDataStructureImageColor.mImage, mMaskImage, mCore->mDataStructureImageColor.mImage);
		}
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageColor.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	cvReleaseImage(&mMaskImage);
}
