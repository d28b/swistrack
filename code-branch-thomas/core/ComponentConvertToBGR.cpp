#include "ComponentConvertToBGR.h"
#define THISCLASS ComponentConvertToBGR

#include <sstream>

THISCLASS::ComponentConvertToBGR(SwisTrackCore *stc):
		Component(stc, "ConvertToBGR"),
		mOutputImage(0),
		mDisplayImageOutput("Output", "Output") {

	// Data structure relations
	mDisplayName="Conversion to Color (BGR)";
	mCategory=&(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureWrite(&(mCore->mDataStructureImageBGR));
}

THISCLASS::~ComponentConvertToBGR() {
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
	}

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=mOutputImage;
	std::ostringstream oss;
	oss << "BGR image, " << mOutputImage->width << "x" << mOutputImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageBGR.mImage=0;
}

void THISCLASS::OnStop() {
	if (mOutputImage) {cvReleaseImage(&mOutputImage);}
}
