#include "ComponentBinaryDilation.h"
#define THISCLASS ComponentBinaryDilation

#include <sstream>

THISCLASS::ComponentBinaryDilation(SwisTrackCore *stc):
		Component(stc, "BinaryDilation"),
		mNumber(10),
		mDisplayImageOutput("Output", "After dilation") {

	// Data structure relations
	mDisplayName="Binary image dilation";
	mCategory=&(mCore->mCategoryBinaryPreprocessing);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentBinaryDilation() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	mNumber=GetConfigurationInt("Number", 1);
	
	// Check for stupid configurations
	if (mNumber<0) {
		AddError("The number of dilations must be greater or equal to 0.");
	}
}

void THISCLASS::OnStep() {
	if (! mCore->mDataStructureImageBinary.mImage) {
		AddError("No input image.");
		return;
	}

	if (mNumber>0) {
		cvDilate(mCore->mDataStructureImageBinary.mImage, mCore->mDataStructureImageBinary.mImage, NULL, mNumber);
	}

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=mCore->mDataStructureImageBinary.mImage;
	std::ostringstream oss;
	oss << "Binary image, " << mCore->mDataStructureImageBinary.mImage->width << "x" << mCore->mDataStructureImageBinary.mImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
