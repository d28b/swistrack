#include "ComponentInputFileImage.h"
#define THISCLASS ComponentInputFileImage

#include "DisplayEditor.h"

THISCLASS::ComponentInputFileImage(SwisTrackCore *stc):
		Component(stc, wxT("InputFileImage")),
		mOutputImage(0),
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
	// Load the image
	wxString filename_string = GetConfigurationString(wxT("File"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filename_string);
	if (! filename.IsOk()) {
		AddError(wxT("Invalid image file path/filename."));
		return;
	}
	mOutputImage = cvLoadImage(filename.GetFullPath().mb_str(wxConvFile), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	if (! mOutputImage) {
		AddError(wxT("Cannot open image file."));
		return;
	}
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	if (! mOutputImage) {
		return;
	}

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage = mOutputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mOutputImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mImage = 0;
	//mOutputImage should not be released here, as this is handled by the HighGUI library
}

void THISCLASS::OnStop() {
	cvReleaseImage(&mOutputImage);
}
