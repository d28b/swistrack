#include "ComponentConvertToGray.h"
#define THISCLASS ComponentConvertToGray

THISCLASS::ComponentConvertToGray(SwisTrackCore *stc):
		Component(stc, "ConvertToGray") {

	mDisplayName="Convert to gray";
}

THISCLASS::~ComponentConvertToGray() {
}

bool THISCLASS::Start() {
	return true;
}

bool THISCLASS::Step() {
	IplImage *inputimage=mCore->mDataStructureImage.mImage;
	if (! inputimage) {return true;}
	
	try {
		// We convert the input image in black and white
		switch (inputImg->nChannels) {
		case 3:	// BGR case, we convert to gray
			PrepareImage(inputimage);
			cvCvtColor(inputimage, mImage, CV_BGR2GRAY);
			break;
		case 1:	// Already in Gray
			outputimage = inputimage;
			break;
		default:	// Other cases, we take the first channel
			outputimage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 1);
			cvCvtPixToPlane(inputimage, outputimage, NULL, NULL, NULL);
			break;
		}
		
		mCore->mDataStructureImageGray.mImage=outputimage;
	} catch(...) {
		AddError("Convertion to gray failed.");
		return false;
	}

	return true;
}

bool THISCLASS::Stop() {
	if (! mCapture) {return false;}	

	cvReleaseCapture(mCapture);
	return true;
}

double THISCLASS::GetFPS() {
	if (! mCapture) {return 0;}	
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_FPS);
}
