#include "ComponentConvertToGray.h"
#define THISCLASS ComponentConvertToGray

THISCLASS::ComponentConvertToGray(SwisTrackCore *stc, const std::string &displayname):
		Component(stc, "ConvertToGray", displayname) {

	// Data structure relations
	AddDataStructureRead(mCore->mDataStructureInput);
	AddDataStructureWrite(mCore->mDataStructureImageGray);
}

THISCLASS::~ComponentConvertToGray() {
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
		return false;
	}

	return true;
}

bool THISCLASS::StepCleanup() {
	mCore->mDataStructureImageGray.mImage=0;
	return true;
}

bool THISCLASS::Stop() {
	if (mOutputImage) {cvReleaseImage(mOutputImage);}
	return true;
}
