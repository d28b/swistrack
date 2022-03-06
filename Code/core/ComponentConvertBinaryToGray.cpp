#include "ComponentConvertBinaryToGray.h"
#define THISCLASS ComponentConvertBinaryToGray

#include "DisplayEditor.h"

THISCLASS::ComponentConvertBinaryToGray(SwisTrackCore * stc):
	Component(stc, wxT("ConvertBinaryToGray")),
	mDisplayOutput(wxT("Output"), wxT("After conversion to grayscale")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertBinaryToGray() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageBinary.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	mCore->mDataStructureImageGray.mImage = inputImage;

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(inputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
