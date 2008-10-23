// capture.c - by Robin Hewitt, 2007
// http://www.cognotics.com/opencv/downloads/camshift_wrapper
// This is free software. See License.txt, in the download
// package, for details.
//


#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include "capture.h"


// File-level variables
CvCapture * pCapture = 0;

//////////////////////////////////
// initCapture()
//
int initCapture()
{
	// Initialize video capture
	pCapture = cvCaptureFromCAM( CV_CAP_ANY );
	if( !pCapture )
	{
		fprintf(stderr, "failed to initialize video capture\n");
		return 0;
	}

	return 1;
}


//////////////////////////////////
// closeCapture()
//
void closeCapture()
{
	// Terminate video capture and free capture resources
	cvReleaseCapture( &pCapture );
	return;
}


//////////////////////////////////
// nextVideoFrame()
//
IplImage * nextVideoFrame()
{
	IplImage * pVideoFrame = cvQueryFrame( pCapture );
	if( !pVideoFrame )
		fprintf(stderr, "failed to get a video frame\n");

	return pVideoFrame;
}

