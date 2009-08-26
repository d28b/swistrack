#include "ComponentConvertBayerToColor.h"
#define THISCLASS ComponentConvertBayerToColor

#include "DisplayEditor.h"

THISCLASS::ComponentConvertBayerToColor(SwisTrackCore *stc):
		Component(stc, wxT("ConvertBayerToColor")),
		mBayerType(0), mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("After conversion from Bayer to BGR")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertBayerToColor() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mBayerType = GetConfigurationInt(wxT("BayerType"), 0);
}

void THISCLASS::OnStep() {
	// Check input image
	IplImage *inputimage = mCore->mDataStructureInput.mImage;
	if (! inputimage) {
		return;
	}
	if (inputimage->nChannels != 1) {
		AddError(wxT("This component requires a grayscale input image."));
	}

	// Prepare the output image
	PrepareOutputImage(inputimage);

	// Convert
	try {
		switch (mBayerType) {
		case 0 :
			cvCvtColor(inputimage, mOutputImage, CV_BayerBG2BGR);
			break;
		case 1 :
			cvCvtColor(inputimage, mOutputImage, CV_BayerGB2BGR);
			break;
		case 2 :
			cvCvtColor(inputimage, mOutputImage, CV_BayerRG2BGR);
			break;
		case 3 :
			cvCvtColor(inputimage, mOutputImage, CV_BayerGR2BGR);
			break;
		default :
			AddError(wxT("Invalid Bayer Pattern Type"));
			return;
		}
	} catch (...) {
		AddError(wxT("Conversion from Bayer to BGR failed."));
	}

	// Set the output image on the color data structure
	mCore->mDataStructureImageColor.mImage = mOutputImage;

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageColor.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageColor.mImage = 0;
}

void THISCLASS::OnStop() {
	if (mOutputImage) {
		cvReleaseImage(&mOutputImage);
	}
}
