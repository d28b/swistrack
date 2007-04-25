#include "ComponentHSVBackgroundSubtractionColor.h"
#define THISCLASS ComponentHSVBackgroundSubtractionColor

#include <highgui.h>
#include <sstream>

THISCLASS::ComponentHSVBackgroundSubtractionColor(SwisTrackCore *stc):
		Component(stc, "HSVBackgroundSubtractionColor"),
		mBackgroundImage(0), mCorrectMean(true),
		mDisplayImageOutput("Output", "After background subtraction") {

	// Data structure relations
	mDisplayName="Color Background Subtraction in the Hue Band";
	mCategory=&(mCore->mCategoryPreprocessing);
	AddDataStructureRead(&(mCore->mDataStructureImageBGR));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentHSVBackgroundSubtractionColor() {
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
	tmpHSVImage=cvCreateImage(cvGetSize(mBackgroundImage),8,3);
	mBackgroundImageHSV=cvCreateImage(cvGetSize(mBackgroundImage),8,1);
	tmpBinaryImage=cvCreateImage(cvGetSize(mBackgroundImage),8,1);
	//Convert image in HSV
	cvCvtColor(mBackgroundImage,tmpHSVImage,CV_BGR2HSV);
	cvSplit(tmpHSVImage,mBackgroundImageHSV,NULL,NULL,NULL);
}

void THISCLASS::OnReloadConfiguration() 
{
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
	if (! mCore->mDataStructureImageGray.mImage) 
	{
		//If the input image was not yet created
		mCore->mDataStructureImageGray.mImage=cvCreateImage(cvGetSize(inputImage),8,1);		
	}
	IplImage *outputImage=mCore->mDataStructureImageGray.mImage;
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
					}
		strcpy(mBackgroundImage->channelSeq,inputImage->channelSeq);
		for (int i=0; i<3;i++)
			cvReleaseImage(&(tmpImage[i]));
		//Convert image in HSV
		cvCvtColor(mBackgroundImage,tmpHSVImage,CV_BGR2HSV);
		cvSplit(tmpHSVImage,mBackgroundImageHSV,NULL,NULL,NULL);
	}
	
	try {
		cvCvtColor(inputImage,tmpHSVImage,CV_BGR2HSV);
		cvSplit(tmpHSVImage,outputImage,NULL,NULL,NULL);
		// Background Substraction
		cvAbsDiff(outputImage, mBackgroundImageHSV, outputImage);
		//Here we have to do something fancy, has in HSV 180 is close to 0
		cvThreshold(outputImage,tmpBinaryImage,90,255,CV_THRESH_BINARY);
		cvSubRS(outputImage,cvScalar(180),outputImage,tmpBinaryImage);
	} catch(...) {
		AddError("Background subtraction failed.");
	}
	
	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=outputImage;
	std::ostringstream oss;
	oss << "After background subtraction, " << inputImage->width << "x" << inputImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() 
{
	if (mBackgroundImage) {cvReleaseImage(&mBackgroundImage);}
	if (tmpHSVImage) {cvReleaseImage(&tmpHSVImage);}
	if (tmpBinaryImage) {cvReleaseImage(&tmpBinaryImage);}
	if (mCore->mDataStructureImageGray.mImage) {cvReleaseImage(&(mCore->mDataStructureImageGray.mImage));}	
}
