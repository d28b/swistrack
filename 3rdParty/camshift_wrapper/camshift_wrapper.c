// camshift_wrapper.c - by Robin Hewitt, 2007
// http://www.cognotics.com/opencv/downloads/camshift_wrapper
// This is free software. See License.txt, in the download
// package, for details.
//

#include "cv.h"
#include <stdio.h>
#include "camshift_wrapper.h"

#ifdef _WIN32
#define isnan(x) _isnan(x)
#endif

// Declarations for internal functions
void updateHueImage(camshift * cs, const IplImage * pImg);

CvBox2D rectToBox(const CvRect rect) {
	CvBox2D out;
	out.center = cvPoint2D32f(rect.x + rect.width / 2.0,
	                          rect.y + rect.height / 2.0);
	out.size = cvSize2D32f(rect.width, rect.height);
	out.angle = 0;
	return out;
}


//////////////////////////////////
// createTracker()
//
int createTracker(camshift * cs, const IplImage * pImg, int histogramDims) {
	int i;
	int nHistBinsTemp[12];

	// Allocate the main data structures ahead of time
	cs->rangesArr = (float **) malloc(2 * cs->histogramDims);
	cs->nHistBins = 30;
	//cs->rangesArr[0] = 0;
	//cs->rangesArr[1] = 180;
	cs->vmin = 65;
	cs->vmax = 256;
	cs->smin = 55;
	cs->histogramDims = histogramDims;
	cs->nFrames = 0;
	for (i = 0; i < NUM_CHANNELS; i++) {
		cs->pChannels[i] = cvCreateImage( cvGetSize(pImg), 8, 1 );
	}
	cs->pMask    = cvCreateImage( cvGetSize(pImg), 8, 1 );
	cs->pProbImg = cvCreateImage( cvGetSize(pImg), 8, 1 );

	assert(cs->histogramDims < 12);
	for (i = 0; i < cs->histogramDims; i++) {
		nHistBinsTemp[i] = cs->nHistBins;
	}

	cs->pHist = cvCreateHist( cs->histogramDims, nHistBinsTemp, CV_HIST_ARRAY, 0, 1 );
	return 1;
}


//////////////////////////////////
// releaseTracker()
//
void releaseTracker(camshift * cs) {
	int i;
	for (i = 0; i < NUM_CHANNELS; i++) {
		cvReleaseImage( &cs->pChannels[i]);
	}
	cvReleaseImage( &cs->pMask );
	cvReleaseImage( &cs->pProbImg );

	cvReleaseHist( &cs->pHist );
}


//////////////////////////////////
// startTracking()
//
void startTracking(camshift * cs, IplImage * pImg, CvRect * pFaceRect) {
	int i;
	float maxVal = 0.f;

	// Make sure internal data structures have been allocated
	assert(cs);
	assert(cs->pHist);

	// Create a new hue image
	updateHueImage(cs, pImg);

	// Create a histogram representation for the face
	for (i = 0; i < NUM_CHANNELS; i++) {
		cvSetImageROI( cs->pChannels[i], *pFaceRect );
	}
	cvSetImageROI( cs->pMask,   *pFaceRect );

	cvCalcHist( cs->pChannels, cs->pHist, 0, cs->pMask );
	cvGetMinMaxHistValue( cs->pHist, 0, &maxVal, 0, 0 );
	cvConvertScale( cs->pHist->bins, cs->pHist->bins,
	                maxVal ? 255.0 / maxVal : 0, 0 );
	for (i = 0; i < NUM_CHANNELS; i++) {
		cvResetImageROI( cs->pChannels[i]);
	}

	cvResetImageROI( cs->pMask );

	// Store the previous face location
	cs->prevFaceRect = *pFaceRect;
	cs->faceBox = rectToBox(*pFaceRect);
}

void ensureSizeBounds(camshift *cs, CvSize size) {
	if (cs->prevFaceRect.x < 0) {
		cs->prevFaceRect.x = 0;
	}
	if (cs->prevFaceRect.y < 0) {
		cs->prevFaceRect.y = 0;
	}
	// -1 because the box has to be 1x1 pixels.
	if (cs->prevFaceRect.x  >= size.width - 1) {
		cs->prevFaceRect.x = size.width - 2;
	}
	if (cs->prevFaceRect.y >= size.height - 1) {
		cs->prevFaceRect.y = size.height - 2;
	}

	if (cs->prevFaceRect.x + cs->prevFaceRect.width >= size.width) {
		cs->prevFaceRect.width = size.width - cs->prevFaceRect.x - 1;
	}
	if (cs->prevFaceRect.y + cs->prevFaceRect.height >= size.height) {
		cs->prevFaceRect.height = size.height - cs->prevFaceRect.y - 1;
	}
	if (isnan(cs->faceBox.size.height) ||
	        isnan(cs->faceBox.size.width) ||
	        isnan(cs->prevFaceRect.width) ||
	        isnan(cs->prevFaceRect.height))  {
		cs->faceBox.size.height = 1;
		cs->faceBox.size.width = 1;
		cs->prevFaceRect.height = 1;
		cs->prevFaceRect.width = 1;
	}
}

//////////////////////////////////
// track()
//
CvBox2D track(camshift * cs, IplImage * pImg) {
	CvConnectedComp components;
	ensureSizeBounds(cs, cvGetSize(cs->pProbImg));

	// Create a new hue image
	updateHueImage(cs, pImg);

	// Create a probability image based on the face histogram
	cvCalcBackProject( cs->pChannels, cs->pProbImg, cs->pHist );
	cvAnd( cs->pProbImg, cs->pMask, cs->pProbImg, 0 );

	// Use CamShift to find the center of the new face probability

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
// If clients send RGB, we'll do it in RGB.  IF they send HSV, we'll do that.
//
void updateHueImage(camshift * cs, const IplImage * pImg) {

	cvInRangeS( pImg, cvScalar(0, cs->smin, MIN(cs->vmin, cs->vmax), 0),
	            cvScalar(180, 256, MAX(cs->vmin, cs->vmax) , 0), cs->pMask );

	cvSplit( pImg, cs->pChannels[0], cs->pChannels[1], cs->pChannels[2], 0 );
}


//////////////////////////////////
// setVmin()
//
void setVmin(camshift * cs, int _vmin) {
	cs->vmin = _vmin;
}


//////////////////////////////////
// setSmin()
//
void setSmin(camshift * cs, int _smin) {
	cs->smin = _smin;
}


