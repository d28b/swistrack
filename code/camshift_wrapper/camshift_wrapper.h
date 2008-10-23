// camshift_wrapper.h - by Robin Hewitt, 2007
// http://www.cognotics.com/opencv/downloads/camshift_wrapper
// This is free software. See License.txt, in the download
// package, for details.
//
//
// Public interface for the Simple Camshift Wrapper

#ifndef __SIMPLE_CAMSHIFT_WRAPPER_H
#define __SIMPLE_CAMSHIFT_WRAPPER_H


struct camshiftStruct {
  // Parameters
  int   nHistBins;                 // number of histogram bins
  float rangesArr[2];          // histogram range
  int vmin; 
  int vmax; 
  int smin; // limits for calculating hue
  // File-level variables
  IplImage * pHSVImg; // the input image converted to HSV color mode
  IplImage * pHueImg; // the Hue channel of the HSV image
  IplImage * pMask; // this image is used for masking pixels
  IplImage * pProbImg; // the face probability estimates for each pixel
  CvHistogram * pHist; // histogram of hue in the original face image
  
  CvRect prevFaceRect;  // location of face in previous frame
  CvBox2D faceBox;      // current face-location estimate
  int nFrames;
};
typedef struct camshiftStruct camshift;

// Main Control functions
int     createTracker(camshift * cs, const IplImage * pImg);
void    releaseTracker(camshift * cs);
void    startTracking(camshift * cs, IplImage * pImg, CvRect * pRect);
CvBox2D track(camshift * cs, IplImage *);


// Parameter settings
void setVmin(camshift * cs, int vmin);
void setSmin(camshift * cs, int smin);

#endif
