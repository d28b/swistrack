// Segmenter.h: interface for the Segmenter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEGMENTER_H__12A4A8C0_A7DD_4FD4_A1FF_10200028C06A__INCLUDED_)
#define AFX_SEGMENTER_H__12A4A8C0_A7DD_4FD4_A1FF_10200028C06A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




#include "polygon.h"
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
/** \file Segmenter.h
    \brief Interface for the Segmenter class and the resultContour structure

    The Segmenter class implements basic image segmentation routines, found
	contours are returned in a vector of resultContour.
*/

/** \struct resultContour
    \brief Entity to save a contour from image segmentation
*/

/** \class Segmenter
    \brief Basic image segmentation routines

	The Segmenter class allows for segmenting a image, that is finding all connected contours
	in it. Found contours are sorted by their area, a threshold can be provided that rejects
	contours that are to small.
*/
struct resultContour{
	/** The center with subpixel accuracy */
	CvPoint2D32f center;
	/** The area of the contour (in pixels) */
	double area;
	/** The contour's compactness (area/circumference ratio) */
	double compactness;
	/** The contour's orientation */
	double orientation;
	/** Smallest surrounding rectangle */
	CvRect boundingrect;
	/** Mean color of the contour */
	double color;
	};

using namespace std;

class Segmenter : public Component
	{
	public:
		void RefreshCoverage();
		double GetFPS();
		vector<resultContour>* GetContours();
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
		int GetContour(IplImage* src, vector<resultContour>* contour_obj);
		int FindContours(IplImage* src, vector<resultContour>* contour_obj);
		void GetContourColor(IplImage* input, vector<resultContour>* contour_obj);
		void BWSegmentation();

		Input* input;

		/** Background image. From a file */
		IplImage* background;	
		/** B&W version of input image */
		IplImage* current;		
		/** Binary image (segmented) */
		IplImage* binary;	
		/** Status code */
		status_code status;		
		
		/** Binarization threshold (0-255) */
		int bin_threshold;		
		/** Exponential average constant for background estimae */
		double alpha;
		
		/** Maximum number of contours to keep (only the 'max_number' largest contours are kept) */
		int max_number;

		/** Vector of resulting contours from segmentation */
		vector<resultContour> contours;
		double GetSqrDist(CvPoint2D32f p1, CvPoint2D32f p2);

		//int input_mode;
        int particlefilter_mode;


	};

#endif // !defined(AFX_SEGMENTER_H__12A4A8C0_A7DD_4FD4_A1FF_10200028C06A__INCLUDED_)
