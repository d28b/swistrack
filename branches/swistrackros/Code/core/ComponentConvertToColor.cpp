#include "ComponentConvertToColor.h"
#define THISCLASS ComponentConvertToColor

#include "DisplayEditor.h"

THISCLASS::ComponentConvertToColor(SwisTrackCore *stc):
		Component(stc, wxT("ConvertToColor")),
		mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("After conversion to color")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertToColor() {
}

void THISCLASS::OnStart() {
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	IplImage *inputimage = mCore->mDataStructureInput.mImage;
	if (! inputimage) {
		return;
	}

	try {
		// We convert the input image to a color image
		if (inputimage->nChannels == 3) {
			// We already have a color image
			mCore->mDataStructureImageColor.mImage = inputimage;
		} else if (inputimage->nChannels == 1) {
			// Gray, convert to BGR
			PrepareOutputImage(inputimage);
			cvCvtColor(inputimage, mOutputImage, CV_GRAY2BGR);
			mCore->mDataStructureImageColor.mImage = mOutputImage;
		} else {
			// Other cases, we take the first channel and transform it in BGR
			PrepareOutputImage(inputimage);
			cvCvtPixToPlane(inputimage, mOutputImage, NULL, NULL, NULL);
			cvCvtColor(mOutputImage, mOutputImage, CV_GRAY2BGR);
			mCore->mDataStructureImageColor.mImage = mOutputImage;
		}
	} catch (...) {
		AddError(wxT("Conversion to gray failed."));
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
	if (mOutputImage) {
		cvReleaseImage(&mOutputImage);
	}
}
