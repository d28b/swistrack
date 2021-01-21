#include "ComponentOutputFramesImages.h"
#define THISCLASS ComponentOutputFramesImages

#include "DisplayEditor.h"
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

THISCLASS::ComponentOutputFramesImages(SwisTrackCore *stc):
		Component(stc, wxT("OutputFramesImages")),
		mInputChannel(cInputChannel_None),
		mDisplayOutput(wxT("Output"), wxT("FramesImages: Unprocessed Frame")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDisplay(&mDisplayOutput);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputFramesImages() {
}

void THISCLASS::OnStart() {
	wxString filename_string = GetConfigurationString(wxT("FilePrefix"), wxT(""));
	mFileName = mCore->GetRunFileName(filename_string);
	// make sure that subdirectory exists:
	mFileName.Mkdir(mFileName.GetPath(), 0777, wxPATH_MKDIR_FULL);
	mFileType = (eFileType)GetConfigurationInt(wxT("FileType"), 0);
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	wxString inputchannel = GetConfigurationString(wxT("InputChannel"), wxT(""));
	if (inputchannel == wxT("color")) {
		mInputChannel = cInputChannel_Color;
	} else if (inputchannel == wxT("grayscale")) {
		mInputChannel = cInputChannel_Grayscale;
	} else if (inputchannel == wxT("binary")) {
		mInputChannel = cInputChannel_Binary;
	} else {
		mInputChannel = cInputChannel_None;
	}
}

void THISCLASS::OnStep() {
	// Get the input image
	IplImage* inputimage;
	switch (mInputChannel) {
	case cInputChannel_Color:
		inputimage = mCore->mDataStructureImageColor.mImage;
		break;
	case cInputChannel_Grayscale:
		inputimage = mCore->mDataStructureImageGray.mImage;
		break;
	case cInputChannel_Binary:
		inputimage = mCore->mDataStructureImageBinary.mImage;
		break;
	default:
		AddError(wxT("Invalid input image"));
		return;
	}

	if (! inputimage) {
		AddError(wxT("No image on selected input."));
		return;
	}

	char *fileExtension;
	switch (mFileType) {
	case 0:
		fileExtension = ".png";
		break;
	case 1:
		fileExtension = ".bmp";
		break;
	case 2:
		fileExtension = ".jpeg";
	}
	std::ostringstream filename_oss;
	filename_oss << mFileName.GetFullPath().mb_str(wxConvFile) << "-" << std::setw(8) << std::setfill('0') << mCore->GetStepCounter() << fileExtension;
	cvSaveImage(filename_oss.str().c_str(), inputimage );

	// Image is always top down in Swistrack
	inputimage->origin = 0;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(inputimage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
