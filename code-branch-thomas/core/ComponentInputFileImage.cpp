#include "ComponentInputFileImage.h"
#define THISCLASS ComponentInputFileImage

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentInputFileImage(SwisTrackCore *stc):
		Component(stc, "InputFileImage"),
		mOutputImage(0), mFrameNumber(0),
		mDisplayOutput("Output", "Input image") {

	// Data structure relations
	mDisplayName="Input from image file";
	mCategory=&(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputFileImage() {
}

void THISCLASS::OnStart() {
	// Load the image
	std::string filename=GetConfigurationString("File", "");
	if (filename!="") {
		mOutputImage=cvLoadImage(filename.c_str(), -1);
	}
	if (! mOutputImage) {
		AddError("Cannot open image file.");
		return;
	}

	// Set the frame number to 0
	mFrameNumber=0;
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	if (! mOutputImage) {return;}

	mFrameNumber++;

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage=mOutputImage;
	mCore->mDataStructureInput.mFrameNumber=mFrameNumber;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mOutputImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mImage=0;
	//mOutputImage should not be released here, as this is handled by the HighGUI library
}

void THISCLASS::OnStop() {
	cvReleaseImage(&mOutputImage);
}
