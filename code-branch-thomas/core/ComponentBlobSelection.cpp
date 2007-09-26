#include "ComponentBlobSelection.h"
#define THISCLASS ComponentBlobSelection

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentBlobSelection(SwisTrackCore *stc):
		Component(stc, "BlobSelection"),
		mMinArea(0), mMaxArea(1000000), mOutputImage(0),
		mDisplayOutput("Output", "After dilation") {

	// Data structure relations
	mCategory=&(mCore->mCategoryPreprocessingBinary);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBlobSelection() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mMinArea=GetConfigurationInt("MinArea", 1);
	mMaxArea=GetConfigurationInt("MaxArea", 1000);
	mAreaSelection=GetConfigurationBool("AreaBool",false);
	mMinCompactness=GetConfigurationDouble("MinCompactness", 1);
	mMaxCompactness=GetConfigurationDouble("MaxCompactness", 1000);
	mCompactnessSelection=GetConfigurationBool("CompactnessBool",false);
	mMinOrientation=GetConfigurationDouble("MinOrientation", -90);
	mMaxOrientation=GetConfigurationDouble("MaxOrientation", 90);
	mOrientationSelection=GetConfigurationBool("OrientationBool",false);
}

void THISCLASS::OnStep() 
{
	// This is the image we want to treat
	IplImage* inputimage = mCore->mDataStructureImageBinary.mImage;
	if (!inputimage)
	{
		AddError("There is no input image");
	}

	// Prepare mOutputImage
	if (! mOutputImage) {
		mOutputImage=cvCreateImage(cvSize(inputimage->width,inputimage->height),inputimage->depth,1);
	}

	// Take a copy of the input image
	cvCopy(inputimage, mOutputImage);
	
	// If no computation is needed
	if ((mAreaSelection==false)&&(mCompactnessSelection==false)&&(mOrientationSelection==false)) {
		// Let the DisplayImage know about our image
		DisplayEditor de(&mDisplayOutput);
		if (de.IsActive()) {
				de.SetMainImage(mCore->mDataStructureImageBinary.mImage);
		}
		return;
	}

	// Initialization
	CvMoments moments; // Used to calculate the moments
	double contourArea;		//Used to select contour based on area
	double contourCompactness; //Used to select contour based on compactness
	double contourOrientation; //Used to select contour based on orientation
	bool removeBlobBool;		//Used to remove the blob from the image

	// We allocate memory to extract the contours from the binary image
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;

	// Init blob extraxtion
	CvContourScanner blobs = cvStartFindContours(inputimage,storage,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

	// This is used to correct the position in case of ROI
	CvRect rectROI;
	if (inputimage->roi != NULL) {
		rectROI = cvGetImageROI(inputimage);
	} else {
		rectROI.x = 0;
		rectROI.y = 0;
	}

	while ((contour=cvFindNextContour(blobs))!=NULL) {
		//A priori, we don't want to remove the blob
		removeBlobBool=false;
		// Calculating the moments
		cvMoments(contour, &moments);

		// Calculating particle area
		contourArea=moments.m00;

		// Selection based on area
		if (mAreaSelection) {
			if ((contourArea<mMinArea) || (contourArea>mMaxArea)) {
				removeBlobBool=true;
			}
		}
		// Selection based on compactness
		if (mCompactnessSelection) {
			double l = cvArcLength(contour, CV_WHOLE_SEQ, 1);
			contourCompactness=fabs(12.56*contourArea/(l*l));
			if ((contourCompactness<mMinCompactness) || (contourCompactness>mMaxCompactness)) {
				removeBlobBool=true;
			}
		}
		if (mOrientationSelection) {
			double tempValue=cvGetCentralMoment(&moments,2,0)-cvGetCentralMoment(&moments,0,2);
			contourOrientation=atan(2*cvGetCentralMoment(&moments,1,1)/(tempValue+sqrt(tempValue*tempValue+4*cvGetCentralMoment(&moments,1,1)*cvGetCentralMoment(&moments,1,1))));
			//Transforming in degrees (between -90 and 90
			contourOrientation=contourOrientation*57.29577951;
			if (!(((contourOrientation>mMinOrientation)&&(contourOrientation<mMaxOrientation))||((contourOrientation>mMinOrientation+180)&&(contourOrientation<mMaxOrientation+180))||((contourOrientation>mMinOrientation-180)&&(contourOrientation<mMaxOrientation-180)))) {
				removeBlobBool=true;
			}
		}
			
		//If we need to remove the blob, we paint it in black
		if (removeBlobBool) {
			//Paint the bad contour in black
			cvDrawContours(outputImage,contour,cvScalarAll(0),cvScalarAll(0),0,CV_FILLED);
		}

		cvRelease((void**)&contour);
	}
	contour = cvEndFindContours(&blobs);

	cvRelease((void**)&contour);
	cvReleaseMemStorage(&storage);
	mCore->mDataStructureImageBinary.mImage=outputImage;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageBinary.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
