#include "ComponentHSVBackgroundSubtractionColor.h"
#define THISCLASS ComponentHSVBackgroundSubtractionColor

#include <highgui.h>
#include "DisplayEditor.h"

THISCLASS::ComponentHSVBackgroundSubtractionColor(SwisTrackCore *stc):
		Component(stc, wxT("HSVBackgroundSubtractionColor")),
		mBackgroundImage(0), mCorrectMean(true),
		mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentHSVBackgroundSubtractionColor() {
}

void THISCLASS::OnStart() {
	wxString filename_string = GetConfigurationString(wxT("BackgroundImage"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filename_string);
	if (filename.IsOk()) {
		mBackgroundImage = cvLoadImage(filename.GetFullPath().mb_str(wxConvFile), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	}
	if (! mBackgroundImage) {
		AddError(wxT("Cannot open background image."));
		return;
	}

	if (mBackgroundImage->nChannels != 3)
	{
		AddError(wxT("Background Image has not 3 channels"));
		return;
	}
	tmpHSVImage = cvCreateImage(cvGetSize(mBackgroundImage), 8, 3);
	mBackgroundImageHSV = cvCreateImage(cvGetSize(mBackgroundImage), 8, 1);
	tmpBinaryImage = cvCreateImage(cvGetSize(mBackgroundImage), 8, 1);
	//Convert image in HSV
	cvCvtColor(mBackgroundImage, tmpHSVImage, CV_BGR2HSV);
	cvSplit(tmpHSVImage, mBackgroundImageHSV, NULL, NULL, NULL);
	outputImage = NULL;
}

void THISCLASS::OnReloadConfiguration()
{
}

void THISCLASS::OnStep() {
	IplImage *inputimage = mCore->mDataStructureImageColor.mImage;
	//Check the images
	if (! inputimage)
	{
		AddError(wxT("No input Image"));
		return;
	}
	if (inputimage->nChannels != 3)
	{
		AddError(wxT("Input image has not 3 channels."));
		return;
	}
	if (!outputImage)
	{
		//If the input image was not yet created
		outputImage = cvCreateImage(cvGetSize(inputimage), 8, 1);
	}
	mCore->mDataStructureImageGray.mImage = outputImage;
	if (! mBackgroundImage)
	{
		AddError(wxT("Background image not accessible"));
		return;
	}
	if ((cvGetSize(inputimage).height != cvGetSize(mBackgroundImage).height) || (cvGetSize(inputimage).width != cvGetSize(mBackgroundImage).width))
	{
		AddError(wxT("Input and background images have not the same dimension"));
		return;
	}

	//Check for the color system of the input image (The loaded image is BGR, OpenCV default) and convert the background respectively
	if (strncmp(mCore->mDataStructureImageColor.mImage->channelSeq, mBackgroundImage->channelSeq, 3))
	{
		//Make a temporary clone of the image in 3 seperate channels
		IplImage* tmpImage[3];
		for (int i = 0;i < 3;i++)
			tmpImage[i] = cvCreateImage(cvGetSize(mBackgroundImage), 8, 1);
		cvSplit(mBackgroundImage, tmpImage[0], tmpImage[1], tmpImage[2], NULL);
		//Modify the sequence of the channels in the background
		for (int i = 0;i < 3;i++)
			//If the channel is not the same, search for the corresponding channel to copy, else copy the channel directly
			if (inputimage->channelSeq[i] != mBackgroundImage->channelSeq[i])
				for (int j = 0;j < 3;j++)
					if (inputimage->channelSeq[i] == mBackgroundImage->channelSeq[j])
					{
						cvSetImageCOI(mBackgroundImage, i + 1);
						cvCopy(tmpImage[j], mBackgroundImage);
						//Remove the COI
						cvSetImageCOI(mBackgroundImage, 0);
					}
		strcpy(mBackgroundImage->channelSeq, inputimage->channelSeq);
		for (int i = 0; i < 3;i++)
			cvReleaseImage(&(tmpImage[i]));
		//Convert image in HSV
		cvCvtColor(mBackgroundImage, tmpHSVImage, CV_BGR2HSV);
		cvSplit(tmpHSVImage, mBackgroundImageHSV, NULL, NULL, NULL);
	}

	try {
		cvCvtColor(inputimage, tmpHSVImage, CV_BGR2HSV);
		cvSplit(tmpHSVImage, outputImage, NULL, NULL, NULL);
		// Background Substraction
		cvAbsDiff(outputImage, mBackgroundImageHSV, outputImage);
		//Here we have to do something fancy, has in HSV 180 is close to 0
		cvThreshold(outputImage, tmpBinaryImage, 90, 255, CV_THRESH_BINARY);
		cvSubRS(outputImage, cvScalar(180), outputImage, tmpBinaryImage);
	} catch (...) {
		AddError(wxT("Background subtraction failed."));
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(outputImage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop()
{
	if (mBackgroundImage) {
		cvReleaseImage(&mBackgroundImage);
	}
	if (tmpHSVImage) {
		cvReleaseImage(&tmpHSVImage);
	}
	if (tmpBinaryImage) {
		cvReleaseImage(&tmpBinaryImage);
	}
	if (outputImage) {
		cvReleaseImage(&(outputImage));
	}
}
