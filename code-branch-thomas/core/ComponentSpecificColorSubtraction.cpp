#include "ComponentSpecificColorSubtraction.h"
#define THISCLASS ComponentSpecificColorSubtraction

#include <highgui.h>
#include <sstream>
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
		Component(stc, "SpecificColorSubtraction"),
		mSpecifiedImageAverage(cvScalarAll(0)), mSpecifyColorBool(true), mCorrectMean(true), mTrackedColor(cvScalar(0)),
		mDisplayImageOutput("Output", "After background subtraction") {

	// Data structure relations
	mDisplayName="Specific color subtraction";
	mCategory=&(mCore->mCategoryPreprocessing);
	AddDataStructureRead(&(mCore->mDataStructureImageBGR));
	AddDataStructureWrite(&(mCore->mDataStructureImageBGR));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentSpecificColorSubtraction() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() 
{
	// Whether to correct the mean or not
	mCorrectMean=GetConfigurationBool("CorrectMean", true);
	
	
	//Whether to use an image to specify the color or not
	mSpecifyColorBool=GetConfigurationBool("SpecifyColorBool",true);
	

	//Get the specified color from the interface
	mTrackedColor.val[0]=GetConfigurationInt("BlueChannel",true);
	mTrackedColor.val[1]=GetConfigurationInt("GreenChannel",true);
	mTrackedColor.val[2]=GetConfigurationInt("RedChannel",true);

	if (mCorrectMean)
		mSpecifiedImageAverage=cvScalarAll(-1);
	/*
	if (mSpecifyColorBool)
		mTrackedColor=cvScalarAll(-1);
		*/
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
	if ((mCorrectMean)&&(mSpecifiedImageAverage.val[0]==-1))
		mSpecifiedImageAverage=cvAvg(inputImage);
	
	/*
	//If we want to specify the color on the input image and it has not been done at this time
	if ((mSpecifyColorBool)&&(mTrackedColor.val[0]==-1))
	{
		
		cvNamedWindow("Click on the desired color in the image");
		cvShowImage("Click on the desired color in the image",inputImage);
		cmx=-1;
		cvSetMouseCallback("Click on the desired color in the image", UpdateMouse);
		while(cmx<0)
		{
			cvWaitKey();
		}
		cvDestroyWindow("Click on the desired color in the image");
		mTrackedColor=cvGet2D(inputImage,cmy,cmx);//matrix reading, we define first the row (height/y) and then the column (width/x)
		mSpecifiedImageAverage=cvAvg(inputImage);
		
		mTrackedColor=cvScalar(100,100,100);
		mSpecifiedImageAverage=cvAvg(inputImage);
	}
	*/
	try {
		// Correct the tmpImage with the difference in image mean
		if (!strncmp(mCore->mDataStructureImageBGR.mImage->channelSeq,"BGR",3))
		{
			if (mCorrectMean)
			{			
				CvScalar tmpScalar=cvAvg(inputImage);
				cvAbsDiffS(inputImage,inputImage,cvScalar(mTrackedColor.val[0]+mSpecifiedImageAverage.val[0]-tmpScalar.val[0],mTrackedColor.val[1]+mSpecifiedImageAverage.val[1]-tmpScalar.val[1],mTrackedColor.val[2]+mSpecifiedImageAverage.val[2]-tmpScalar.val[2]));
			}
			else
			{
				cvAbsDiffS(inputImage,inputImage,mTrackedColor);
			}
		}
	} catch(...) {
		AddError("Substracting specifique color failed");
	}

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=inputImage;
	std::ostringstream oss;
	oss << "After background subtraction, " << inputImage->width << "x" << inputImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}


