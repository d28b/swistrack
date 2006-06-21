//******************************* DEFINITION MODULE **********************************************/
//
// Author   :   Your name, your e-mail
//              $Author: correll $
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
// Filename :   $RCSfile: polygon.cpp,v $
// Version:     $Revision: 1.2 $
/*************************************************************************************************/

// CVS/SCCS Tag
static char *id="@(#) $Id: polygon.cpp,v 1.2 2005/08/26 16:54:23 correll Exp $";

/******************************** inclusions *****************************************************/
/*************************************************************************************************/
#include "polygon.h" 

#include <math.h>

/*************************************************************************************************/
/************ private types, variables, classes and function prototypes **************************/
/*************************************************************************************************/

/*************************************************************************************************/
/************* public class methods and functions implementations ********************************/
/*************************************************************************************************/

/** \brief Returns a polygon (array of points) with the contour
 *
 * Detailed description of the function
 *
 * \param binimage: A binary image
 * \param polysize: Returns number of points in polygone
 * \result Returns pointer to polygone
 */
CvPoint* GetContourPoly(CvSeq* contour,int* polysize)

{

 CvSeq* poly;
 CvMemStorage* store = cvCreateMemStorage(0);		// memory space that will be used for contour storage			
 CvPoint* p;
 CvPoint* q;
 int i;

 poly = cvApproxPoly(contour,sizeof(CvContour),store,CV_POLY_APPROX_DP,0.1);
 if (!polysize) polysize = (int*) new int;
 *polysize = poly->total;
 if (CV_IS_SEQ_CLOSED( poly ))
 {
  p = (CvPoint*) new CvPoint[*polysize+1];
 }
 else
 {
  p = (CvPoint*) new CvPoint[*polysize];
 }

 for (i=0;i<(*polysize);i++)  
 {
	 q =(CvPoint*) cvGetSeqElem(poly,i); 
	 p[i] = *q;
 }

 if (CV_IS_SEQ_CLOSED( poly )) /* If polygon is close first elem should be equal to last */
 {
  p[(*polysize)++] = p[0];
 }
 return p;
}





/** \brief Returns the compacteness of a contour
 *
 * Compactness is a contour feature defined as 4*pi*Area/Perimeter^2
 * It is maximum (1) for a circle, 0.78 for a square and \f[\frac{\pi\alpha}{(1+\alpha)^2}\f]
 * for a rectangle where on side is \f[(\alpha \leq 1) \f] smaller than the other.
 *
 * \param contour: a CvSeq* containing the contour
 * \result Returns the compactness 
 */

//OPENCVAPI  double  cvContourCompactness( const void* contour)
double  cvContourCompactness( const void* contour)
{
 double l = cvArcLength(contour,CV_WHOLE_SEQ,1);
 return fabs(12.56*cvContourArea(contour)/(l*l));	
}                               


/** \brief Tests if a point is inside a polygon
 *
 * This function is based on the code in 
 * http://www.ecse.rpi.edu/Homepages/wrf/geom/pnpoly.html , based on the Jordan
 * theorem: A ray is thrown from the point, and the number of intersection of
 * the ray with the polygon is counted. If it's an inside point, There will be 
 * an odd number of interesections.
 *
 * \param poly: Polygon given as a CvSeq of CvPoint2D32f
 * \param p: The point to be tested 
 * \result Returns 1 if the point is inside, else 0.
 */

inline int IsPointInsidePoly(CvSeq* poly, CvPoint2D32f p)
  {
    int i, j, c = 0;
	int npol = poly->total;
	CvPoint2D32f* p_i;
    CvPoint2D32f* p_j;
	int added = 0;

	/* If the polygon is defined as closed by the first and last points are not equal
	 * then the first point is copied as last, and it will be erased afterwards */
	if (CV_IS_SEQ_CLOSED( poly ))  
	{
     p_i = (CvPoint2D32f*) cvGetSeqElem(poly,0);
     p_j = (CvPoint2D32f*) cvGetSeqElem(poly,npol);

	 if ((p_i->x != p_j->x) || ((p_i->y != p_j->y))) 
	 {
		 cvSeqPush(poly,(void*) p_i);
		 added++;
	 }
	}

    for (i = 0, j = npol-1; i < npol; j = i++) 
      {
		p_i = (CvPoint2D32f*) cvGetSeqElem(poly,i);
		p_j = (CvPoint2D32f*) cvGetSeqElem(poly,j);
        if ((((p_i->y<=p.y) && (p.y<p_j->y)) ||
             ((p_j->y<=p.y) && (p.y<p_i->y))) &&
	    (p.x < (p_j->x - p_i->x) * (p.y - p_i->y) / (p_j->y - p_i->y) + p_i->x))
            c = !c;
      }

	if (added)
	{
     cvSeqPop(poly); 
	}
    return c;
  }


/** \brief Tests if a point is inside a polygon
 *
 * This function is based on the code in 
 * http://www.ecse.rpi.edu/Homepages/wrf/geom/pnpoly.html , based on the Jordan
 * theorem: A ray is thrown from the point, and the number of intersection of
 * the ray with the polygon is counted. If it's an inside point, There will be 
 * an odd number of interesections.
 *
 * \param poly: Polygon given as a CvSeq of CvPoint
 * \param p: The point to be tested 
 * \result Returns 1 if the point is inside, else 0.
 */

inline int IsPointInsidePoly(CvSeq* poly, CvPoint p)
  {
    int i, j, c = 0;
	int npol = poly->total;
	CvPoint* p_i;
    CvPoint* p_j;
	int added = 0;


	/* If the polygon is defined as closed by the first and last points are not equal
	 * then the first point is copied as last, and it will be erased afterwards */
	if (CV_IS_SEQ_CLOSED( poly ))  
	{
     p_i = (CvPoint*) cvGetSeqElem(poly,0);
	 p_j = (CvPoint*) cvGetSeqElem(poly,npol);

	 if ((p_i->x != p_j->x) || ((p_i->y != p_j->y))) 
	 {
		 cvSeqPush(poly,(void*) p_i);
		 added++;
	 }
	}

    for (i = 0, j = npol-1; i < npol; j = i++) 
      {
		p_i = (CvPoint*) cvGetSeqElem(poly,i);
		p_j = (CvPoint*) cvGetSeqElem(poly,j);
        if ((((p_i->y<=p.y) && (p.y<p_j->y)) ||
             ((p_j->y<=p.y) && (p.y<p_i->y))) &&
	    (p.x < (p_j->x - p_i->x) * (p.y - p_i->y) / (p_j->y - p_i->y) + p_i->x))
            c = !c;
      }

	if (added)
	{
     cvSeqPop(poly); 
	}

    return c;
  }


/*************************************************************************************************/
/************* private class methods and functions implementations *******************************/
/*************************************************************************************************/

/*************************************************************************************************/
/********************** test routines/program ****************************************************/
/*************************************************************************************************/

  
/************************** End of File **********************************************************/







