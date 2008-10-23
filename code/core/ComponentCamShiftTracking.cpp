#include "ComponentCamShiftTracking.h"
#define THISCLASS ComponentCamShiftTracking
using namespace std;
#include <iostream>

#include "DisplayEditor.h"

THISCLASS::ComponentCamShiftTracking(SwisTrackCore *stc):
		Component(stc, wxT("CamShiftTracking")),
		mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("Tracking"))
{
	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	Initialize();						// Read the XML configuration file
}

THISCLASS::~ComponentCamShiftTracking()
{

}

void THISCLASS::OnStart()
{
  

  setVmin(&cs, GetConfigurationInt(wxT("VMin"), 60));
  setSmin(&cs, GetConfigurationInt(wxT("SMin"), 50));
  //mTracker.set_window(cvRect(452, 493, 75, 110));
  //int hist_sizes[] = {30, 32, 50};
  //mTracker.set_hist_dims(1, hist_sizes);
  //mTracker.set_hist_bin_range(0, 0, 180);
}

void THISCLASS::OnReloadConfiguration()
{
  OnStop();
}

void THISCLASS::OnStep()
{	
  IplImage *inputImage = mCore->mDataStructureImageColor.mImage;
  if (! inputImage) {
    AddError(wxT("No input image."));
    return;
  }

  if (mOutputImage == 0) {
    //CvRect start = cvRect(452, 493, 75, 110);
    CvRect start = cvRect(452, 493, 20, 20);
    createTracker(&cs, inputImage);
    startTracking(&cs, inputImage, &start);
  }

  CvBox2D box = track(&cs, inputImage);
  /*
  if (!mTracker.track_object(inputImage)) {
    AddError(wxT("Couldn't track."));
  }
  if (!mTracker.update_histogram(inputImage)) {
    AddError(wxT("Couldn't update histogram."));
    }*/

  // update the tracks store locally to this component
  //mCore->mDataStructureTracks.mTracks = &mTracks;

  // Let the DisplayImage know about our image
  DisplayEditor de(&mDisplayOutput);
  if (de.IsActive()) {
    //de.SetParticles(particles);	//TODO: what was this?
    //cvRectangle(mOutputImage, mTracker.get_window());
    //cvCopy(inputImage, mOutputImage);
    if (mOutputImage != NULL) {
      cvReleaseImage(&mOutputImage);
    }
    mOutputImage = cvCloneImage(inputImage);
    //mOutputImage = cvCloneImage(mTracker.get_back_project());
    //cvCopy(mTracker.get_back_project(), mOutputImage);
    cvEllipseBox(mOutputImage, box,
		 CV_RGB(255,0,0), 3, CV_AA, 0 );

    /*cvRectangle(mOutputImage, 
		cvPoint(box.rect.x, box.rect.y),
		cvPoint(box.rect.x + box.rect.width,
			box.rect.y + box.rect.height),
			cvScalar(255,255,255));*/
		
    
    de.SetMainImage(mOutputImage);
  }
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
   releaseTracker(&cs);
   cvReleaseImage(&mOutputImage);
   mOutputImage = 0;
}

