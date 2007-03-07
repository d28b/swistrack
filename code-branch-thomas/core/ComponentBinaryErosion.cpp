#include "ComponentBinaryErosion.h"
#define THISCLASS ComponentBinaryErosion

#include <sstream>

THISCLASS::ComponentBinaryErosion(SwisTrackCore *stc):
		Component(stc, "BinaryErosion"),
		mIterations(1),
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
	mIterations=GetConfigurationInt("Iterations", 1);
	
	// Check for stupid configurations
	if (mIterations<0) {
		AddError("The number of erosions must be greater or equal to 0.");
	}
}

void THISCLASS::OnStep() {
	if (! mCore->mDataStructureImageBinary.mImage) {
		AddError("No input image.");
		return;
	}

	if (mIterations>0) {
		cvErode(mCore->mDataStructureImageBinary.mImage, mCore->mDataStructureImageBinary.mImage, NULL, mIterations);
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
