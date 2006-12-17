#include "ComponentBackgroundSubtraction.h"
#define THISCLASS ComponentBackgroundSubtraction

THISCLASS::ComponentBackgroundSubtraction(SwisTrackCore *stc):
		Component(stc, "SegmenterBackgroundSubtraction"), mCapture(0), mLastImage(0) {

	mDisplayName="Background subtraction";
}

THISCLASS::~ComponentBackgroundSubtraction() {
	if (! mCapture) {return;}
	cvReleaseCapture(mCapture);
}

bool THISCLASS::Start() {
	binary = cvCreateImage(input->GetInputDim(),input->GetInputDepth(),1);
	binary->origin = input->GetInputOrigin();
	trackingimg->Init(input->GetInputIpl(),binary);

	status = READY_TO_START;


	IplImage* bg = cvLoadImage(GetConfigurationString("BackgroundImage", ""), -1); 						
	if (! bg) {
		status = CAN_NOT_OPEN_BACKGROUND_FILE;
		AddError("Cannot open background file.");
		return false;
	}

	background = cvCreateImage(input->GetInputDim(), input->GetInputDepth(), 1);

	switch (bg->nChannels) {
	case 3:	// BGR case, we convert in gray
		cvCvtColor(bg, background, CV_BGR2GRAY);
		break;
	case 1:	// Already in gray
		cvCopy(bg, background);
		break;
	default:	// other cases, we take the first channel
		cvCvtPixToPlane(bg, background, NULL, NULL, NULL);
		break;
	}			
			
	if (! background) {
		status = CAN_NOT_OPEN_BACKGROUND_FILE;
		throw "[Segmenter::Segmenter] Can not create background file";
	}
	
	//We always calculate the background average, so we can select if we use the moving threshold during the segmentation
	backgroundAverage=(int)cvMean(background);
	cvReleaseImage(&bg);

	mCore->mDataStructureImage.mIsInColor=(strcmp(mLastImage->colorModel, "GRAY")!=0);
}

bool THISCLASS::Step() {
	IplImage *inputimage=mCore->mDataStructureImage.mImage;
	if (! inputimage) {return true;}
	
	try {
		IplImage *outputimage;

		// Correct the tmpImage with the difference in image mean
		if (fixedThresholdBoolean==0)
			cvAddS(inputimage, cvScalar(backgroundAverage-cvMean(inputimage)), inputimage);

		// Background Substraction
		cvAbsDiff(inputimage, background, binary);
		cvThreshold(binary,binary, bin_threshold, 255,  CV_THRESH_BINARY);
	} catch(...) {
		AddError("Background subtraction failed.");
	}
	
	cvReleaseImage(&tmpImage);
	return true;
}

bool THISCLASS::Stop() {
	if (! mCapture) {return false;}	

	cvReleaseCapture(mCapture);
	return true;
}

double THISCLASS::GetFPS() {
	if (! mCapture) {return 0;}	
	return cvGetCaptureProperty(mCapture, CV_CAP_PROP_FPS);
}
