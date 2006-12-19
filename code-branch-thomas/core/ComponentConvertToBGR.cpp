#include "ComponentConvertToBGR.h"
#define THISCLASS ComponentConvertToBGR

THISCLASS::ComponentConvertToBGR(SwisTrackCore *stc):
		Component(stc, "ConvertToBGR") {

	// User-friendly information about this component
	mDisplayName="Convert to colors (BGR)";
	AddDataStructureRead(mCore->mDataStructureInput);
	AddDataStructureWrite(mCore->mDataStructureImageBGR);
}

THISCLASS::~ComponentConvertToBGR() {
}

bool THISCLASS::Start() {
	return true;
}

bool THISCLASS::Step() {
	IplImage *inputimage=mCore->mDataStructureInput.mImage;
	if (! inputimage) {return true;}
	
	try {
		// We convert the input image in black and white
		switch (inputimage->nChannels) {
		case 3:	// Already in BGR
			mCore->mDataStructureImageBGR.mImage=inputimage;
			break;
		case 1:	// Gray, convert to BGR
			PrepareOutputImage(inputimage);
			cvCvtColor(inputimage, mOutputImage, CV_GRAY2BGR);
			mCore->mDataStructureImageBGR.mImage=mOutputImage;
			break;
		default:	// Other cases, we take the first channel
			PrepareOutputImage(inputimage);
			cvCvtPixToPlane(inputimage, mOutputImage, NULL, NULL, NULL);
			mCore->mDataStructureImageBGR.mImage=mOutputImage;
			break;
		}
	} catch(...) {
		AddError("Convertion to gray failed.");
		return false;
	}

	return true;
}

bool THISCLASS::StepCleanup() {
	mCore->mDataStructureImageBGR.mImage=0;
	return true;
}

bool THISCLASS::Stop() {
	if (mOutputImage) {cvReleaseImage(mOutputImage);}
	return true;
}
