// Segmenter.h: interface for the Segmenter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEGMENTER_H__12A4A8C0_A7DD_4FD4_A1FF_10200028C06A__INCLUDED_)
#define AFX_SEGMENTER_H__12A4A8C0_A7DD_4FD4_A1FF_10200028C06A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




#include "TrackingImage.h"
#include "Input.h"
#include <vector>
#include <math.h>
#include "cv.h"
#include "highgui.h"
#include "Component.h"




/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/

enum status_code   { OK,  //0
GENERIC_ERROR, // 1 
NO_BACKGROUND_IMAGE, // 2
NO_AVI_SEQUENCE, //3
NO_XML_FILE,// 4
CAN_NOT_OPEN_BACKGROUND_FILE, // 5
CAN_NOT_OPEN_AVI_FILE, // 6
CAN_NOT_OPEN_XML_FILE, // 7
READY_TO_START, // 8
SEARCHING, // 9
RUNNING, // 10
FINISHED, // 11
STOPPED, // 12
SEGMENTER_MODE_INVALID, //13
PAUSED }; // 14
using namespace std;
/** \file Segmenter.h
    \brief Interface for the Segmenter class and the resultContour structure

    The Segmenter class implements basic image segmentation routines, found
	contours are returned in a vector of resultContour.
*/

/** \class Segmenter
    \brief Basic image segmentation routines

	The Segmenter class allows for segmenting a image, that is finding all connected contours
	in it. Found contours are sorted by their area, a threshold can be provided that rejects
	contours that are to small.
*/

class Segmenter : public Component
	{
	public:
		IplImage* GetInputImage();
		IplImage* GetBinaryImage();
		IplImage* GetBackgroundImage();	
		void RefreshCoverage();
		double GetFPS();
		Segmenter(xmlpp::Element* cfgRoot, TrackingImage* trackingimg);
		virtual ~Segmenter();
		int GetStatus();
		double GetProgress(int kind);
		int init(int overhead=2);
		int Step();
		/** Color image to display tracking progress */
		TrackingImage* trackingimg;
        void SetParameters();

		
	private:
	
		void QueryFrame1394(IplImage* input);
		void Segmentation();
		Input* input;

		/** Background image. From a file */
		IplImage* background;	
		/** Input image */
		IplImage* inputImg;		
		/** Binary image (segmented) */
		IplImage* binary;	
		/** Status code */
		status_code status;
		
		/** Fixed Threshold (0 false, 1 true)*/
		int fixedThresholdBoolean;


		/** Binarization threshold (0-255) */
		int bin_threshold;
		/** Average value of the background, for threshold based on image average (0-255)*/
		int backgroundAverage;
		/** Exponential average constant for background estimae */
		double alpha;
		//Mode 3 parameters
		/** Specified color (Mode 3) Coded as BGR */
		CvScalar specifiedColor;
		/** Color Describe Foreground boolean (Mode 3) (0 false, 1 true)*/
		int colorDescribeForegroundBoolean;
		/** Working with color ratio boolean (Mode 3) (0 false, 1 true)*/
		int workingWithColorRatioBoolean;


		//Post-Processing variables
		/** Using a mask during the Post-processing (0 false, 1 true)*/
		int usingMaskBoolean;
		/** Mask Image */
		IplImage* mask;
		/** Number of dilates at the first step*/
		int firstDilate;
		/** Number of erodes at the second step*/
		int firstErode;
		/** Number of dilates at the third step*/
		int secondDilate;

		
		double GetSqrDist(CvPoint2D32f p1, CvPoint2D32f p2);
	};

#endif // !defined(AFX_SEGMENTER_H__12A4A8C0_A7DD_4FD4_A1FF_10200028C06A__INCLUDED_)
