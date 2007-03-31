#include "ComponentInputFileImage.h"
#define THISCLASS ComponentInputFileImage

#include <sstream>

THISCLASS::ComponentInputFileImage(SwisTrackCore *stc):
		Component(stc, "InputFileImage"),
		mOutputImage(0),
		mDisplayImageOutput("Output", "Input image") {

	// Data structure relations
	mDisplayName="Input from image file";
	mCategory=&(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentInputFileImage() {
}

void THISCLASS::OnStart() {
	std::string filename=GetConfigurationString("File", "");
	if (filename!="") {
		mOutputImage=cvLoadImage(filename.c_str(), -1);
	}
	if (! mOutputImage) {
		AddError("Cannot open image file.");
		return;
	}
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	if (! mOutputImage) {return;}

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage=mOutputImage;
	mCore->mDataStructureInput.mFrameNumber=0;

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=mOutputImage;
	std::ostringstream oss;
	oss << "Image from file, " << mOutputImage->width << "x" << mOutputImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mImage=0;
	//mOutputImage should not be released here, as this is handled by the HighGUI library
}

void THISCLASS::OnStop() {
	cvReleaseImage(&mOutputImage);
}
