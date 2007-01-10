#include "ComponentInputFileAVI.h"
#define THISCLASS ComponentInputFileAVI

THISCLASS::ComponentInputFileAVI(SwisTrackCore *stc):
		Component(stc, "FileAVI"),
		mCapture(0), mLastImage(0) {

	// Data structure relations
	mDisplayName="AVI File";
	mCategory=mCore->mCategoryInput;
	AddDataStructureWrite(&(mCore->mDataStructureInput));
}

THISCLASS::~ComponentInputFileAVI() {
	if (mCapture) {cvReleaseCapture(&mCapture);}
}

void THISCLASS::OnStart() {
	// Open file
	std::string filename=GetConfigurationString("File", "");
	mCapture = cvCaptureFromFile(filename.c_str());
	
	// Error? Check whether the file exists or not, to give an appropriate error message to the user
	if (! mCapture) {
		FILE* f;
#ifdef VS2003
		f=fopen(filename, "r");
#else
		fopen_s(&f, filename.c_str(), "r");
#endif
		if (f) {
			fclose(f);
			AddError("Cannot open AVI file: codec problem, VFW codec required, not DirectShow.");
			return;
		} else {
#ifdef VS2003
			f = fopen("swistrack.log", "w");
#else
			fopen_s(&f,"swistrack.log", "w");
#endif
			fprintf(f, "%s not found", filename);
			fclose(f);
			AddError("Cannot open AVI file: file not found.");
			return;
		}
	}

	// Reset to first frame
	cvSetCaptureProperty(mCapture, CV_CAP_PROP_POS_FRAMES, 0);
}

void THISCLASS::OnStep() {
	if (! mCapture) {return;}	

	mLastImage=cvQueryFrame(mCapture);
	if (! mLastImage) {
		AddError("Could not read frame from AVI file.");
		return;
	}

	// AVI files are flipped
	cvFlip(mLastImage, 0);

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage=mLastImage;
	mCore->mDataStructureInput.mFrameNumber++;
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mImage=0;
	if (mLastImage) {cvReleaseImage(&mLastImage);}
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
