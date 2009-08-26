#include "ComponentOutputFileAVI.h"
#define THISCLASS ComponentOutputFileAVI

#include "DisplayEditor.h"

THISCLASS::ComponentOutputFileAVI(SwisTrackCore *stc):
		Component(stc, wxT("OutputFileAVI")),
		mWriter(0), mFrameRate(15), mInputChannel(cInputChannel_None), mCodecString(wxString(wxT("Def."))),
		mFrameBufferCount(1), mFrameBuffer(0), mFrameBufferWriteCounter(0),
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
	BufferedFrames_Allocate(0);
}

void THISCLASS::OnStart() {
	wxString filename_string = GetConfigurationString(wxT("FileTitle"), wxT(""));
	mFileName = mCore->GetRunFileName(filename_string);
	mFrameRate = GetConfigurationInt(wxT("FrameRate"), 15);
	BufferedFrames_Allocate(GetConfigurationInt(wxT("FrameBufferCount"), 1));
	mCodecString = GetConfigurationString(wxT("Codec"), wxT("Def."));
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

	// Select the codec
	int codecValue;
	if (mCodecString.Cmp(wxT("Def.")))
		codecValue = CV_FOURCC(mCodecString.GetChar(0), mCodecString.GetChar(1), mCodecString.GetChar(2), mCodecString.GetChar(3));
	else
		codecValue = -1;

	// Create the Writer
	if (! mWriter) {
		if (inputimage->nChannels == 3) {
			wxString toto = mFileName.GetFullPath();
			mWriter = cvCreateVideoWriter(mFileName.GetFullPath().mb_str(wxConvFile), codecValue, mFrameRate, cvGetSize(inputimage));
		} else if (inputimage->nChannels == 1) {
			mWriter = cvCreateVideoWriter(mFileName.GetFullPath().mb_str(wxConvFile), codecValue, mFrameRate, cvGetSize(inputimage), 0);
		} else {
			AddError(wxT("Input image must have 1 or 3 channels"));
			return;
		}

		if (! mWriter) {
			AddError(wxT("Error while creating the AVI file."));
			return;
		}
	}

	// Image is always top down in Swistrack
	inputimage->origin = 0;

	// Add the frame to the buffer
	BufferedFrames_Add(inputimage);

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(inputimage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	BufferedFrames_Write();
	if (mWriter) {
		cvReleaseVideoWriter(&mWriter);
	}
}

void THISCLASS::BufferedFrames_Allocate(int count) {
	// Check bounds
	if (count < 1) {
		count = 1;
	}
	if (count > 1000) {
		count = 1000;
	}

	// If the value did not change, return
	if (count == mFrameBufferCount) {
		return;
	}

	// Deallocate old buffer, and allocate new buffer if necessary
	if (mFrameBuffer) {
		for (int i = 0; i < mFrameBufferWriteCounter; i++) {
			cvReleaseImage(&(mFrameBuffer[i]));
		}
		delete [] mFrameBuffer;
	}
	if (count == 1) {
		return;
	}
	mFrameBufferCount = count;
	mFrameBuffer = new IplImage*[mFrameBufferCount];
}

void THISCLASS::BufferedFrames_Add(IplImage *inputimage) {
	// No buffering?
	if (! mFrameBuffer) {
		cvWriteFrame(mWriter, inputimage);
		return;
	}

	// Add to the buffer, and write the whole buffer if it is full
	mFrameBuffer[mFrameBufferWriteCounter] = cvCloneImage(inputimage);
	mFrameBufferWriteCounter++;
	if (mFrameBufferWriteCounter >= mFrameBufferCount) {
		BufferedFrames_Write();
	}
}

void THISCLASS::BufferedFrames_Write() {
	for (int i = 0; i < mFrameBufferWriteCounter; i++) {
		cvWriteFrame(mWriter, mFrameBuffer[i]);
		cvReleaseImage(&(mFrameBuffer[i]));
	}
	mFrameBufferWriteCounter = 0;
}
