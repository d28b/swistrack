#include "ComponentBinaryMask.h"
#define THISCLASS ComponentBinaryMask

#include <highgui.h>
#include "DisplayEditor.h"

THISCLASS::ComponentBinaryMask(SwisTrackCore *stc):
		Component(stc, wxT("BinaryMask")),
		mMaskImage(0),
		mDisplayOutput(wxT("Output"), wxT("After applying mask")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBinaryMask() {
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
		// BGR case, we convert to gray
		IplImage *img = cvCreateImage(cvSize(mMaskImage->width, mMaskImage->height), mMaskImage->depth, 1);
		cvCvtColor(mMaskImage, img, CV_BGR2GRAY);
		cvReleaseImage(&mMaskImage);
		mMaskImage = img;
	} else if (mMaskImage->nChannels == 1) {
		// Already in gray, do nothing
	} else {
		// Other cases, we take the first channel
		IplImage *img = cvCreateImage(cvSize(mMaskImage->width, mMaskImage->height), mMaskImage->depth, 1);
		cvCvtPixToPlane(mMaskImage, img, NULL, NULL, NULL);
		cvReleaseImage(&mMaskImage);
		mMaskImage = img;
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
	if (! mCore->mDataStructureImageBinary.mImage) {
		AddError(wxT("No input image."));
		return;
	}

	// Mask the image
	if (mMaskImage) {
		if ((mCore->mDataStructureImageBinary.mImage->width != mMaskImage->width) || (mCore->mDataStructureImageBinary.mImage->height != mMaskImage->height)) {
			AddError(wxT("Wrong mask size."));
			return;
		}

		if ((mMode == cMode_WhiteWhite) || (mMode == cMode_BlackWhite)) {
			cvOr(mCore->mDataStructureImageBinary.mImage, mMaskImage, mCore->mDataStructureImageBinary.mImage);
		} else {
			cvAnd(mCore->mDataStructureImageBinary.mImage, mMaskImage, mCore->mDataStructureImageBinary.mImage);
		}
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
