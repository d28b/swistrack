#include "ComponentInputFileAVI.h"
#define THISCLASS ComponentInputFileAVI

THISCLASS::ComponentInputFileAVI(SwisTrackCore *stc):
		Component(stc, "FileAVI"), mCapture(0), mLastImage(0) {

	// User-friendly information about this component
	mDisplayName="AVI file";
	AddDataStructureWrite(mCore->mDataStructureImage);
	AddDataStructureWrite(mCore->mDataStructureInput);
}

THISCLASS::~ComponentInputFileAVI() {
	if (! mCapture) {return;}
	cvReleaseCapture(mCapture);
}

bool THISCLASS::Start() {
	// Open file
	std::string filename=GetConfigurationString("File", "");
	mCapture = cvCaptureFromFile(filename);
	
	// Error? Check whether the file exists or not, to give an appropriate error message to the user
	if (! mCapture) {
		FILE* f;
#ifdef VS2003
		f=fopen(filename, "r");
#else
		fopen_s(&f, filename, "r");
#endif
		if (f) {
			fclose(f);
			AddError("Cannot open AVI file: codec problem, VFW codec required, not DirectShow.");
			return false;
		} else {
#ifdef VS2003
			f = fopen("swistrack.log", "w");
#else
			fopen_s(&f,"swistrack.log", "w");
#endif
			fprintf(f, "%s not found", filename);
			fclose(f);
			AddError("Cannot open AVI file: file not found.");
			return false;
		}
	}

	// Reset to first frame
	cvSetCaptureProperty(mCapture, CV_CAP_PROP_POS_FRAMES, 0);
	return true;
}

bool THISCLASS::Step() {
	if (! mCapture) {return false;}	

	mLastImage=cvQueryFrame(mCapture);
	if (! mLastImage) {
		AddError("Could not read frame from AVI file.");
		return false;
	}

	// AVI files are flipped
	cvFlip(mLastImage, 0);

	// Set DataStructureImage
	mCore->mDataStructureImage.mImage=inputimage;
	mCore->mDataStructureInput.mFrameNumber++;

	// Show status
	std::ostringstream oss;
	oss << "Frame " << mCore->mDataStructureImage.mFrameNumber << "(" << GetProgressPercent() << " %)";
	AddInfo(oss.str);
	return true;

}

bool THISCLASS::StepCleanup() {
	mCore->mDataStructureImage.mImage=0;
	if (mLastImage) {cvReleaseImage(mLastImage);}
	return true;
}

bool THISCLASS::Stop() {
	if (mCapture) {cvReleaseCapture(mCapture);}
	return true;
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
