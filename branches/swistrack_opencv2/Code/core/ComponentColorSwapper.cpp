#include "ComponentColorSwapper.h"
#define THISCLASS ComponentColorSwapper

#include "DisplayEditor.h"
#include "Utility.h"
#include <limits>
#include <iostream>
#include <set>

using namespace std;

THISCLASS::ComponentColorSwapper(SwisTrackCore *stc):
		Component(stc, wxT("ColorSwapper")),
		mOutputImage(NULL),
		mDisplayOutput(wxT("Output"), wxT("Color Swapper"))

{
  // Data structure relations
  mCategory = &(mCore->mCategoryPreprocessingColor);
  AddDataStructureRead(&(mCore->mDataStructureImageColor));
  AddDataStructureWrite(&(mCore->mDataStructureImageColor));   
  AddDisplay(&mDisplayOutput);
  Initialize();
}

THISCLASS::~ComponentColorSwapper()
{

}

void THISCLASS::OnStart()
{
  THISCLASS::OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration()
{
  mSourceColor = GetConfigurationColor(wxT("SourceColor"), *wxWHITE);
  mTargetColor = GetConfigurationColor(wxT("TargetColor"), *wxBLACK);
  
  cout << "source: " << mSourceColor.GetAsString(wxC2S_HTML_SYNTAX).ToAscii() << endl;
  cout << "target: " << mTargetColor.GetAsString(wxC2S_HTML_SYNTAX).ToAscii() << endl;
}

void THISCLASS::OnStep()
{
  IplImage *inputImage = mCore->mDataStructureImageColor.mImage;
  if (! inputImage) {
    AddError(wxT("No input image."));
    return;
  }
  if (mOutputImage == NULL) {
    mOutputImage = cvCloneImage(inputImage);
  }
  cvCopy(inputImage, mOutputImage);

  for (int y = 0; y < mOutputImage->height; y++) {
    uchar * outptr = (uchar *) (mOutputImage->imageData + y * mOutputImage->widthStep);
    uchar * inptr = (uchar *) (inputImage->imageData + y * mOutputImage->widthStep);
    for (int x = 0; x < mOutputImage->width; x++) { 

      uchar b = inptr[3*x + 0];
      uchar g = inptr[3*x + 1];
      uchar r = inptr[3*x + 2];
      //printf("r: %d\n", r);
      //printf("b: %d\n", b);
      //printf("g: %d\n", g);

      //printf("r: %d\n", mSourceColor.Red());
      //printf("b: %d\n", mSourceColor.Green());
      //printf("g: %d\n", mSourceColor.Blue());

      if (r == mSourceColor.Red() && 
	  g == mSourceColor.Green() &&
	  b == mSourceColor.Blue()) {
 	//cout << "Swapping." << endl;
	
	//printf("r: %d\n", mTargetColor.Red());
	//printf("b: %d\n", mTargetColor.Green());
	//printf("g: %d\n", mTargetColor.Blue());
	outptr[3*x + 0] = mTargetColor.Blue();
	outptr[3*x + 1] = mTargetColor.Green();
	outptr[3*x + 2] = mTargetColor.Red();
      }
    }
  }

  mCore->mDataStructureImageColor.mImage  = mOutputImage; 
  DisplayEditor de(&mDisplayOutput);
  if (de.IsActive()) {
    de.SetMainImage(mOutputImage);
  }
}

void THISCLASS::OnStepCleanup() {
  
}

void THISCLASS::OnStop() {
  cvReleaseImage(&mOutputImage);
}


