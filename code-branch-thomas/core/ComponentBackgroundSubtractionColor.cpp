#include "ComponentBackgroundSubtractionColor.h"
#define THISCLASS ComponentBackgroundSubtractionColor

#include <highgui.h>
#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentBackgroundSubtractionColor(SwisTrackCore *stc):
		Component(stc, "BackgroundSubtractionColor"),
		mBackgroundImageMean(cvScalarAll(0)), mBackgroundImage(0), mCorrectMean(true),
		mDisplayOutput("Output", "After background subtraction") {

	// Data structure relations
	mCategory=&(mCore->mCategoryPreprocessing);
	AddDataStructureRead(&(mCore->mDataStructureImageBGR));
	AddDataStructureWrite(&(mCore->mDataStructureImageBGR));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBackgroundSubtractionColor() {
}

void THISCLASS::OnStart() {
	std::string filename=GetConfigurationString("BackgroundImage", "");
	if (filename!="") {
		mBackgroundImage=cvLoadImage(filename.c_str(),CV_LOAD_IMAGE_UNCHANGED);
	}
	if (! mBackgroundImage) {
		AddError("Cannot open background image.");
		return;
	}

	if (mBackgroundImage->nChannels!=3) 
	{
		AddError("Background Image has not 3 channels");
		return;
	}

	// Whether to correct the mean or not
	mCorrectMean=GetConfigurationBool("CorrectMean", true);

	// We always calculate the background average, so we can select if we use the moving threshold during the segmentation
	if (mCorrectMean) {
		mBackgroundImageMean=cvAvg(mBackgroundImage);
	} else {
		mBackgroundImageMean=cvScalarAll(0);
	}
}

void THISCLASS::OnReloadConfiguration() 
{
	// Whether to correct the mean or not
	mCorrectMean=GetConfigurationBool("CorrectMean", true);
	// We always calculate the background average, so we can select if we use the moving threshold during the segmentation
	if (mCorrectMean) {
		mBackgroundImageMean=cvAvg(mBackgroundImage);
	} else {
		mBackgroundImageMean=cvScalarAll(0);
	}
}

void THISCLASS::OnStep() {
	IplImage *inputImage=mCore->mDataStructureImageBGR.mImage;	
	//Check the images
	if (! inputImage) 
	{
		AddError("No input Image");
		return;
	}
	if (inputImage->nChannels !=3)
	{
		AddError("Input image has not 3 channels.");
		return;
	}
	if (! mBackgroundImage) 
	{
		AddError("Background image not accessible");
		return;
	}
	if ((cvGetSize(inputImage).height!=cvGetSize(mBackgroundImage).height)||(cvGetSize(inputImage).width!=cvGetSize(mBackgroundImage).width))
	{
		AddError("Input and background images have not the same dimension");
		return;
	}

	//Check for the color system of the input image (The loaded image is BGR, OpenCV default) and convert the background respectively
	if (strncmp(mCore->mDataStructureImageBGR.mImage->channelSeq,mBackgroundImage->channelSeq,3))
	{	
		//Make a temporary clone of the image in 3 seperate channels
		IplImage* tmpImage[3];
		for (int i=0;i<3;i++)
			tmpImage[i]=cvCreateImage(cvGetSize(mBackgroundImage),8,1);
		cvSplit(mBackgroundImage,tmpImage[0],tmpImage[1],tmpImage[2],NULL);
		CvScalar tmpBackgroundMean=mBackgroundImageMean;
		//Modify the sequence of the channels in the background		
		for (int i=0;i<3;i++)
			//If the channel is not the same, search for the corresponding channel to copy, else copy the channel directly
			if (inputImage->channelSeq[i]!=mBackgroundImage->channelSeq[i])
				for (int j=0;j<3;j++)
					if (inputImage->channelSeq[i]==mBackgroundImage->channelSeq[j])
					{						
						cvSetImageCOI(mBackgroundImage,i+1);						
						cvCopy(tmpImage[j],mBackgroundImage);
						//Remove the COI						
						cvSetImageCOI(mBackgroundImage,0);												
						mBackgroundImageMean.val[i]=tmpBackgroundMean.val[j];
					}
		strcpy(mBackgroundImage->channelSeq,inputImage->channelSeq);
		for (int i=0; i<3;i++)
			cvReleaseImage(&(tmpImage[i]));
	}
	
	try {
		// Correct the tmpImage with the difference in image mean		
		if (mCorrectMean) 
		{			
			CvScalar tmpScalar=cvAvg(inputImage);			
			cvAddS(inputImage, cvScalar(mBackgroundImageMean.val[0]-tmpScalar.val[0],mBackgroundImageMean.val[1]-tmpScalar.val[1],mBackgroundImageMean.val[2]-tmpScalar.val[2]),inputImage);
		}

		// Background Substraction
		cvAbsDiff(inputImage, mBackgroundImage, inputImage);
	} catch(...) {
		AddError("Background subtraction failed.");
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(inputImage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	if (mBackgroundImage) {cvReleaseImage(&mBackgroundImage);}
}
