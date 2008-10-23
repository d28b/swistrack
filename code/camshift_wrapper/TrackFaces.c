// TrackFaces.c - by Robin Hewitt, 2007
// http://www.cognotics.com/opencv/downloads/camshift_wrapper
// This is free software. See License.txt, in the download
// package, for details.
//

///////////////////////////////////////////////////////////////////////////////
// An example program that It detects a face in live video, and automatically
// begins tracking it, using the Simple Camshift Wrapper.

#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include "capture.h"
#include "facedet.h"
#include "camshift_wrapper.h"


//// Constants
const char * DISPLAY_WINDOW = "DisplayWindow";
#define OPENCV_ROOT  "C:/Program Files/OpenCV/1.0"


//// Global variables
IplImage  * pVideoFrameCopy = 0;


//// Function definitions
int initAll();
void exitProgram(int code);
void captureVideoFrame();


//////////////////////////////////
// main()
//
void main( int argc, char** argv )
{
	CvRect * pFaceRect = 0;
	if( !initAll() ) exitProgram(-1);

	// Capture and display video frames until a face
	// is detected
	while( 1 )
	{
		// Look for a face in the next video frame
		captureVideoFrame();
		pFaceRect = detectFace(pVideoFrameCopy);

		// Show the display image
		cvShowImage( DISPLAY_WINDOW, pVideoFrameCopy );
		if( (char)27==cvWaitKey(1) ) exitProgram(0);

		// exit loop when a face is detected
		if(pFaceRect) break;
	}

	// initialize tracking
	startTracking(pVideoFrameCopy, pFaceRect);

	// Track the detected face using CamShift
	while( 1 )
	{
		CvBox2D faceBox;

		// get the next video frame
		captureVideoFrame();

		// track the face in the new video frame
		faceBox = track(pVideoFrameCopy);

		// outline face ellipse
		cvEllipseBox(pVideoFrameCopy, faceBox,
		             CV_RGB(255,0,0), 3, CV_AA, 0 );
		cvShowImage( DISPLAY_WINDOW, pVideoFrameCopy );
		if( (char)27==cvWaitKey(1) ) break;
	}

	exitProgram(0);
}


//////////////////////////////////
// initAll()
//
int initAll()
{
	if( !initCapture() ) return 0;
	if( !initFaceDet(OPENCV_ROOT
		"/data/haarcascades/haarcascade_frontalface_default.xml"))
		return 0;

	// Startup message tells user how to begin and how to exit
	printf( "\n********************************************\n"
	        "To exit, click inside the video display,\n"
	        "then press the ESC key\n\n"
			"Press <ENTER> to begin"
			"\n********************************************\n" );
	fgetc(stdin);

	// Create the display window
	cvNamedWindow( DISPLAY_WINDOW, 1 );

	// Initialize tracker
	captureVideoFrame();
	if( !createTracker(pVideoFrameCopy) ) return 0;

	// Set Camshift parameters
	setVmin(60);
	setSmin(50);

	return 1;
}


//////////////////////////////////
// exitProgram()
//
void exitProgram(int code)
{
	// Release resources allocated in this file
	cvDestroyWindow( DISPLAY_WINDOW );
	cvReleaseImage( &pVideoFrameCopy );

	// Release resources allocated in other project files
	closeCapture();
	closeFaceDet();
	releaseTracker();

	exit(code);
}


//////////////////////////////////
// captureVideoFrame()
//
void captureVideoFrame()
{
	// Capture the next frame
	IplImage  * pVideoFrame = nextVideoFrame();
	if( !pVideoFrame ) exitProgram(-1);

	// Copy it to the display image, inverting it if needed
	if( !pVideoFrameCopy )
		pVideoFrameCopy = cvCreateImage( cvGetSize(pVideoFrame), 8, 3 );
	cvCopy( pVideoFrame, pVideoFrameCopy, 0 );
	pVideoFrameCopy->origin = pVideoFrame->origin;

	if( 1 == pVideoFrameCopy->origin ) // 1 means the image is inverted
	{
		cvFlip( pVideoFrameCopy, 0, 0 );
		pVideoFrameCopy->origin = 0;
	}
}

