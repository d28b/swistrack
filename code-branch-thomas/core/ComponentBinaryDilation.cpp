#include "ComponentBinaryDilation.h"
#define THISCLASS ComponentBinaryDilation

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentBinaryDilation(SwisTrackCore *stc):
		Component(stc, "BinaryDilation"),
		mIterations(1),
		mDisplayOutput("Output", "After dilation") {

	// Data structure relations
	mCategory=&(mCore->mCategoryPreprocessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBinaryDilation() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	mIterations=GetConfigurationInt("Iterations", 1);
	
	// Check for stupid configurations
	if (mIterations<0) {
		AddError("The number of dilations must be greater or equal to 0.");
	}
}

void THISCLASS::OnStep() {
	if (! mCore->mDataStructureImageBinary.mImage) {
		AddError("No input image.");
		return;
	}

	if (mIterations>0) {
		cvDilate(mCore->mDataStructureImageBinary.mImage, mCore->mDataStructureImageBinary.mImage, NULL, mIterations);
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageBinary.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
