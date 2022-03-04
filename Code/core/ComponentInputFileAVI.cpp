#include "ComponentInputFileAVI.h"
#define THISCLASS ComponentInputFileAVI

#include <fstream>
#include "DisplayEditor.h"

THISCLASS::ComponentInputFileAVI(SwisTrackCore * stc):
	Component(stc, wxT("InputFileAVI")),
	mFlipHorizontally(false), mFlipVertically(false),
	mCapture(),
	mDisplayOutput(wxT("Output"), wxT("AVI File: Unprocessed Frame")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputFileAVI() {
}

void THISCLASS::OnStart() {
	// Open file
	wxFileName filename = mCore->GetProjectFileName(GetConfigurationString(wxT("File"), wxT("")));
	if (! filename.IsOk()) {
		AddError(wxT("The video file name is invalid."));
		return;
	}

	if (! mCapture.open(filename.GetFullPath().ToStdString())) {
		AddError(wxT("The video file '") + filename.GetFullPath() + wxT("' could not be opened."));
		return;
	}

	// Reset to first frame
	mCapture.set(cv::CAP_PROP_POS_FRAMES, 0);
}

void THISCLASS::OnReloadConfiguration() {
	mFlipHorizontally = GetConfigurationBool(wxT("FlipHorizontally"), false);
	mFlipVertically = GetConfigurationBool(wxT("FlipVertically"), false);
}

void THISCLASS::OnStep() {
	// Read the next frame
	int frameNumber = (int) mCapture.get(cv::CAP_PROP_POS_FRAMES);
	int framesCount = (int) mCapture.get(cv::CAP_PROP_FRAME_COUNT);

	cv::Mat image;
	if (! mCapture.read(image)) {
		AddError(wxT("Finished reading AVI file."));
		mCore->TriggerStop();
		return;
	}

	// Flip the image if desired
	if (mFlipHorizontally && mFlipVertically) {
		cv::Mat flippedImage(image.size(), CV_8UC3);
		cv::flip(image, flippedImage, -1);
		image = flippedImage;
	} else if (mFlipHorizontally) {
		cv::Mat flippedImage(image.size(), CV_8UC3);
		cv::flip(image, flippedImage, 0);
		image = flippedImage;
	} else if (mFlipVertically) {
		cv::Mat flippedImage(image.size(), CV_8UC3);
		cv::flip(image, flippedImage, 1);
		image = flippedImage;
	}

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage = image;
	mCore->mDataStructureInput.mFrameNumber = frameNumber;
	mCore->mDataStructureInput.mFramesCount = framesCount;

	// On linux, directly readig the millisecond offset doesn't work.
	//double progress = GetProgressMSec();
	double secondsInVideo = GetProgressFrameNumber() / GetFPS();
	wxDateTime ts((time_t) secondsInVideo);
	double millis = (secondsInVideo - (int) secondsInVideo) * 1000;
	ts.SetMillisecond((int) millis);
	mCore->mDataStructureInput.SetFrameTimestamp(ts);

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(image);
}

void THISCLASS::OnStepCleanup() {
	//	mCore->mDataStructureInput.mImage = 0;
	//mOutputImage should not be released here, as this is handled by the HighGUI library
}

void THISCLASS::OnStop() {
	mCapture.release();
}

double THISCLASS::GetProgressPercent() {
	return mCapture.get(cv::CAP_PROP_POS_AVI_RATIO);
}

double THISCLASS::GetProgressMSec() {
	return mCapture.get(cv::CAP_PROP_POS_MSEC);
}

int THISCLASS::GetProgressFrameNumber() {
	return (int) mCapture.get(cv::CAP_PROP_POS_FRAMES);
}

double THISCLASS::GetFPS() {
	return mCapture.get(cv::CAP_PROP_FPS);
}
