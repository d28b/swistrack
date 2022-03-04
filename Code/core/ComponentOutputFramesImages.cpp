#include "ComponentOutputFramesImages.h"
#define THISCLASS ComponentOutputFramesImages

#include "DisplayEditor.h"
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

THISCLASS::ComponentOutputFramesImages(SwisTrackCore * stc):
	Component(stc, wxT("OutputFramesImages")),
	mInputChannel(COLOR),
	mFileBase(""), mFileExtension("png"),
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
	wxFileName fileName = mCore->GetRunFileName(GetConfigurationString(wxT("FileBase"), wxT("")));
	fileName.Mkdir(fileName.GetPath(), 0777, wxPATH_MKDIR_FULL);
	mFileBase = fileName.GetFullPath().ToStdString();
	mFileExtension = GetConfigurationString(wxT("FileExtension"), wxT("png"));
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	wxString inputChannelString = GetConfigurationString(wxT("InputChannel"), wxT(""));
	mInputChannel =
	    inputChannelString == wxT("grayscale") ? GRAYSCALE :
	    inputChannelString == wxT("binary") ? BINARY :
	    COLOR;
}

void THISCLASS::OnStep() {
	// Get the input image
	cv::Mat inputImage =
	    mInputChannel == GRAYSCALE ? mCore->mDataStructureImageGray.mImage :
	    mInputChannel == BINARY ? mCore->mDataStructureImageBinary.mImage :
	    mCore->mDataStructureImageColor.mImage;

	if (inputImage.empty()) {
		AddError(wxT("No image on selected input."));
		return;
	}

	std::stringstream imageFileName;
	imageFileName << mFileBase << std::setw(8) << std::setfill('0') << mCore->GetStepCounter() << "." << mFileExtension;
	cv::imwrite(imageFileName.str(), inputImage);

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(inputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
