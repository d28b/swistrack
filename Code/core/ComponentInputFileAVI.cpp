#include "ComponentInputFileAVI.h"
#define THISCLASS ComponentInputFileAVI

#include <fstream>
#include "DisplayEditor.h"

THISCLASS::ComponentInputFileAVI(SwisTrackCore *stc):
		Component(stc, wxT("InputFileAVI")),
		mFlipVertically(false),
		mCapture(0), mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("AVI File: Unprocessed Frame")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputFileAVI() {
	if (mCapture) {
		cvReleaseCapture(&mCapture);
	}
}

void THISCLASS::OnStart() {
	// Open file
	wxString filename_string = GetConfigurationString(wxT("File"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filename_string);
	if (! filename.FileExists()) {
	  wxString msg = wxT("The avi file does not exist: ");
	  msg << filename_string;
	  AddError(msg);
	  return;
	}
	if (filename.IsOk()) {
	  mCapture = cvCreateFileCapture(filename.GetFullPath().mb_str(wxConvFile));
	}

	// Error? Check whether the file exists or not, to give an appropriate error message to the user
	if (mCapture == NULL) {
		std::fstream f;
		f.open(filename.GetFullPath().mb_str(wxConvFile));
		if (f.is_open()) {
			f.close();
			AddError(wxT("Can open the AVI file as a file, but not with OpenCV."));
			return;
		} else {
			AddError(wxT("Cannot open AVI file: permissions problem or something?"));
			return;
		}
	}

	// Reset to first frame
	cvSetCaptureProperty(mCapture, CV_CAP_PROP_POS_FRAMES, 0);
}

void THISCLASS::OnReloadConfiguration() {
	mFlipVertically = GetConfigurationBool(wxT("FlipVertically"), false);
}

void THISCLASS::OnStep() {
	if (! mCapture) {
		return;
	}

	// Read the next frame
	int framenumber = (int)cvGetCaptureProperty(mCapture, CV_CAP_PROP_POS_FRAMES);
	int framescount = (int)cvGetCaptureProperty(mCapture, CV_CAP_PROP_FRAME_COUNT);
	mOutputImage = cvQueryFrame(mCapture);
	if (! mOutputImage) {
		AddError(wxT("Finished reading AVI file."));
		mCore->TriggerStop();
		return;
	}

	// Flip the image if desired
	if (mFlipVertically) {
		cvFlip(mOutputImage, 0, 0);
	}

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage = mOutputImage;
	mCore->mDataStructureInput.mFrameNumber = framenumber;
	mCore->mDataStructureInput.mFramesCount = framescount;

	// On linux, directly readig the millisecond offset doesn't work.
	//double progress = GetProgressMSec();
	double secondsInVideo = GetProgressFrameNumber() / GetFPS();
	wxDateTime ts((time_t) secondsInVideo);
	double millis = (secondsInVideo - (int) secondsInVideo) * 1000;
	ts.SetMillisecond((int) millis);
	mCore->mDataStructureInput.SetFrameTimestamp(ts);

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mOutputImage);
	}
}

void THISCLASS::OnStepCleanup() {
  //	mCore->mDataStructureInput.mImage = 0;
	//mOutputImage should not be released here, as this is handled by the HighGUI library
}

void THISCLASS::OnStop() {
	if (mCapture) {
		cvReleaseCapture(&mCapture);
	}
}

double THISCLASS::GetProgressPercent() {
	if (! mCapture) {
		return 0;
	}
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_POS_AVI_RATIO);
}

double THISCLASS::GetProgressMSec() {
	if (! mCapture) {
		return 0;
	}
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_POS_MSEC);
}

int THISCLASS::GetProgressFrameNumber() {
	if (! mCapture) {
		return 0;
	}
	return (int)cvGetCaptureProperty(mCapture, CV_CAP_PROP_POS_FRAMES);
}

double THISCLASS::GetFPS() {
	if (! mCapture) {
		return 0;
	}
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_FPS);
}
