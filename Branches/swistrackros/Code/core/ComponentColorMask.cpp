#include "ComponentColorMask.h"
#define THISCLASS ComponentColorMask

#include <highgui.h>
#include "DisplayEditor.h"

THISCLASS::ComponentColorMask(SwisTrackCore *stc):
		Component(stc, wxT("ColorMask")),
		mMaskImage(0), mMode(cMode_BlackBlack),
		mDisplayOutput(wxT("Output"), wxT("After applying mask")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentColorMask() {
}

void THISCLASS::OnStart() {
	// Load mask image
	wxString filename_string = GetConfigurationString(wxT("MaskImage"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filename_string);
	if (filename.IsOk()) {
		mMaskImage = cvLoadImage(filename.GetFullPath().mb_str(wxConvFile), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	}
	if (! mMaskImage) {
		AddError(wxT("Cannot open mask file."));
		return;
	}

	// Convert mask image
	if (mMaskImage->nChannels == 3) {
		// Already in BGR, do nothing
	} else if (mMaskImage->nChannels == 1) {
		// Grayscale image, we convert to BGR
		IplImage *img = cvCreateImage(cvSize(mMaskImage->width, mMaskImage->height), mMaskImage->depth, 3);
		cvCvtColor(mMaskImage, img, CV_GRAY2BGR);
		cvReleaseImage(&mMaskImage);
		mMaskImage = img;
	} else {
		AddError(wxT("Invalid mask file. The mask file must be a grayscale or color image."));
		return;
	}

	// Mask mode
	wxString mode = GetConfigurationString(wxT("Mode"), wxT("black-black"));
	if (mode == wxT("white-white")) {
		mMode = cMode_WhiteWhite;
	} else if (mode == wxT("white-black")) {
		mMode = cMode_WhiteBlack;
		cvNot(mMaskImage, mMaskImage);
	} else if (mode == wxT("black-white")) {
		mMode = cMode_BlackWhite;
		cvNot(mMaskImage, mMaskImage);
	} else {
		mMode = cMode_BlackBlack;
	}

	// Load other parameters
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	if (! mCore->mDataStructureImageColor.mImage) {
		AddError(wxT("No input image."));
		return;
	}

	// Mask the image
	if (mMaskImage) {
		if ((mCore->mDataStructureImageColor.mImage->width != mMaskImage->width) || (mCore->mDataStructureImageColor.mImage->height != mMaskImage->height)) {
			AddError(wxT("Wrong mask size."));
			return;
		}

		if ((mMode == cMode_WhiteWhite) || (mMode == cMode_BlackWhite)) {
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
