#include "ComponentBackgroundSubtractionColor.h"
#define THISCLASS ComponentBackgroundSubtractionColor

#include <highgui.h>
#include "DisplayEditor.h"

THISCLASS::ComponentBackgroundSubtractionColor(SwisTrackCore *stc):
		Component(stc, wxT("BackgroundSubtractionColor")),
		mBackgroundImage(0), mBackgroundImageMean(cvScalarAll(0)), mCorrectMean(true),
		mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBackgroundSubtractionColor() {
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

	// load other parameters:
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration()
{
	// Whether to correct the mean or not
	mCorrectMean = GetConfigurationBool(wxT("CorrectMean"), true);
	// We always calculate the background average, so we can select if we use the moving threshold during the segmentation
	if (mCorrectMean) {
		mBackgroundImageMean = cvAvg(mBackgroundImage);
	} else {
		mBackgroundImageMean = cvScalarAll(0);
	}
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
		CvScalar tmpBackgroundMean = mBackgroundImageMean;
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
						mBackgroundImageMean.val[i] = tmpBackgroundMean.val[j];
					}
		strcpy(mBackgroundImage->channelSeq, inputimage->channelSeq);
		for (int i = 0; i < 3;i++)
			cvReleaseImage(&(tmpImage[i]));
	}

	try {
		// Correct the tmpImage with the difference in image mean
		if (mCorrectMean)
		{
			CvScalar tmpScalar = cvAvg(inputimage);
			cvAddS(inputimage, cvScalar(mBackgroundImageMean.val[0] - tmpScalar.val[0], mBackgroundImageMean.val[1] - tmpScalar.val[1], mBackgroundImageMean.val[2] - tmpScalar.val[2]), inputimage);
		}

		// Background Substraction
		cvAbsDiff(inputimage, mBackgroundImage, inputimage);
	} catch (...) {
		AddError(wxT("Background subtraction failed."));
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(inputimage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	if (mBackgroundImage) {
		cvReleaseImage(&mBackgroundImage);
	}
}
