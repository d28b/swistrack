#include "ComponentAdaptiveBackgroundSubtractionMode.h"
#define THISCLASS ComponentAdaptiveBackgroundSubtractionMode

#include <highgui.h>
#include "DisplayEditor.h"

const int EPSILON = 8;

THISCLASS::ComponentAdaptiveBackgroundSubtractionMode(SwisTrackCore *stc):
  Component(stc, wxT("AdaptiveBackgroundSubtractionMode")),
  mCounter(), mBackgroundModel(0), mOutputImage(0), 
  mDisplayOutput(wxT("Output"), wxT("After background subtruction"))
{
     mCategory = &(mCore->mCategoryPreprocessingColor);
     AddDataStructureRead(&(mCore->mDataStructureImageColor));
     AddDataStructureWrite(&(mCore->mDataStructureImageColor));
     AddDisplay(&mDisplayOutput);

     Initialize();
}


THISCLASS::~ComponentAdaptiveBackgroundSubtractionMode()
{
}


void THISCLASS::OnStart()
{
  OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration()
{
}

void THISCLASS::OnStep()
{
  IplImage *inputImage = mCore->mDataStructureImageColor.mImage;
  if (! inputImage)
    {
      AddError(wxT("No input image."));
      return ;
    }
  if (inputImage->nChannels != 3)
    {
      AddError(wxT("The input image is not a color image."));
      return ;
    }
  if (!mBackgroundModel)
    {
      mBackgroundModel = cvCloneImage(inputImage);
      mCounter.resize(mBackgroundModel->height);
      for (int i = 0; i < (int)mCounter.size();i++)
	mCounter[i].resize(mBackgroundModel->width);
    }
  if (!mOutputImage)
    {
      mOutputImage = cvCloneImage(inputImage);
    }
  UpdateBackgroundModel(inputImage);
  
  mCore->mDataStructureImageColor.mImage = mOutputImage;
  
  DisplayEditor de(&mDisplayOutput);
  if (de.IsActive())
    {
      de.SetMainImage(mOutputImage);
    }
}


void THISCLASS::UpdateBackgroundModel(IplImage * inputImage)
{
 
  for (int y = 0; y < mBackgroundModel->height; y++)
    {
      uchar * bgtr = (uchar *) (mBackgroundModel->imageData +
				y*mBackgroundModel->widthStep);
      uchar * inputr  = (uchar *) (inputImage->imageData + 
				 y*mBackgroundModel->widthStep);
    
      for (int x = 0; x < mBackgroundModel->width; x++)
	{
	  bool flag = TRUE, debug = TRUE;
          int rmax = -1, rmin = -1;
          int i;
	  for (i=0;i < MAX_BUFFER_SIZE && mCounter[y][x].a[i].count;i++)
	    {
		bool f = TRUE;
		for (int j=0;j<3;j++) 
		   if (abs(mCounter[y][x].a[i].color[j] - inputr[x*3 + j]) > EPSILON ) {f = FALSE; break;}
		if (f) {mCounter[y][x].a[i].count++; debug = FALSE;}
		if (rmin == -1 || mCounter[y][x].a[i].count<mCounter[y][x].a[rmin].count) rmin = i;
		if (rmax == -1 || mCounter[y][x].a[i].count>mCounter[y][x].a[rmax].count) rmax = i;
	    }
	  if (flag)
	    {
	       if (rmin == -1) rmin = 0;
	       if (i < MAX_BUFFER_SIZE) rmin = i;
	       if (rmax == -1) rmax = 0;	
	       mCounter[y][x].a[rmin].count = 1;
	       for (int j=0;j<3;j++) mCounter[y][x].a[rmin].color[j] = inputr[3*x+j];
	    }
	  for (int j=0;j<3;j++) bgtr[3*x + j] = (uchar) mCounter[y][x].a[rmax].color[j];	
/*          if (debug) for (int j=0;j<3;j++) bgtr[3*x + j] = (uchar) 225;//mCounter[y][x].a[rmax].color[j];	      
          else for (int j=0;j<3;j++) bgtr[3*x + j] = (uchar) mCounter[y][x].a[rmax].color[j];	      */
	}
    }
//  mOutputImage = mBackgroundModel;
  cvAbsDiff(inputImage, mBackgroundModel, mOutputImage);
}

void THISCLASS::OnStepCleanup()
{
}

void THISCLASS::OnStop()
{
}
