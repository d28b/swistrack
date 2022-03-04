#include "ComponentOutputFileAVI.h"
#define THISCLASS ComponentOutputFileAVI

#include "DisplayEditor.h"

THISCLASS::ComponentOutputFileAVI(SwisTrackCore * stc):
	Component(stc, wxT("OutputFileAVI")),
	mInputChannel(COLOR),
	mWriter(), mFrameRate(15), mCodec(-1),
	mDisplayOutput(wxT("Output"), wxT("AVI File: Unprocessed Frame")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDisplay(&mDisplayOutput);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputFileAVI() {
}

void THISCLASS::OnStart() {
	wxFileName fileName = mCore->GetRunFileName(GetConfigurationString(wxT("FileTitle"), wxT("")));
	mFileName = fileName.GetFullPath().ToStdString();

	mFrameRate = GetConfigurationInt(wxT("FrameRate"), 15);

	wxString codecString = GetConfigurationString(wxT("Codec"), wxT("Def."));
	mCodec = codecString.Cmp(wxT("Def.")) == 0 ? -1 : cv::VideoWriter::fourcc(codecString.GetChar(0), codecString.GetChar(1), codecString.GetChar(2), codecString.GetChar(3));

	wxString inputChannelString = GetConfigurationString(wxT("InputChannel"), wxT(""));
	mInputChannel =
	    inputChannelString == wxT("grayscale") ? GRAYSCALE :
	    inputChannelString == wxT("binary") ? BINARY :
	    COLOR;

	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
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

	// Open the writer
	if (! mWriter.isOpened()) {
		bool success = mWriter.open(mFileName, mCodec, mFrameRate, inputImage.size(), inputImage.channels() == 3);
		if (! success) AddError(wxT("Error while creating the AVI file."));
	}

	// Add the frame to the buffer
	mWriter.write(inputImage);

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(inputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	mWriter.release();
}
