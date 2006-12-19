#include "ComponentThresholdGray.h"
#define THISCLASS ComponentThresholdGray

THISCLASS::ComponentThresholdGray(SwisTrackCore *stc):
		Component(stc, "Threshold"), mCapture(0), mLastImage(0) {

	// User-friendly information about this component
	mDisplayName="Thresholding";
	AddDataStructureWrite(mCore->mDataStructureImage);
	AddDataStructureWrite(mCore->mDataStructureImageBinary);
}

THISCLASS::~ComponentThresholdGray() {
	if (! mCapture) {return;}
	cvReleaseCapture(mCapture);
}

bool THISCLASS::Start() {
	binary = cvCreateImage(input->GetInputDim(),input->GetInputDepth(),1);
	binary->origin = input->GetInputOrigin();
	trackingimg->Init(input->GetInputIpl(),binary);

	IplImage* bg = cvLoadImage(GetConfigurationString("BackgroundImage", ""), -1);
	if (! bg) {
		AddError("Cannot open background file.");
		return false;
	}

	mBackgroundImage=cvCreateImage(bg->GetInputDim(), bg->GetInputDepth(), 1);
	if (! mBackgroundImage) {
		AddError("Cannot create background image.");
		return false;
	}

	switch (bg->nChannels) {
	case 3:	// BGR case, we convert in gray
		cvCvtColor(bg, mBackgroundImage, CV_BGR2GRAY);
		break;
	case 1:	// Already in gray
		cvCopy(bg, mBackgroundImage);
		break;
	default:	// other cases, we take the first channel
		cvCvtPixToPlane(bg, mBackgroundImage, NULL, NULL, NULL);
		break;
	}

	// We always calculate the background average, so we can select if we use the moving threshold during the segmentation
	mBackgroundImageMean=(int)cvMean(mBackgroundImage);
	
	// Release the temporary image
	cvReleaseImage(&bg);
}

bool THISCLASS::Step() {
	IplImage *inputimage=mCore->mDataStructureImageGray.mImage;
	if (! inputimage) {return true;}


	IplImage *inputimage=mCore->mDataStructureImage.mImage;
	if (! inputimage) {return true;}
	

	try {
		// Threshold the image
		cvThreshold(binary, binary, bin_threshold, 255, CV_THRESH_BINARY);
	} catch(...) {
		AddError("Background subtraction failed.");
	}
	
	cvReleaseImage(&tmpImage);
	return true;
}

bool THISCLASS::Stop() {
	if (! mBackgroundImage) {return false;}	

	cvReleaseCapture(mBackgroundImage);
	return true;
}

double THISCLASS::GetFPS() {
	if (! mCapture) {return 0;}	
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_FPS);
}
