#include "ComponentBackgroundSubtractionCheungKamath.h"
#define THISCLASS ComponentBackgroundSubtractionCheungKamath

#include <highgui.h>
#include "DisplayEditor.h"
#include <Blob.h>
#include <BlobResult.h>
using namespace std;
#include <iostream>

CvSeq * maskToSequence(IplImage * mask, CvMemStorage*storage) {

  CvSeq * seq = cvCreateSeq(CV_SEQ_KIND_GENERIC|CV_SEQ_ELTYPE_POINT, sizeof(CvSeq),
			    sizeof(CvPoint), storage);
  
  CvPoint p;
  for (int y = 0; y < mask->height; y++) {
    uchar * ptr = (uchar *) (mask->imageData + y * mask->widthStep);
    for (int x = 0; x < mask->width; x++) {
      int value = ptr[x];
      if (value == 255) {

	p.x = x;
	p.y = y;
	cvSeqPush(seq, &p);
      } else if (value != 0) {
	cout << "Unexpected value " << value << " in mask." << endl;
      }
      }
  }
  return seq;
}

class CBlobIntersectMask : public COperadorBlob
{
  IplImage * mMask;
  IplImage * mTmp;
public:
  CBlobIntersectMask(IplImage * mask, IplImage * tmp) {
    mMask = mask;
    mTmp = tmp;
  }
  double operator()(const CBlob &blob) const {
    cvZero(mTmp);
    blob.FillBlob(mTmp, cvRealScalar(255));
    cvAnd(mTmp, mMask, mTmp);
    int numNonZero = cvCountNonZero(mTmp);
    return numNonZero;
    
  }
  const char *GetNom() const
  {
    return "CBlobIntersectMask";
  }
};

THISCLASS::ComponentBackgroundSubtractionCheungKamath(SwisTrackCore *stc):
		Component(stc, wxT("BackgroundSubtractionCheungKamath")),
		mOutputImage(0), mShortTermForegroundMask(0),  mTmp(0),  mR(0), mB(0), mG(0), mTmpColors(0), mTmpBinary(0), mLastFrame(0),
		
		mDisplayOutput(wxT("Output"), wxT("After background subtraction")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageBinary));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();

	mStorage = cvCreateMemStorage(0);
}

THISCLASS::~ComponentBackgroundSubtractionCheungKamath() {
  cvReleaseMemStorage(&mStorage);
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();


	if (mOutputImage != NULL) {
	  cvReleaseImage(&mOutputImage);
	}
	mOutputImage = NULL;
	
}

void THISCLASS::OnReloadConfiguration() {
	mThreshold = GetConfigurationInt(wxT("Threshold"), 50);


}

void THISCLASS::OnStep() {
	// Get and check input image
  IplImage * longTermBackground = mCore->mDataStructureImageBinary.mImage;
  IplImage * currentFrame = mCore->mDataStructureInput.mImage;
  if (! currentFrame) {
    AddError(wxT("No input image."));
    return;
  }
  
  if (!mLastFrame) {
    mLastFrame = cvCloneImage(currentFrame);
    return;
  }

  if (!mTmp) {
    mTmp = cvCloneImage(currentFrame);
  }
  if (!mTmpColors) {
    mTmpColors = cvCreateImage(cvGetSize(currentFrame), IPL_DEPTH_32F, 1);
  }
  if (!mTmpBinary) {
    mTmpBinary = cvCloneImage(longTermBackground);
  }

  if (!mOutputImage) {
    mOutputImage = cvCloneImage(longTermBackground);
  }
  
  if (!mShortTermForegroundMask) {
    mShortTermForegroundMask = cvCloneImage(longTermBackground);
  }  

  if (!mR) {
    mR = cvCloneImage(longTermBackground);
  }     
  if (!mG) {
    mG = cvCloneImage(longTermBackground);
  }     
  if (!mB) {
    mB = cvCloneImage(longTermBackground);
  }     
  cvSub(currentFrame, mLastFrame, mTmp);
  CvScalar mu_d, sigma_d;
  cvAvgSdv(mTmp, &mu_d, &sigma_d);
  cvSubS(mTmp, mu_d, mTmp);
  cvAbs(mTmp, mTmp);
  cvScale(mTmp, mTmp, sigma_d.val[0]);

  cvSplit(mTmp, mR, mG, mB, NULL);
  cvZero(mTmpColors);
  cvAcc(mR, mTmpColors);
  cvAcc(mG, mTmpColors);
  cvAcc(mB, mTmpColors);
  cvThreshold(mTmpColors, mShortTermForegroundMask, mThreshold, 255, CV_THRESH_BINARY);
  cvErode(mShortTermForegroundMask, mShortTermForegroundMask);

  
  CBlobResult blobs;
  blobs = CBlobResult(longTermBackground, NULL, 0, true);
  blobs.Filter(blobs, B_EXCLUDE, CBlobGetMean(), B_EQUAL, 0);

  blobs.Filter(blobs, B_EXCLUDE, CBlobIntersectMask(mShortTermForegroundMask,
						    mTmpBinary),
	       B_EQUAL, 0);

  cvZero(mTmp);

  CBlob * currentBlob;
  cvZero(mOutputImage);
  for (int i = 0; i < blobs.GetNumBlobs(); i++) {
    currentBlob = blobs.GetBlob(i);

    cvZero(mTmpBinary);
    currentBlob->FillBlob(mTmpBinary, cvRealScalar(255));
	
    cvAnd(mTmpBinary, mShortTermForegroundMask, mTmpBinary);
    mStorage = cvCreateMemStorage(0);
    CvSeq * points = maskToSequence(mTmpBinary, mStorage);
    
    CvBox2D ellipse = cvMinAreaRect2(points);

    cvZero(mTmpBinary);
    cvEllipseBox(mTmpBinary, ellipse, cvRealScalar(255), CV_FILLED);
    cvAnd(mTmpBinary, longTermBackground, mTmpBinary);

    cvOr(mTmpBinary, mOutputImage, mOutputImage);
    
    currentBlob->FillBlob(mTmp, CV_RGB(255, 0, 0));
    cvEllipseBox(mTmp, ellipse, CV_RGB(0,255,0), 3, CV_AA, 0 );
    cvClearSeq(points);
    cvReleaseMemStorage(&mStorage);

  }

    
  mCore->mDataStructureImageBinary.mImage = mOutputImage;
  // Set the display
  DisplayEditor de(&mDisplayOutput);
  if (de.IsActive()) {
    de.SetMainImage(mOutputImage);
  }
  cvCopy(currentFrame, mLastFrame);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {

}
