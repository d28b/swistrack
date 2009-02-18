// facedet.c - by Robin Hewitt, 2007
// http://www.cognotics.com/opencv/downloads/camshift_wrapper
// This is free software. See License.txt, in the download
// package, for details.
//

#include "cv.h"
#include <stdio.h>
#include "facedet.h"


// File-level variables
CvHaarClassifierCascade * pCascade = 0;  // the face detector
CvMemStorage * pStorage = 0;             // memory for detector to use
CvSeq * pFaceRectSeq;                    // memory-access interface


//////////////////////////////////
// initFaceDet()
//
int initFaceDet(const char * haarCascadePath)
{
	if( !(pStorage = cvCreateMemStorage(0)) )
	{
		fprintf(stderr, "Can\'t allocate memory for face detection\n");
		return 0;
	}

	pCascade = (CvHaarClassifierCascade *)cvLoad( haarCascadePath, 0, 0, 0 );
	if( !pCascade )
	{
		fprintf(stderr, "Can\'t load Haar classifier cascade from\n"
		                "   %s\n"
		                "Please check that this is the correct path\n",
						haarCascadePath);
		return 0;
	}

	return 1;
}


//////////////////////////////////
// closeFaceDet()
//
void closeFaceDet()
{
	if(pCascade) cvReleaseHaarClassifierCascade(&pCascade);
	if(pStorage) cvReleaseMemStorage(&pStorage);
}


//////////////////////////////////
// detectFace()
//
CvRect * detectFace(IplImage * pImg)
{
	CvRect* r = 0;

	// detect faces in image
	int minFaceSize = pImg->width / 5;
	pFaceRectSeq = cvHaarDetectObjects
		(pImg, pCascade, pStorage,
		1.1,                       // increase search scale by 10% each pass
		6,                         // require six neighbors
		CV_HAAR_DO_CANNY_PRUNING,  // skip regions unlikely to contain a face
		cvSize(minFaceSize, minFaceSize));

	// if one or more faces are detected, return the first one
	if( pFaceRectSeq && pFaceRectSeq->total )
		r = (CvRect*)cvGetSeqElem(pFaceRectSeq, 0);

	return r;
}



