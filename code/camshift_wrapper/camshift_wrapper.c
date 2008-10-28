// camshift_wrapper.c - by Robin Hewitt, 2007
// http://www.cognotics.com/opencv/downloads/camshift_wrapper
// This is free software. See License.txt, in the download
// package, for details.
//

#include "cv.h"
#include <stdio.h>
#include "camshift_wrapper.h"
// Declarations for internal functions
void updateHueImage(camshift * cs, const IplImage * pImg);


//////////////////////////////////
// createTracker()
//
int createTracker(camshift * cs, const IplImage * pImg)
{
	// Allocate the main data structures ahead of time
	float * pRanges = cs->rangesArr;
	cs->nHistBins = 30;
	cs->rangesArr[0] = 0;
	cs->rangesArr[1] = 180;
	cs->vmin = 65;
	cs->vmax = 256;
	cs->smin = 55;
	cs->nFrames = 0;
	cs->pHSVImg  = cvCreateImage( cvGetSize(pImg), 8, 3 );
	cs->pHueImg  = cvCreateImage( cvGetSize(pImg), 8, 1 );
	cs->pMask    = cvCreateImage( cvGetSize(pImg), 8, 1 );
	cs->pProbImg = cvCreateImage( cvGetSize(pImg), 8, 1 );

	cs->pHist = cvCreateHist( 1, &cs->nHistBins, CV_HIST_ARRAY, &pRanges, 1 );

	return 1;
}


//////////////////////////////////
// releaseTracker()
//
void releaseTracker(camshift * cs)
{
	// Release all tracker resources
	cvReleaseImage( &cs->pHSVImg );
	cvReleaseImage( &cs->pHueImg );
	cvReleaseImage( &cs->pMask );
	cvReleaseImage( &cs->pProbImg );

	cvReleaseHist( &cs->pHist );
}


//////////////////////////////////
// startTracking()
//
void startTracking(camshift * cs, IplImage * pImg, CvRect * pFaceRect)
{
	float maxVal = 0.f;

	// Make sure internal data structures have been allocated
	if( !cs->pHist ) createTracker(cs, pImg);

	// Create a new hue image
	updateHueImage(cs, pImg);

	// Create a histogram representation for the face
    cvSetImageROI( cs->pHueImg, *pFaceRect );
    cvSetImageROI( cs->pMask,   *pFaceRect );
    cvCalcHist( &cs->pHueImg, cs->pHist, 0, cs->pMask );
    cvGetMinMaxHistValue( cs->pHist, 0, &maxVal, 0, 0 );
    cvConvertScale( cs->pHist->bins, cs->pHist->bins, 
		    maxVal? 255.0/maxVal : 0, 0 );
    cvResetImageROI( cs->pHueImg );
    cvResetImageROI( cs->pMask );

	// Store the previous face location
	cs->prevFaceRect = *pFaceRect;
}


//////////////////////////////////
// track()
//
CvBox2D track(camshift * cs, IplImage * pImg)
{
	CvConnectedComp components;

	// Create a new hue image
	updateHueImage(cs, pImg);

	// Create a probability image based on the face histogram
	cvCalcBackProject( &cs->pHueImg, cs->pProbImg, cs->pHist );
    cvAnd( cs->pProbImg, cs->pMask, cs->pProbImg, 0 );

	// Use CamShift to find the center of the new face probability
    CvSize size = cvGetSize(cs->pProbImg);
    if (cs->prevFaceRect.x < 0) {
      cs->prevFaceRect.x = 0;
    }
    if (cs->prevFaceRect.x >= size.width) {
      cs->prevFaceRect.x = size.width - 1;
    }
    if (cs->prevFaceRect.y < 0) {
      cs->prevFaceRect.y = 0;
    }
    if (cs->prevFaceRect.y >= size.height) {
      cs->prevFaceRect.y = size.height - 1;
    }

    if (cs->prevFaceRect.x + cs->prevFaceRect.width > size.width) {
      cs->prevFaceRect.width = size.width - cs->prevFaceRect.x;
    }
    if (cs->prevFaceRect.y + cs->prevFaceRect.height > size.height) {
      cs->prevFaceRect.height = size.height - cs->prevFaceRect.y;
    }


    cvCamShift( cs->pProbImg, cs->prevFaceRect,
                cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),
                &components, &cs->faceBox );

	// Update face location and angle
    cs->prevFaceRect = components.rect;
	cs->faceBox.angle = -cs->faceBox.angle;

	return cs->faceBox;
}


//////////////////////////////////
// updateHueImage()
//
void updateHueImage(camshift * cs, const IplImage * pImg)
{
	// Convert to HSV color model
	cvCvtColor( pImg, cs->pHSVImg, CV_BGR2HSV );

	// Mask out-of-range values
	cvInRangeS( cs->pHSVImg, cvScalar(0, cs->smin, MIN(cs->vmin,cs->vmax), 0),
	            cvScalar(180, 256, MAX(cs->vmin,cs->vmax) ,0), cs->pMask );

	// Extract the hue channel
	cvSplit( cs->pHSVImg, cs->pHueImg, 0, 0, 0 );
}


//////////////////////////////////
// setVmin()
//
void setVmin(camshift * cs, int _vmin)
{ cs->vmin = _vmin; }


//////////////////////////////////
// setSmin()
//
void setSmin(camshift * cs, int _smin)
{ cs->smin = _smin; }


