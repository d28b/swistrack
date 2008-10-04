#include "ComponentSpecificColorSubtraction.h"
#define THISCLASS ComponentSpecificColorSubtraction

#include <highgui.h>
#include "DisplayEditor.h"

/*
//Function to get the position of the mouse on the image
int cmx, cmy, cmevent;
void UpdateMouse(int event, int x, int y, int flags,void* param)
	{
		if (event==CV_EVENT_LBUTTONDOWN)
		{
			cmx=x;
			cmy=y;
		}
	}
*/

THISCLASS::ComponentSpecificColorSubtraction(SwisTrackCore *stc):
		Component(stc, wxT("SpecificColorSubtraction")),
		mCorrectMean(true), mSpecifyColorBool(true),
		mTrackedColor(cvScalar(0)), mSpecifiedImageAverage(cvScalarAll(0)),
		mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentSpecificColorSubtraction() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration()
{
	// Whether to correct the mean or not
	mCorrectMean = GetConfigurationBool(wxT("CorrectMean"), true);


	//Whether to use an image to specify the color or not
	mSpecifyColorBool = GetConfigurationBool(wxT("SpecifyColorBool"), true);


	//Get the specified color from the interface
	mTrackedColor.val[0] = GetConfigurationInt(wxT("BlueChannel"), true);
	mTrackedColor.val[1] = GetConfigurationInt(wxT("GreenChannel"), true);
	mTrackedColor.val[2] = GetConfigurationInt(wxT("RedChannel"), true);
	strcpy(trackedColorSeq, "BGR");

	if (mCorrectMean)
		mSpecifiedImageAverage = cvScalarAll(-1);
	/*
	if (mSpecifyColorBool)
		mTrackedColor=cvScalarAll(-1);
		*/
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
	if ((mCorrectMean) && (mSpecifiedImageAverage.val[0] == -1))
		mSpecifiedImageAverage = cvAvg(inputimage);
	/*
	//If we want to specify the color on the input image and it has not been done at this time
	if ((mSpecifyColorBool)&&(mTrackedColor.val[0]==-1))
	{
		
		cvNamedWindow("Click on the desired color in the image");
		cvShowImage(wxT("Click on the desired color in the image"),inputimage);
		cmx=-1;
		cvSetMouseCallback(wxT("Click on the desired color in the image"), UpdateMouse);
		while(cmx<0)
		{
			cvWaitKey();
		}
		cvDestroyWindow("Click on the desired color in the image");
		mTrackedColor=cvGet2D(inputimage,cmy,cmx);//matrix reading, we define first the row (height/y) and then the column (width/x)
		mSpecifiedImageAverage=cvAvg(inputimage);
		
		mTrackedColor=cvScalar(100,100,100);
		mSpecifiedImageAverage=cvAvg(inputimage);
	}
	*/
	try {
		// Correct the channel sequence
		if (strncmp(mCore->mDataStructureImageColor.mImage->channelSeq, trackedColorSeq, 3))
		{
			CvScalar tmpColor;
			for (int i = 0;i < 3;i++)
				for (int j = 0;j < 3;j++)
					if (inputimage->channelSeq[i] == trackedColorSeq[j])
					{
						tmpColor.val[i] = mTrackedColor.val[j];
					}
			strcpy(trackedColorSeq, inputimage->channelSeq);
			for (int i = 0;i < 3;i++)
				mTrackedColor.val[i] = tmpColor.val[i];
		}

		// Correct the tmpImage with the difference in image mean
		if (mCorrectMean)
		{
			CvScalar tmpScalar = cvAvg(inputimage);
			cvAbsDiffS(inputimage, inputimage, cvScalar(mTrackedColor.val[0] + mSpecifiedImageAverage.val[0] - tmpScalar.val[0], mTrackedColor.val[1] + mSpecifiedImageAverage.val[1] - tmpScalar.val[1], mTrackedColor.val[2] + mSpecifiedImageAverage.val[2] - tmpScalar.val[2]));
		}
		else
		{
			cvAbsDiffS(inputimage, inputimage, mTrackedColor);
		}

	} catch (...) {
		AddError(wxT("Substracting specifique color failed"));
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
}


