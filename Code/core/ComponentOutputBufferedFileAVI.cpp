#include "ComponentOutputBufferedFileAVI.h"
#define THISCLASS ComponentOutputBufferedFileAVI

#include "DisplayEditor.h"

THISCLASS::ComponentOutputBufferedFileAVI(SwisTrackCore * stc):
	Component(stc, wxT("OutputBufferedFileAVI")),
	mInputChannel(COLOR),
	mBuffer(), mBufferSize(0), mKeepLast(false),
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

THISCLASS::~ComponentOutputBufferedFileAVI() {
}

void THISCLASS::OnStart() {
	wxString inputChannelString = GetConfigurationString(wxT("InputChannel"), wxT(""));
	mInputChannel =
	    inputChannelString == wxT("grayscale") ? GRAYSCALE :
	    inputChannelString == wxT("binary") ? BINARY :
	    COLOR;

	while (! mBuffer.empty()) mBuffer.pop();
	mBufferSize = GetConfigurationInt(wxT("FrameRate"), 15);
	mKeepLast = GetConfigurationString(wxT("Keep"), wxT("last")) == wxT("last");

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

	if (mBuffer.size() < mBufferSize) {
		mBuffer.push(inputImage);
	} else if (mKeepLast) {
		mBuffer.pop();
		mBuffer.push(inputImage);
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(inputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	// No video
	if (mBuffer.size() == 0) {
		AddWarning(wxT("No images acquired. Not writing any video."));
		return;
	}

	// Use the first image as reference
	cv::Mat firstImage = mBuffer.front();

	// Read the configuration
	wxFileName fileName = mCore->GetRunFileName(GetConfigurationString(wxT("FileTitle"), wxT("")));
	int frameRate = GetConfigurationInt(wxT("FrameRate"), 15);
	wxString codecString = GetConfigurationString(wxT("Codec"), wxT("Def."));
	int codec = codecString.Cmp(wxT("Def.")) == 0 ? -1 : cv::VideoWriter::fourcc(codecString.GetChar(0), codecString.GetChar(1), codecString.GetChar(2), codecString.GetChar(3));

	// Open the writer
	cv::VideoWriter writer;
	bool success = writer.open(fileName.GetFullPath().ToStdString(), codec, frameRate, firstImage.size(), firstImage.channels() == 3);
	if (! success) AddError(wxT("Error while creating the video file."));

	// Write all frames
	writer.write(firstImage);
	while (! mBuffer.empty()) {
		writer.write(mBuffer.front());
		mBuffer.pop();
	}

	// Close
	writer.release();
}
