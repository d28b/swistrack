#include "ComponentBlobSelection.h"
#define THISCLASS ComponentBlobSelection

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentBlobSelection(SwisTrackCore *stc):
		Component(stc, "BlobSelection"),
		mMinArea(0), mMaxArea(1000000),outputImage(0),
		mDisplayOutput("Output", "After dilation") {

	// Data structure relations
	mCategory=&(mCore->mCategoryBinaryPreprocessing);
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
}

void THISCLASS::OnStep() 
{
	// This is the image we want to treat
	IplImage* inputImage = mCore->mDataStructureImageBinary.mImage;
	if (!inputImage)
	{
		AddError("There is no input image");
	}

	//Prepare outputImage
	if (!outputImage)
		outputImage=cvCreateImage(cvSize(inputImage->width,inputImage->height),inputImage->depth,1);
	//copy the 
	cvCopy(inputImage,outputImage);
	
	//If no computation is needed
	if ((mAreaSelection==false)&&(mCompactnessSelection==false))
	{
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
	bool removeBlobBool;		//Used to remove the blob from the image

	// We allocate memory to extract the contours from the binary image
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;

	// Init blob extraxtion
	CvContourScanner blobs = cvStartFindContours(inputImage,storage,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

	// This is used to correct the position in case of ROI
	CvRect rectROI;
	if(inputImage->roi != NULL) {
		rectROI = cvGetImageROI(inputImage);
	} else {
		rectROI.x = 0;
		rectROI.y = 0;
	}

	while ((contour=cvFindNextContour(blobs))!=NULL) 
	{
		//A priori, we don't want to remove the blob
		removeBlobBool=false;
		// Calculating the moments
		cvMoments(contour, &moments);

		// Calculating particle area
		contourArea=moments.m00;

		// Selection based on area
		if (mAreaSelection)
		{
			if ((contourArea<mMinArea) || (contourArea>mMaxArea))
			{
				removeBlobBool=true;
			}
		}
		// Selection based on compactness
		if (mCompactnessSelection)
		{
			double l = cvArcLength(contour, CV_WHOLE_SEQ, 1);
			contourCompactness=fabs(12.56*contourArea/(l*l));
			if ((contourCompactness<mMinCompactness) || (contourCompactness>mMaxCompactness))
			{
				removeBlobBool=true;
			}
		}
		//If we need to remove the blob, we paint it in black
		if (removeBlobBool)
		{
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
