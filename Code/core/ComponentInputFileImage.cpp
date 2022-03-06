#include "ComponentInputFileImage.h"
#define THISCLASS ComponentInputFileImage

#include "DisplayEditor.h"

THISCLASS::ComponentInputFileImage(SwisTrackCore * stc):
	Component(stc, wxT("InputFileImage")),
	mInputImage(),
	mDisplayOutput(wxT("Output"), wxT("Input image")) {

	// Data structure relations
	mDisplayName = wxT("Input from image file");
	mCategory = &(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputFileImage() {
}

void THISCLASS::OnStart() {
	mInputImage = LoadConfigurationImage(wxT("File"), wxT("Input image"), cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	mCore->mDataStructureInput.mImage = mInputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(mInputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	mInputImage.release();
}
