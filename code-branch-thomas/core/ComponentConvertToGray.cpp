#include "ComponentConvertToGray.h"
#define THISCLASS ComponentConvertToGray

#include <sstream>

THISCLASS::ComponentConvertToGray(SwisTrackCore *stc):
		Component(stc, "ConvertToGray"),
		mOutputImage(0),
		mDisplayImageOutput("Output", "Output") {

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
		// We convert the input image to black and white
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

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=mOutputImage;
	std::ostringstream oss;
	oss << "Grayscale image, " << mOutputImage->width << "x" << mOutputImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageGray.mImage=0;
}

void THISCLASS::OnStop() {
	if (mOutputImage) {cvReleaseImage(&mOutputImage);}
}
