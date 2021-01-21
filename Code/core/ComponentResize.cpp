#include "ComponentResize.h"
#define THISCLASS ComponentResize

#include <highgui.h>
#include "DisplayEditor.h"

using namespace cv;
using namespace std;

THISCLASS::ComponentResize(SwisTrackCore *stc):
		Component(stc, wxT("Resize")),
		mDisplayOutput(wxT("Output"), wxT("Resized")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);
	
	mDisplayOutputImage=NULL;	
	// Read the XML configuration file
	Initialize();
	
}

THISCLASS::~ComponentResize() {
}

void THISCLASS::OnStart() {

	// Load other parameters
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	mConfigurationChanged=true;
}

void THISCLASS::OnStep() {
	IplImage *inputimage = mCore->mDataStructureImageGray.mImage;
	if (! inputimage)
	{
	  AddError(wxT("Cannot access input image."));
	  return;
	}
	
	
	// update output data structure - note that this is using the new cv::Mat data structure from OpenCV2, as it is much easier to use. 
    cv::Mat Im=inputimage;

	if (mConfigurationChanged) {
		ROI_width=Im.cols;
		ROI_height=Im.rows;
		ROI_x=GetConfigurationDouble(wxT("CropLeft"), true);
		ROI_y=GetConfigurationDouble(wxT("CropTop"), true);
		
		if ((int)GetConfigurationDouble(wxT("width"), true)>0) {
			
			ROI_width=min((int)GetConfigurationDouble(wxT("width"), true), (int)Im.cols-ROI_x);
		} else {
			ROI_width=min(max(1, (int)(Im.cols+GetConfigurationDouble(wxT("width"), true))), (int)Im.cols-ROI_x);
		}

		if ((int)GetConfigurationDouble(wxT("height"), true)>0) {
			ROI_height=min((int)GetConfigurationDouble(wxT("height"), true), (int)Im.rows-ROI_y);
		} else {
			ROI_height=min(max(1, (int)(Im.rows+GetConfigurationDouble(wxT("height"), true))), (int)Im.rows-ROI_y);
		}
		mConfigurationChanged=false;
	}
	cv::Rect roi(Point( ROI_x,ROI_y), Size( ROI_width, ROI_height));
	cv::Mat out = Im(roi);

		
	if (mDisplayOutputImage == NULL) {
		mDisplayOutputImage = cvCreateImage(cvSize(0,0), 8, 1);
		  
	}

		
	*mDisplayOutputImage=out;
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mDisplayOutputImage);
	}
	mCore->mDataStructureImageGray.mImage=mDisplayOutputImage;
	
	
	
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	//cvReleaseImage(&mDisplayOutputImage);
}

