#include "ComponentInputFileAVI.h"
#define THISCLASS ComponentInputFileAVI

#include <sstream>
#include <fstream>
#include "DisplayEditor.h"

THISCLASS::ComponentInputFileAVI(SwisTrackCore *stc):
		Component(stc, "InputFileAVI"),
		mCapture(0), mOutputImage(0),
		mDisplayOutput("Output", "AVI File: Unprocessed Frame") {

	// Data structure relations
	mCategory=&(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputFileAVI() {
	if (mCapture) {cvReleaseCapture(&mCapture);}
}

void THISCLASS::OnStart() {
	// Open file
	std::string filename=GetConfigurationString("File", "");
	mCapture = cvCaptureFromFile(filename.c_str());
	
	// Error? Check whether the file exists or not, to give an appropriate error message to the user
	if (mCapture == NULL) {
        std::fstream f;
        f.open(filename.c_str());
		if (f.is_open()) {
			f.close();
			AddError("Cannot open AVI file: codec problem, VFW codec required, not DirectShow.");
			return;
		} else {
			AddError("Cannot open AVI file: file not found.");
			return;
		}
	}

	// Reset to first frame
	cvSetCaptureProperty(mCapture, CV_CAP_PROP_POS_FRAMES, 0);
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	if (! mCapture) {return;}	

	// Read the next frame
	int framenumber=(int)cvGetCaptureProperty(mCapture, CV_CAP_PROP_POS_FRAMES);
	int framescount=(int)cvGetCaptureProperty(mCapture, CV_CAP_PROP_FRAME_COUNT);
	mOutputImage=cvQueryFrame(mCapture);
	if (! mOutputImage) {
		AddError("Could not read frame from AVI file.");
		return;
	}

	// AVI files are flipped
	cvFlip(mOutputImage, 0);

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage=mOutputImage;
	mCore->mDataStructureInput.mFrameNumber=framenumber;
	mCore->mDataStructureInput.mFramesCount=framescount;

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
	if (mCapture) {cvReleaseCapture(&mCapture);}
}

double THISCLASS::GetProgressPercent() {
	if (! mCapture) {return 0;}
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_POS_AVI_RATIO);
}

double THISCLASS::GetProgressMSec() {
	if (! mCapture) {return 0;}
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_POS_MSEC);
}

int THISCLASS::GetProgressFrameNumber() {
	if (! mCapture) {return 0;}
	return (int)cvGetCaptureProperty(mCapture, CV_CAP_PROP_POS_FRAMES);
}

double THISCLASS::GetFPS() {
	if (! mCapture) {return 0;}	
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_FPS);
}
