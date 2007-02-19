#include "ComponentInputFileAVI.h"
#define THISCLASS ComponentInputFileAVI

#include <sstream>

THISCLASS::ComponentInputFileAVI(SwisTrackCore *stc):
		Component(stc, "FileAVI"),
		mCapture(0), mCurrentImage(0),
		mDisplayImageOutput("Output", "Output") {

	// Data structure relations
	mDisplayName="AVI File";
	mCategory=&(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplayImage(&mDisplayImageOutput);
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

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	if (! mCapture) {return;}	

	// Read the next frame
	int framenumber=(int)cvGetCaptureProperty(mCapture, CV_CAP_PROP_POS_FRAMES);
	int framecount=(int)cvGetCaptureProperty(mCapture, CV_CAP_PROP_FRAME_COUNT);
	mCurrentImage=cvQueryFrame(mCapture);
	if (! mCurrentImage) {
		AddError("Could not read frame from AVI file.");
		return;
	}

	// AVI files are flipped
	cvFlip(mCurrentImage, 0);

	// Set DataStructureImage
	mCore->mDataStructureInput.mImage=mCurrentImage;
	mCore->mDataStructureInput.mFrameNumber=framenumber;

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=mCurrentImage;
	std::ostringstream oss;
	oss << "Frame " << framenumber << " / " << framecount << ", " << mCurrentImage->width << "x" << mCurrentImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mImage=0;
	//mCurrentImage should not be released here, as this is handled by the HighGUI library
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
