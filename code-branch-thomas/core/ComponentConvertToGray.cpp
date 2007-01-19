#include "ComponentConvertToGray.h"
#define THISCLASS ComponentConvertToGray

THISCLASS::ComponentConvertToGray(SwisTrackCore *stc):
		Component(stc, "ConvertToGray") {

	// Data structure relations
	mDisplayName="Conversion to Grayscale(BGR)";
	mCategory=&(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
}

THISCLASS::~ComponentConvertToGray() {
}

void THISCLASS::OnStart() {
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	IplImage *inputimage=mCore->mDataStructureInput.mImage;
	if (! inputimage) {return;}
	
	try {
		// We convert the input image in black and white
		switch (inputimage->nChannels) {
		case 3:	// BGR case, we convert to gray
			PrepareOutputImage(inputimage);
			cvCvtColor(inputimage, mOutputImage, CV_BGR2GRAY);
			mCore->mDataStructureImageGray.mImage=mOutputImage;
			break;
		case 1:	// Already in Gray
			mCore->mDataStructureImageGray.mImage=inputimage;
			break;
		default:	// Other cases, we take the first channel
			PrepareOutputImage(inputimage);
			cvCvtPixToPlane(inputimage, mOutputImage, NULL, NULL, NULL);
			mCore->mDataStructureImageGray.mImage=mOutputImage;
			break;
		}
	} catch(...) {
		AddError("Convertion to gray failed.");
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageGray.mImage=0;
}

void THISCLASS::OnStop() {
	if (mOutputImage) {cvReleaseImage(&mOutputImage);}
}
