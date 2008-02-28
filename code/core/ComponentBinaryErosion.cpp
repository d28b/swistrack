#include "ComponentBinaryErosion.h"
#define THISCLASS ComponentBinaryErosion

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentBinaryErosion(SwisTrackCore *stc):
		Component(stc, "BinaryErosion"),
		mIterations(1),
		mDisplayOutput("Output", "After erosion") {

	// Data structure relations
	mCategory=&(mCore->mCategoryPreprocessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
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

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageBinary.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles=0;
}

void THISCLASS::OnStop() {
}
