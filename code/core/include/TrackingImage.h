// TrackingImage.h: interface for the TrackingImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACKINGIMAGE_H__1E9374C7_ABF5_407F_B5E9_1B79CD0503B1__INCLUDED_)
#define AFX_TRACKINGIMAGE_H__1E9374C7_ABF5_407F_B5E9_1B79CD0503B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "libgeom.h"
#include "cv.h"
#include "highgui.h" // only for debugging
#include "Component.h"

class TrackingImage  : public Component
{
	public:
		void ClearCoverageImage();
		IplImage* GetRawImagePointer();
		void DrawContours(VisBlobSet* blobs);
		IplImage* GetCoveragePointer();
		void RefreshCoverage(IplImage* background);
		void Cover(CvPoint p, CvScalar color, int size);
		void Text(char* smove,CvPoint pt,CvScalar color);
		void Line(CvPoint2D32f p1,CvPoint2D32f p2,CvScalar color);
		void DrawCircle(CvPoint p, CvScalar color);
		IplImage* GetBinaryPointer();
		void Refresh(IplImage* input,IplImage *binary);
		void Init(IplImage* input,IplImage *binary);
		IplImage* GetImagePointer();
		int GetDisplay();
		void SetDisplay(int display_vid);
		void DrawRectangle(CvRect* r,CvScalar color);
		TrackingImage();
		virtual ~TrackingImage();
		/** Colorimage for drawing */
		IplImage* colorimg;	
		IplImage* binary;
		IplImage* coverage;
		IplImage* rawimage;

			
	private:
		/** flag whether output is shown */
		int display_vid;

};

#endif // !defined(AFX_TRACKINGIMAGE_H__1E9374C7_ABF5_407F_B5E9_1B79CD0503B1__INCLUDED_)
