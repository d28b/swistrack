//******************************* DEFINITION MODULE **********************************************/
//
// Author   :   Cyrille Lebossé, cyrille.lebosse@voila.fr
//              $Author: yuri $
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
// Filename :   $RCSfile : VisBlob.cpp ,v $
// Version:     $Revision 1.0 $
/*************************************************************************************************/

// CVS/SCCS Tag
static char *id="@(#) $Id: VisBlob.cpp,v 1.1.1.1 2003/12/17 02:03:50 yuri Exp $";


/** \file VisBlob.cpp
 *  \brief Definitions of the VisBlob class methods.
 *  
 *  This file contains the definition of VisBlob class methods which calculate
 *  blobs features and store them internally.
 */


/******************************** inclusions *****************************************************/
/*************************************************************************************************/
 
#include "VisBlob.hpp"
#include <float.h>



#include <iostream>
//#include <cstdio>
using namespace std;


/*************************************************************************************************/
/************ private types, variables, classes and function prototypes **************************/
/*************************************************************************************************/


/*************************************************************************************************/
/************* public class methods and functions implementations ********************************/
/*************************************************************************************************/

/** \brief Default Constructor 
 * Initialization of blob features.  
 */
VisBlob :: VisBlob(void)
{

  contour = NULL;
  area=0;
  perimeter=0;
  compactness=0;
  orientation=0;
  centre.x=0;
  centre.y=0;
  centre_real.x=0;
  centre_real.y=0;
  key=0;
  ellipseminoraxislength=0;
  ellipsemajoraxislength=0;
  eccentricity=0;
  min_enclo_circle_centre.x=0.0;
  min_enclo_circle_centre.y=0.0;
  min_enclo_circle_radius=0.0;
  (equivalent_rect.center).x=0.0;
  (equivalent_rect.center).y=0.0;
  (equivalent_rect.angle)=0.0;
  (equivalent_rect.size).width=0.0;
  (equivalent_rect.size).height=0.0;
  majoraxis.a=0;
  majoraxis.b=0;
  majoraxis.c=0;
  minoraxis.a=0;
  minoraxis.b=0;
  minoraxis.c=0;
  ellipse_sim=0;
  bounding_rect.x=0;
  bounding_rect.y=0;
  bounding_rect.width=0;
  bounding_rect.height=0;
  majoraxislength=0;
  minoraxislength=0;
}




/** \fn Blob::Blob(CvSeq* c,_IplROI* r)
 *  \brief Constructor from a given contour 
 *
 *  If the contour was found in the Region Of Interest(ROI) of an image, 
 *  the ROI is given to the constructor to readjust blob center coordinates
 *
 * \param c: A contour
 * \param r: The Region of Interest where the contour was found.
 */
VisBlob :: VisBlob(CvSeq* c,_IplROI* r)
{
  if (r)
	roi = *r;
  else
  {
	roi.coi=0;
	roi.xOffset=0;
	roi.yOffset=0;
	roi.width=0;
	roi.height=0;
  }
  contour = c;
  area=0;
  perimeter=0;
  compactness=0;
  orientation=0;
  centre.x=0;
  centre.y=0;
  centre_real.x=0;
  centre_real.y=0;
  key=0;
  ellipseminoraxislength=0;
  ellipsemajoraxislength=0;
  eccentricity=0;
  min_enclo_circle_centre.x=0.0;
  min_enclo_circle_centre.y=0.0;
  min_enclo_circle_radius=0.0;
  (equivalent_rect.center).x=0.0;
  (equivalent_rect.center).y=0.0;
  (equivalent_rect.angle)=0.0;
  (equivalent_rect.size).width=0.0;
  (equivalent_rect.size).height=0.0;
  majoraxis.a=0;
  majoraxis.b=0;
  majoraxis.c=0;
  minoraxis.a=0;
  minoraxis.b=0;
  minoraxis.c=0;
  ellipse_sim=0;
  bounding_rect.x=0;
  bounding_rect.y=0;
  bounding_rect.width=0;
  bounding_rect.height=0;
  majoraxislength=0;
  minoraxislength=0;
}

/** Destructor */
VisBlob :: ~VisBlob()
{
  contour=NULL;
}


//-------------------------------------------------------------------------------------
/** \brief Calculates Blob area 
 *
 * This method calculates the blob area by using the OpenCV function "cvContourArea"
 * and stores it internally.
 */
void VisBlob::Calc_Area(void)
{
 
	area = cvContourArea(contour);
	area = fabs(area);

}
//--------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
/** \brief Calculates Blob centre 
 *
 *  This method calculates the blob centre by using moments 
 *  and stores it internally.
 *  If a ROI is defined, blob center coordinates are reajusted so that they are expressed in the
 *	global image reference frame.
 */
void VisBlob::Calc_Centre(void)
{
	CvMoments moments;

	//param contour: a CvSeq* containing the contour
	cvContourMoments(contour,&moments);
	centre.x = int(moments.m10/moments.m00);
	centre.y = int(moments.m01/moments.m00);

	centre_real.x = float(moments.m10/moments.m00);
	centre_real.y = float(moments.m01/moments.m00);


	if( &roi != NULL )
	{
		centre.x += roi.xOffset;
		centre.y += roi.yOffset;
		
		centre_real.x += roi.xOffset;
		centre_real.y += roi.yOffset;

	}

}
//--------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
/** \brief Calculates Blob orientation ((angle in degree compared to the horizontal axis)). 
 *
 * This method calculates the blob orientation by using moments
 * and stores it internally.
 */
void VisBlob::Calc_Orientation(void)
{
	CvMoments moments;

	//param contour: a CvSeq* containing the contour
	cvContourMoments(contour,&moments);
	centre.x = int(moments.m10/moments.m00);
	centre.y = int(moments.m01/moments.m00);

	centre_real.x = float(moments.m10/moments.m00);
	centre_real.y = float(moments.m01/moments.m00);

/*
	if( &roi != NULL )
	{
		centre.x += roi.xOffset;
		centre.y += roi.yOffset;
		
		centre_real.x += roi.xOffset;
		centre_real.y += roi.yOffset;
	}

*/
	orientation = 0.5*atan2( 2*(moments.m11/moments.m00 - centre_real.x*centre_real.y) , ( (moments.m20/moments.m00 - centre_real.x*centre_real.x)-(moments.m02/moments.m00 - centre_real.y*centre_real.y) )) ;
	orientation = -orientation*180/PI;

}
//--------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
/** \brief Calculates Major Axis Length of the blob. 
 *
 * This method calculates the Major Axis Length of the blob by using moments
 * and stores it internally.
 */
void VisBlob::Calc_MajorAxisLength(void)
{
	CvMoments moments;
	double a,b,c;


	//param contour: a CvSeq* containing the contour
	cvContourMoments(contour,&moments);
	centre.x = int(moments.m10/moments.m00);
	centre.y = int(moments.m01/moments.m00);

	centre_real.x = float(moments.m10/moments.m00);
	centre_real.y = float(moments.m01/moments.m00);

	a=(moments.m20/moments.m00)-centre_real.x*centre_real.x;
	c=(moments.m02/moments.m00)-centre_real.y*centre_real.y;
	b=2*(moments.m11/moments.m00-centre_real.y*centre_real.x);

	majoraxislength = (double)sqrt( (a + c + sqrt(b*b-(a-c)*(a-c)))/2 );

}
//--------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
/** \brief Calculates Minor Axis Length of the blob. 
 *
 * This method calculates the Minor Axis Length of the blob by using moments
 * and stores it internally.
 */
void VisBlob::Calc_MinorAxisLength(void)
{
	CvMoments moments;
	double a,b,c;


	//param contour: a CvSeq* containing the contour
	cvContourMoments(contour,&moments);
	centre.x = int(moments.m10/moments.m00);
	centre.y = int(moments.m01/moments.m00);

	centre_real.x = float(moments.m10/moments.m00);
	centre_real.y = float(moments.m01/moments.m00);

	a=(moments.m20/moments.m00)-centre_real.x*centre_real.x;
	c=(moments.m02/moments.m00)-centre_real.y*centre_real.y;
	b=2*(moments.m11/moments.m00-centre_real.y*centre_real.x);

	minoraxislength = (double)sqrt( (a + c - sqrt(b*b-(a-c)*(a-c)))/2 );

}
//--------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------
/** \brief Calculates Blob eccentricity. 
 *
 * This method calculates the blob eccentricity by using moments
 * and stores it internally.
 */
void VisBlob::Calc_Eccentricity(void)
{
	CvMoments moments;
	CvMat* C = cvCreateMat(2,2, CV_32FC1);
	CvMat* Vect = cvCreateMat(2,2, CV_32FC1);
	CvMat* Val = cvCreateMat(2,1, CV_32FC1);
	float* tab;
	float* val;
	int step;
	CvSize size;
	float minval, maxval;

	if(  (area == 0)  )
	{
		Calc_Area();
	}
	
	//param contour: a CvSeq* containing the contour
	cvContourMoments(contour,&moments);
	centre.x = int(moments.m10/moments.m00);
	centre.y = int(moments.m01/moments.m00);

	centre_real.x = float(moments.m10/moments.m00);
	centre_real.y = float(moments.m01/moments.m00);

/*
	if( &roi != NULL )
	{
		centre.x += roi.xOffset;
		centre.y += roi.yOffset;
		
		centre_real.x += roi.xOffset;
		centre_real.y += roi.yOffset;

	}
*/


	cvGetRawData(C, (uchar**) &tab,&step,&size);
	tab[0] = (float)(moments.m20 - centre_real.x*centre_real.x*moments.m00);
	tab[1] = (float)(moments.m11 - centre_real.x*centre_real.y*moments.m00);
	tab[2] = (float)(moments.m11 - centre_real.x*centre_real.y*moments.m00);
	tab[3] = (float)(moments.m02 - centre_real.y*centre_real.y*moments.m00);


	cvEigenVV (C, Vect, Val, DBL_EPSILON);

	cvGetRawData(Val, (uchar**) &val,&step,&size);


	if( val[0] > val[1] )
	{
		minval=val[1];
		maxval=val[0];
	}
	else
	{
		minval=val[0];
		maxval=val[1];
	}

	eccentricity = (double)sqrt(1-minval/maxval);

	cvReleaseMat( &C);
	cvReleaseMat( &Vect);
	cvReleaseMat( &Val);

}
//--------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
/** \brief Calculates Major Axis Length of the "equivalent" ellipse of the blob. 
 *
 * This method calculates the Major Axis Length of the "equivalent" ellipse of the blob by using moments
 * and stores it internally.
 */
void VisBlob::Calc_EllipseMajorAxisLength(void)
{
	CvMoments moments;
	CvMat* C = cvCreateMat(2,2, CV_32FC1);
	CvMat* Vect = cvCreateMat(2,2, CV_32FC1);
	CvMat* Val = cvCreateMat(2,1, CV_32FC1);
	float* tab;
	float* val;
	int step;
	CvSize size;
	float minval, maxval;


	//param contour: a CvSeq* containing the contour
	cvContourMoments(contour,&moments);
	centre.x = int(moments.m10/moments.m00);
	centre.y = int(moments.m01/moments.m00);

	centre_real.x = float(moments.m10/moments.m00);
	centre_real.y = float(moments.m01/moments.m00);

/*
	if( &roi != NULL )
	{
		centre.x += roi.xOffset;
		centre.y += roi.yOffset;
		
		centre_real.x += roi.xOffset;
		centre_real.y += roi.yOffset;
	}
*/
	cvGetRawData(C, (uchar**) &tab,&step,&size);
	tab[0] = (float)(moments.m20 - centre_real.x*centre_real.x*moments.m00);
	tab[1] = (float)(moments.m11 - centre_real.x*centre_real.y*moments.m00);
	tab[2] = (float)(moments.m11 - centre_real.x*centre_real.y*moments.m00);
	tab[3] = (float)(moments.m02 - centre_real.y*centre_real.y*moments.m00);


	cvEigenVV (C, Vect, Val, DBL_EPSILON);

	cvGetRawData(Val, (uchar**) &val,&step,&size);

	if( val[0] > val[1] )
	{
		minval=val[1];
		maxval=val[0];
	}
	else
	{
		minval=val[0];
		maxval=val[1];
	}

	ellipsemajoraxislength = (double)sqrt( sqrt(4*sqrt(maxval*maxval*maxval)/(PI*sqrt(minval))) )*2;


	cvReleaseMat( &C);
	cvReleaseMat( &Vect);
	cvReleaseMat( &Val);

}
//--------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
/** \brief Calculates Minor Axis Length of "equivalent" ellipse of the blob. 
 *
 * This method calculates the Minor Axis Length of "equivalent" ellipse of the blob by using moments
 * and stores it internally.
 */
void VisBlob::Calc_EllipseMinorAxisLength(void)
{
	CvMoments moments;
	CvMat* C = cvCreateMat(2,2, CV_32FC1);
	CvMat* Vect = cvCreateMat(2,2, CV_32FC1);
	CvMat* Val = cvCreateMat(2,1, CV_32FC1);
	float* tab;
	float* val;
	int step;
	CvSize size;
	float minval, maxval;

	//param contour: a CvSeq* containing the contour
	cvContourMoments(contour,&moments);
	centre.x = int(moments.m10/moments.m00);
	centre.y = int(moments.m01/moments.m00);

	centre_real.x = float(moments.m10/moments.m00);
	centre_real.y = float(moments.m01/moments.m00);

/*
	if( &roi != NULL )
	{
		centre.x += roi.xOffset;
		centre.y += roi.yOffset;
		
		centre_real.x += roi.xOffset;
		centre_real.y += roi.yOffset;
	}
*/
	cvGetRawData(C, (uchar**) &tab,&step,&size);
	tab[0] = (float)(moments.m20 - centre_real.x*centre_real.x*moments.m00);
	tab[1] = (float)(moments.m11 - centre_real.x*centre_real.y*moments.m00);
	tab[2] = (float)(moments.m11 - centre_real.x*centre_real.y*moments.m00);
	tab[3] = (float)(moments.m02 - centre_real.y*centre_real.y*moments.m00);


	cvEigenVV (C, Vect, Val, DBL_EPSILON);

	cvGetRawData(Val, (uchar**) &val,&step,&size);

	if( val[0] > val[1] )
	{
		minval=val[1];
		maxval=val[0];
	}
	else
	{
		minval=val[0];
		maxval=val[1];
	}

	ellipseminoraxislength = (double)sqrt( sqrt(4*sqrt(minval*minval*minval)/(PI*sqrt(maxval))) )*2;

	cvReleaseMat( &C);
	cvReleaseMat( &Vect);
	cvReleaseMat( &Val);

}
//--------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
/** \brief Calculates Major Axis of the blob. 
 *
 * This method calculates the Major Axis of the blob by using moments.
 * and stores it internally.
 */
void VisBlob::Calc_MajorAxis(void)
{
	CvMoments moments;
	CvMat* C = cvCreateMat(2,2, CV_32FC1);
	CvMat* Vect = cvCreateMat(2,2, CV_32FC1);
	CvMat* Val = cvCreateMat(2,1, CV_32FC1);
	float* tab;
	float* val;
	float* vect;
	int step, step2;
	CvSize size, size2;

	
	
	//param contour: a CvSeq* containing the contour
	cvContourMoments(contour,&moments);
	centre.x = int(moments.m10/moments.m00);
	centre.y = int(moments.m01/moments.m00);

	centre_real.x = float(moments.m10/moments.m00);
	centre_real.y = float(moments.m01/moments.m00);

/*
	if( &roi != NULL )
	{
		centre.x += roi.xOffset;
		centre.y += roi.yOffset;
		
		centre_real.x += roi.xOffset;
		centre_real.y += roi.yOffset;
	}
*/
	cvGetRawData(C, (uchar**) &tab,&step,&size);
	tab[0] = (float)(moments.m20 - centre_real.x*centre_real.x*moments.m00);
	tab[1] = (float)(moments.m11 - centre_real.x*centre_real.y*moments.m00);
	tab[2] = (float)(moments.m11 - centre_real.x*centre_real.y*moments.m00);
	tab[3] = (float)(moments.m02 - centre_real.y*centre_real.y*moments.m00);


	cvEigenVV (C, Vect, Val, DBL_EPSILON);

	cvGetRawData(Val, (uchar**) &val,&step,&size);
	cvGetRawData(Vect, (uchar**) &vect,&step2,&size2);

	if( val[0] < val[1] )
	{
		majoraxis.b = -vect[1]/sqrt(vect[1]*vect[1] + vect[3]*vect[3]);
		majoraxis.a = vect[3]/sqrt(vect[1]*vect[1] + vect[3]*vect[3]);
	}
	else
	{
		majoraxis.b = -vect[0]/sqrt(vect[2]*vect[2] + vect[0]*vect[0]);
		majoraxis.a = vect[2]/sqrt(vect[2]*vect[2] + vect[0]*vect[0]);
	}

	majoraxis.c = -majoraxis.a*centre_real.x -majoraxis.b*centre_real.y;

	cvReleaseMat( &C);
	cvReleaseMat( &Vect);
	cvReleaseMat( &Val);

}
//--------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
/** \brief Calculates Minor Axis of the blob. 
 *
 * This method calculates the Minor Axis of the blob by using moments.
 * and stores it internally.
 */
void VisBlob::Calc_MinorAxis(void)
{
	CvMoments moments;
	CvMat* C = cvCreateMat(2,2, CV_32FC1);
	CvMat* Vect = cvCreateMat(2,2, CV_32FC1);
	CvMat* Val = cvCreateMat(2,1, CV_32FC1);
	float* tab;
	float* val;
	float* vect;
	int step, step2;
	CvSize size, size2;

	
	
	//param contour: a CvSeq* containing the contour
	cvContourMoments(contour,&moments);
	centre.x = int(moments.m10/moments.m00);
	centre.y = int(moments.m01/moments.m00);

	centre_real.x = float(moments.m10/moments.m00);
	centre_real.y = float(moments.m01/moments.m00);

/*
	if( &roi != NULL )
	{
		centre.x += roi.xOffset;
		centre.y += roi.yOffset;
		
		centre_real.x += roi.xOffset;
		centre_real.y += roi.yOffset;
	}
*/
	cvGetRawData(C, (uchar**) &tab,&step,&size);
	tab[0] = (float)(moments.m20 - centre_real.x*centre_real.x*moments.m00);
	tab[1] = (float)(moments.m11 - centre_real.x*centre_real.y*moments.m00);
	tab[2] = (float)(moments.m11 - centre_real.x*centre_real.y*moments.m00);
	tab[3] = (float)(moments.m02 - centre_real.y*centre_real.y*moments.m00);


	cvEigenVV (C, Vect, Val, DBL_EPSILON);

	cvGetRawData(Val, (uchar**) &val,&step,&size);
	cvGetRawData(Vect, (uchar**) &vect,&step2,&size2);

	if( val[0] > val[1] )
	{
		minoraxis.b = -vect[1]/sqrt(vect[1]*vect[1] + vect[3]*vect[3]);
		minoraxis.a = vect[3]/sqrt(vect[1]*vect[1] + vect[3]*vect[3]);
	}
	else
	{
		minoraxis.b = -vect[0]/sqrt(vect[0]*vect[0] + vect[2]*vect[2]);
		minoraxis.a = vect[2]/sqrt(vect[0]*vect[0] + vect[2]*vect[2]);
	}

	minoraxis.c = -minoraxis.a*centre_real.x -minoraxis.b*centre_real.y;

	cvReleaseMat( &C);
	cvReleaseMat( &Vect);
	cvReleaseMat( &Val);

}
//--------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------
/** \brief Calculates Blob perimeter 
 *
 * This method calculates the blob perimeter by using the OpenCV function "cvArcLength" 
 * and stores it internally.
 * A 95% correction factor is used to account for a systematic error due to the
 * contour discretization.
 */
void VisBlob::Calc_Perimeter(void)
{
 
	//param contour: a CvSeq* containing the contour
	// 0.95 = correction factor 
	perimeter = 0.95*cvArcLength( contour, CV_WHOLE_SEQ, 1 );

	
}
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
/** \brief Calculates Blob compactness 
 *
 *  This method calculates the blob compactness by using OpenCV functions. 
 *  Compactness is a contour feature defined as 
 *	\f[ \frac{4*pi*Area}{Perimeter^2} \f]
 *  It is maximum (1) for a circle, less but superior to 0 for other shapes.
 *
 *	If the blob perimeter is less than 50, then the error on the compactness is
 *	bigger than 10%.
 *
 * Uses the methods "calc_area()" and "calc_perimeter()"
 * Puts the compactness in the public member "compactness"
 */
void VisBlob::Calc_Compactness(void)
{
	if(  (area == 0) || (perimeter == 0) )
	{
		Calc_Area();
		Calc_Perimeter();
	}

//	if( perimeter < 50 ){ compactness = -1; } // The blob is too small
//	else
//	{
		//param contour: a CvSeq* containing the contour
		compactness = 4*PI*area/(perimeter*perimeter);

		if( compactness > 1 ){ compactness = 1; } //correction
//	}

}
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
/** \brief Calculates ellipse similarity factor of the blob 
 *
 *  This method calculates the factor defined as 
 *	\f[ \frac{Area}{MinEnclosingCircleRadius^2} \f]
 *
 * Uses the methods "Calc_Area()" and "Calc_MinEncloCircle()"
 * Puts the factor in the public member "ellipse_sim"
 */
void VisBlob::Calc_EllipseSim(void)
{
	if(  (area == 0) || (min_enclo_circle_radius == 0) )
	{
		Calc_Area();
		Calc_MinEncloCircle();
	}
	
		ellipse_sim = area/(min_enclo_circle_radius*min_enclo_circle_radius);

}
//--------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------
/** \brief Calculates the minimum enclosing circle of the blob 
 *
 *  This function finds the minimal enclosing circle for the blob. "Enclosing" means
 *  that all the points from the blob are either inside or on the boundary of the
 *  circle. "Minimal" means that there is no enclosing circle of a smaller radius.
 *  It is stored internally.
 */
void VisBlob::Calc_MinEncloCircle(void)
{
 
	cvMinEnclosingCircle(contour, &min_enclo_circle_centre, &min_enclo_circle_radius);
	// This function can't be found in "OpenCVMan.pdf" (old version)
	// Because this is the new version of the function "cvFindMinEnclosingCircle"

	if( &roi != NULL )
	{		
		min_enclo_circle_centre.x += roi.xOffset;
		min_enclo_circle_centre.y += roi.yOffset;
	}


}
//--------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------
/** \brief Calculates the minimum area circumscribed rectangle for the blob using moments.
 *
 *  This function finds a circumscribed rectangle of the minimal area. 
 *
 *  It is stored internally in a CvBox2D.
 */
void VisBlob::Calc_EquivalentRect(void)
{

	/*-----------------------------------------------------------------
	CvPoint* tab;
	CvMat mat;

	tab = new CvPoint[contour->total];

	for( int i=0 ; i<contour->total ; i++ )
	{
		tab[i] = *(CvPoint*)cvGetSeqElem( contour, i );
	}

	 mat = cvMat( 1, contour->total, CV_32SC2, tab );
	
	
	min_area_rect = cvMinAreaRect2(&mat);
	
	// WARNING !!!!!!!!!
	// This OpenCV function doesn't work !!!!!!!!!!!
	// WARNING !!!!!!!!!
	
	
	//This function can't be found in "OpenCVMan.pdf" (old version)
	//Because this is the new version of the function "cvMinAreaRect"
	//It returns a "CvBox2D" which defines the rectangle.
	
	-----------------------------------------------------------------*/
	
	CvMoments moments;
	CvMat* C = cvCreateMat(2,2, CV_32FC1);
	CvMat* Vect = cvCreateMat(2,2, CV_32FC1);
	CvMat* Val = cvCreateMat(2,1, CV_32FC1);
	float* tab;
	float* val;
	int step;
	CvSize size;
	float minval, maxval;

	
	//param contour: a CvSeq* containing the contour
	cvContourMoments(contour,&moments);
	centre.x = int(moments.m10/moments.m00);
	centre.y = int(moments.m01/moments.m00);

	centre_real.x = float(moments.m10/moments.m00);
	centre_real.y = float(moments.m01/moments.m00);

/*
	if( &roi != NULL )
	{
		centre.x += roi.xOffset;
		centre.y += roi.yOffset;
		
		centre_real.x += roi.xOffset;
		centre_real.y += roi.yOffset;
	}
*/
	cvGetRawData(C, (uchar**) &tab,&step,&size);
	tab[0] = (float)(moments.m20 - centre_real.x*centre_real.x*moments.m00);
	tab[1] = (float)(moments.m11 - centre_real.x*centre_real.y*moments.m00);
	tab[2] = (float)(moments.m11 - centre_real.x*centre_real.y*moments.m00);
	tab[3] = (float)(moments.m02 - centre_real.y*centre_real.y*moments.m00);


	cvEigenVV (C, Vect, Val, DBL_EPSILON);

	cvGetRawData(Val, (uchar**) &val,&step,&size);

	if( val[0] > val[1] )
	{
		minval=val[1];
		maxval=val[0];
	}
	else
	{
		minval=val[0];
		maxval=val[1];
	}

	

	equivalent_rect.angle = -(float)0.5*(float)atan2( 2*(moments.m11/moments.m00 - centre_real.x*centre_real.y) , ( (moments.m20/moments.m00 - centre_real.x*centre_real.x)-(moments.m02/moments.m00 - centre_real.y*centre_real.y) )) ;
	
	if( &roi != NULL )
	{
		centre.x += roi.xOffset;
		centre.y += roi.yOffset;
		
		centre_real.x += roi.xOffset;
		centre_real.y += roi.yOffset;
	}
	
	equivalent_rect.center = cvPoint2D32f(centre_real.x,centre_real.y);
	(equivalent_rect.size).width = (float)sqrt( sqrt(12*sqrt(minval*minval*minval)/(sqrt(maxval))) );
	(equivalent_rect.size).height = (float)sqrt( sqrt(12*sqrt(maxval*maxval*maxval)/(sqrt(minval))) );


	cvReleaseMat( &C);
	cvReleaseMat( &Vect);
	cvReleaseMat( &Val);

}
//--------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
/** \brief Calculates the bounding rectangle of the blob 
 *
 *  This function finds the bounding rectangle for the blob. "Bounding" means
 *  that all the points from the blob are either inside or on the boundary of the
 *  rectangle. The rectangle is horizontal that is why it is not the minimum
 *	enclosing rectangle wchich would have the same orientation as the blob.
 *
 */
void VisBlob::Calc_BoundingRectangle(void)
{
 
	bounding_rect = cvBoundingRect( contour, 1 );
	// this function returns a CvRect.

}
//--------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------
/** \brief Returns the angle of the segment constituted by the blob center and the given point 
 *
 */
double VisBlob :: GetSegmentAngle( CvPoint2D32f point )
{
	double b;
	double a;

	b = point.y - centre_real.y;
	a = point.x - centre_real.x;

	segment_angle = atan2(b,a);
	segment_angle = -segment_angle*180/PI;

	return segment_angle;
}
//--------------------------------------------------------------------------------------




/*************************************************************************************************/
/********************** test routines/program ****************************************************/
/*************************************************************************************************/

#ifdef BLOB_TEST
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
//#include "ipllpm.h"

void tst0(void) 
{
 	CvContourScanner blob_list;
	CvMemStorage* mem    = cvCreateMemStorage();
	CvSeq* contour_blob  = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvContour),mem);
    IplImage* imagetmp   = cvCreateImage(cvSize(500,500),IPL_DEPTH_8U,1);
	cvCircle(imagetmp,cvPoint(250,250),100,255,-1);
    //savetiff(imagetmp,"circle.tif");
    blob_list = cvStartFindContours(imagetmp,mem,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
    contour_blob = cvFindNextContour(blob_list);
	contour_blob = cvFindNextContour(blob_list);
	if (contour_blob)
	{
		VisBlob titi(contour_blob);
		titi.Calc_Area();
		cout << "\n Area = " << titi.GetArea() << endl << endl;
	}

};

void tst1(void) 
{
	CvContourScanner blob_list;
	CvMemStorage* mem    = cvCreateMemStorage();
	CvSeq* contour_blob  = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvContour),mem);
    IplImage* imagetmp   = cvCreateImage(cvSize(500,500),IPL_DEPTH_8U,1);
	cvCircle(imagetmp,cvPoint(250,250),100,255,-1);
    //savetiff(imagetmp,"circle.tif");
    blob_list = cvStartFindContours(imagetmp,mem,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
    contour_blob = cvFindNextContour(blob_list);
	contour_blob = cvFindNextContour(blob_list);
	if (contour_blob)
	{
		VisBlob titi(contour_blob);
		titi.Calc_Perimeter();
		cout << "\n Perimeter =  " << titi.GetPerimeter() << endl << endl;
	}
};
void tst2(void) 
{
	CvContourScanner blob_list;
	CvMemStorage* mem    = cvCreateMemStorage();
	CvSeq* contour_blob  = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvContour),mem);
    IplImage* imagetmp   = cvCreateImage(cvSize(500,500),IPL_DEPTH_8U,1);
	cvCircle(imagetmp,cvPoint(250,250),100,255,-1);
    //savetiff(imagetmp,"circle.tif");
    blob_list = cvStartFindContours(imagetmp,mem,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
    contour_blob = cvFindNextContour(blob_list);
	contour_blob = cvFindNextContour(blob_list);
	if (contour_blob)
	{
		VisBlob titi(contour_blob);
		titi.Calc_Compactness();
		cout << "\n Compactness =  " << titi.GetCompactness() << endl << endl;
		cout << " Real Compactness =  " << 1 << endl ;
		cout << " Error on the Compactness =  " << ((1 - titi.GetCompactness())/1)*100 << " %" << endl ;
		cout << "\n --------------------------------------------" << endl << endl ;
	}
};
void tst3(void) 
{
	FILE* per_err;
	FILE* ar_err;
	FILE* cl_size;
	per_err = fopen( "cl_per_err.dat" , "w" );
	ar_err = fopen( "cl_ar_err.dat" , "w" );
	cl_size = fopen( "cl_size.dat" , "w" );

	CvContourScanner blob_list;
	CvMemStorage* mem    = cvCreateMemStorage();
	CvSeq* contour_blob  = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvContour),mem);
    IplImage* imagetmp   = cvCreateImage(cvSize(500,500),IPL_DEPTH_8U,1);
	for(int i=0 ; i<25 ; i++ )
	{	
		cvCircle(imagetmp,cvPoint(250,250),(1+i*10),255,-1);

		int circle_s = (1+i*10) ;
		fprintf(cl_size," %d \n",circle_s);
		//savetiff(imagetmp,"circle.tif");
		blob_list = cvStartFindContours(imagetmp,mem,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
		contour_blob = cvFindNextContour(blob_list);
		contour_blob = cvFindNextContour(blob_list);
		if (contour_blob)
		{
			cout << "\n Circle radius =  " << (1+i*10) << "  pixels" << endl ;
			VisBlob titi(contour_blob);
			titi.Calc_Perimeter();
			cout << " Perimeter =  " << titi.GetPerimeter() << endl ;
			cout << " Real Perimeter =  " << (2*PI*(1+i*10)) << endl ;
			cout << " Error on the Perimeter =  " << ((2*PI*(1+i*10)/titi.GetPerimeter())*100 -100) << " %" << endl ;
			double err_p = ((2*PI*(1+i*10)/titi.GetPerimeter())*100 -100) ;
			fprintf(per_err," %lf \n",err_p);
			
			titi.Calc_Area();
			cout << " Area =  " << titi.GetArea() << endl ;
			cout << " Real Area =  " << (PI*(1+i*10)*(1+i*10)) << endl ;
			cout << " Error on the Area =  " << ((PI*(1+i*10)*(1+i*10)/titi.GetArea())*100 -100) << " %" << endl ;
			double err_a = ((PI*(1+i*10)*(1+i*10)/titi.GetArea())*100 -100) ;
			fprintf(ar_err," %lf \n",err_a);
			
			cout << "--------------------------------------------" << endl << endl ;
		}
	}

	fclose(cl_size);
	fclose(per_err);
	fclose(ar_err);


};
void tst4(void) 
{
	FILE* per_err;
	FILE* ar_err;
	FILE* sq_size;
	per_err = fopen( "sq_per_err.dat" , "w" );
	ar_err = fopen( "sq_ar_err.dat" , "w" );
	sq_size = fopen( "sq_size.dat" , "w" );

	CvContourScanner blob_list;
	CvMemStorage* mem    = cvCreateMemStorage();
	CvSeq* contour_blob  = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvContour),mem);
    IplImage* imagetmp   = cvCreateImage(cvSize(500,500),IPL_DEPTH_8U,1);
	for(int i=0 ; i<25 ; i++ )
	{	
		cvRectangle (imagetmp, cvPoint(50,50), cvPoint( (50+ i*10) , (50+ i*10)),255,-1);
		
		int square_s = i*10 ;
		fprintf(sq_size," %d \n",square_s);
		//savetiff(imagetmp,"circle.tif");
		blob_list = cvStartFindContours(imagetmp,mem,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
		contour_blob = cvFindNextContour(blob_list);
		contour_blob = cvFindNextContour(blob_list);
		if (contour_blob)
		{
			cout << "\n Square size  =  " << (i*10) << "  pixels" << endl ;
			VisBlob titi(contour_blob);
			titi.Calc_Perimeter();
			cout << " Perimeter =  " << titi.GetPerimeter() << endl ;
			cout << " Real Perimeter =  " << ((i*10)*4) << endl ;
			cout << " Error on the Perimeter =  " << ((((i*10)*4)/titi.GetPerimeter())*100 -100) << " %" << endl ;
			double err_p = ((((i*10)*4)/titi.GetPerimeter())*100 -100) ;
			fprintf(per_err," %lf \n",err_p);
			
			titi.Calc_Area();
			cout << " Area =  " << titi.GetArea() << endl ;
			cout << " Real Area =  " << ((i*10)*(i*10)) << endl ;
			cout << " Error on the Area =  " << ((((i*10)*(i*10))/titi.GetArea())*100 -100) << " %" << endl ;
			double err_a = ((((i*10)*(i*10))/titi.GetArea())*100 -100) ;
			fprintf(ar_err," %lf \n",err_a);
			
			cout << "--------------------------------------------" << endl << endl ;
		}
	}

	fclose(sq_size);
	fclose(per_err);
	fclose(ar_err);

};
void tst5(void) 
{
	
	
	FILE* per_err;
	FILE* ar_err;
	FILE* ell_size;
	per_err = fopen( "ell_per_err.dat" , "w" );
	ar_err = fopen( "ell_ar_err.dat" , "w" );
	ell_size = fopen( "ell_sm_axis.dat" , "w" );

	CvContourScanner blob_list;
	CvMemStorage* mem    = cvCreateMemStorage();
	CvSeq* contour_blob  = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvContour),mem);
    IplImage* imagetmp   = cvCreateImage(cvSize(500,500),IPL_DEPTH_8U,1);


	for(int i=0 ; i<25 ; i++ )
	{	
	
		cvEllipse (imagetmp, cvPoint(250,250), cvSize( (int)(1.5*(1+i*5)),(1+i*5)),0,-180,180,255,-1);
		
		int ellipse_s = (1+i*5) ;
		fprintf(ell_size," %d \n",ellipse_s);
		//savetiff(imagetmp,"ellipse.tif");

		blob_list = cvStartFindContours(imagetmp,mem,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
		contour_blob = cvFindNextContour(blob_list);
		contour_blob = cvFindNextContour(blob_list);
		if (contour_blob)
		{
			cout << "\n Small Axis  =  " << ellipse_s << " " << endl ;
			VisBlob titi(contour_blob);
			titi.Calc_Perimeter();
			cout << " Perimeter =  " << titi.GetPerimeter() << endl ;
			cout << " Real Perimeter =  " << (PI*sqrt(2*((1.5*(1+i*5))*(1.5*(1+i*5)) + (1+i*5)*(1+i*5) ))) << endl ;
			cout << " Error on the Perimeter =  " << ((((PI*sqrt(2*((1.5*(1+i*5))*(1.5*(1+i*5)) + (1+i*5)*(1+i*5) ))))/titi.GetPerimeter())*100 -100) << " %" << endl ;
			double err_p = (((PI*sqrt(2*((1.5*(1+i*5))*(1.5*(1+i*5)) + (1+i*5)*(1+i*5) )))/titi.GetPerimeter())*100 -100) ;
			fprintf(per_err," %lf \n",err_p);
			
			titi.Calc_Area();
			cout << " Area =  " << titi.GetArea() << endl ;
			cout << " Real Area =  " << (PI*(1.5*(1+i*5))*(1+i*5)) << endl ;
			cout << " Error on the Area =  " << (((PI*(1.5*(1+i*5))*(1+i*5))/titi.GetArea())*100 -100) << " %" << endl ;
			double err_a = (((PI*(1.5*(1+i*5))*(1+i*5))/titi.GetArea())*100 -100) ;
			fprintf(ar_err," %lf \n",err_a);
			
			cout << "--------------------------------------------" << endl << endl ;
		}
	}

	fclose(ell_size);
	fclose(per_err);
	fclose(ar_err);


};
void tst6(void) 
{
	ofstream ostr("Ell_per_area.dat") ;

	CvContourScanner blob_list;
	CvMemStorage* mem    = cvCreateMemStorage();
	CvSeq* contour_blob  = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvContour),mem);
    IplImage* imagetmp   = cvCreateImage(cvSize(500,500),IPL_DEPTH_8U,1);

	ostr << "% \t" << "Small Axis" << "\t" << "Perimeter" << "\t" << "Area" << "\t" << "\n\n";
    double A,P;
	for(int i=0 ; i<60 ; i++ )
	{	
	
		cvEllipse (imagetmp, cvPoint(250,250), cvSize( (int)(1.5*(1+i*2)),(1+i*2)),0,-180,180,255,-1);
		
		int ellipse_s = (1+i*2) ;
		
		//savetiff(imagetmp,"ellipse.tif");

		blob_list = cvStartFindContours(imagetmp,mem,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
		contour_blob = cvFindNextContour(blob_list);
		contour_blob = cvFindNextContour(blob_list);
		if (contour_blob)
		{
			cout << "\n Small Axis  =  " << ellipse_s << " " << endl ;
			VisBlob titi(contour_blob);
			titi.Calc_Perimeter();
			cout << " Perimeter =  " << titi.GetPerimeter() << endl ;
			cout << " Real Perimeter =  " << (PI*sqrt(2*((1.5*(1+i*2))*(1.5*(1+i*2)) + (1+i*2)*(1+i*2) ))) << endl ;
			cout << " Error on the Perimeter =  " << ((((PI*sqrt(2*((1.5*(1+i*2))*(1.5*(1+i*2)) + (1+i*2)*(1+i*2) ))))/titi.GetPerimeter())*100 -100) << " %" << endl ;
			double err_p = (((PI*sqrt(2*((1.5*(1+i*2))*(1.5*(1+i*2)) + (1+i*2)*(1+i*2) )))/titi.GetPerimeter())*100 -100) ;
			P = titi.GetPerimeter();

			
			titi.Calc_Area();
			cout << " Area =  " << titi.GetArea() << endl ;
			cout << " Real Area =  " << (PI*(1.5*(1+i*2))*(1+i*2)) << endl ;
			cout << " Error on the Area =  " << (((PI*(1.5*(1+i*2))*(1+i*2))/titi.GetArea())*100 -100) << " %" << endl ;
			double err_a = (((PI*(1.5*(1+i*2))*(1+i*2))/titi.GetArea())*100 -100) ;
			A = titi.GetArea();
			 
			ostr << " \t  " << ellipse_s << "\t" << "\t" <<  P  << "\t" << "\t" <<  A  << "  \t  " << "\n";
			
			cout << "--------------------------------------------" << endl << endl ;
		}

		

	}

	ostr.close();


};
void tst7(void) 
{

	ofstream ostr("Cir_per_area.dat") ;
    double A,P;
	CvContourScanner blob_list;
	CvMemStorage* mem    = cvCreateMemStorage();
	CvSeq* contour_blob  = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvContour),mem);
    IplImage* imagetmp   = cvCreateImage(cvSize(500,500),IPL_DEPTH_8U,1);

	ostr << "% \t" << "Radius" << "\t" << "Perimeter" << "\t" << "Area" << "\t" << "\n\n";

	for(int i=0 ; i<80 ; i++ )
	{	
	
		cvCircle(imagetmp,cvPoint(250,250),(1+i*2),255,-1);

		int circle_s = (1+i*2) ;
		
		//savetiff(imagetmp,"ellipse.tif");

		blob_list = cvStartFindContours(imagetmp,mem,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
		contour_blob = cvFindNextContour(blob_list);
		contour_blob = cvFindNextContour(blob_list);
		if (contour_blob)
		{
			cout << "\n Radius  =  " << circle_s << " " << endl ;
			VisBlob titi(contour_blob);
			titi.Calc_Perimeter();
			cout << " Perimeter =  " << titi.GetPerimeter() << endl ;
			P = titi.GetPerimeter();

			
			titi.Calc_Area();
			cout << " Area =  " << titi.GetArea() << endl ;
			A = titi.GetArea();
				
			ostr << "\t " << circle_s << "  \t " <<  P  << "  \t  " <<  A  << "  \t  " << "\n";

			
			cout << "--------------------------------------------" << endl << endl ;
		}

	
	}

	ostr.close();



};
void tst8(void) {};
void tst9(void) {};


int main(void)
  {
  char ans;
  do
    {
    cout << "0: Area test" << endl ;
	cout << "1: Perimeter test" << endl ;
	cout << "2: Compactness test" << endl ;
	cout << "3: Circles test" << endl ;
	cout << "4: Squares test" << endl ;
	cout << "5: Ellipses test" << endl ;
	cout << "6: Ellipses test (Area - Perimeter) " << endl ;
	cout << "7: Circles test (Area - Perimeter) " << endl ;
    cout << "0-9 tst0->tst9" << endl << endl;
    cout << "q quit" << endl << endl;
    cin >> ans;
    switch(ans)
      {
      case '0' : tst0(); break;
      case '1' : tst1(); break;
      case '2' : tst2(); break;
      case '3' : tst3(); break;
      case '4' : tst4(); break;
      case '5' : tst5(); break;
      case '6' : tst6(); break;
      case '7' : tst7(); break;
      case '8' : tst8(); break;
      case '9' : tst9(); break;
      }
    }
  while (ans!='q');
  return 0;
  }
  
#endif  // test routines


/************************** End of File **********************************************************/






