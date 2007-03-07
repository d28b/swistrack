#include "ComponentBinaryErosion.h"
#define THISCLASS ComponentBinaryErosion

#include <sstream>

THISCLASS::ComponentBinaryErosion(SwisTrackCore *stc):
		Component(stc, "BlobDetectionMinMax"),
		mNumber(10),
		mDisplayImageOutput("Output", "After erosion") {

	// Data structure relations
	mDisplayName="Binary image erosion";
	mCategory=&(mCore->mCategoryBinaryPreprocessing);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentBinaryErosion() {
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
		cvErode(mCore->mDataStructureImageBinary.mImage, mCore->mDataStructureImageBinary.mImage, NULL, mNumber);
	}

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=mCore->mDataStructureImageBinary.mImage;
	std::ostringstream oss;
	oss << "Binary image, " << mCore->mDataStructureImageBinary.mImage->width << "x" << mCore->mDataStructureImageBinary.mImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles=0;
}

void THISCLASS::OnStop() {
}

double THISCLASS::GetContourCompactness(const void* contour) {
	double l = cvArcLength(contour, CV_WHOLE_SEQ, 1);
	return fabs(12.56*cvContourArea(contour)/(l*l));	
}
