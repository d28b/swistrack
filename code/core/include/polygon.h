#ifndef polygon_h        
#define polygon_h
//****************************************************************************/
// Filename :   $RCSfile: polygon.h,v $
// Version  :   $Revision: 1.2 $
//
// Author   :   $Author: correll $
//              Your name
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
//
//
// Date     :   $Date: 2005/08/26 16:54:23 $
/** \file polygon.h
    \brief Contour and polygon manipulation functions

    Purpose  : Contains functions for polygon manipulation and analysis
*/
// See end of this header file for usage notes
/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/


#include "cv.h"

#ifdef _cplusplus
extern "C" {
#endif


/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** types **************************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** prototypes *********************************/
/*****************************************************************************/


CvPoint* GetContourPoly(CvSeq* contour,int* polysize);
//OPENCVAPI double cvContourCompactness( const void* contour); /** Compute Compactness of contour */
double cvContourCompactness( const void* contour); /** Compute Compactness of contour */
inline int IsPointInsidePoly(CvSeq* poly, CvPoint2D32f p); /** Tests if a point is inside a polygon */
inline int IsPointInsidePoly(CvSeq* poly, CvPoint p); /** Tests if a point is inside a polygon */


/*****************************************************************************/
/******************************** usage notes ********************************/
/*****************************************************************************/
/*

*/

#ifdef _cplusplus
}
#endif

#endif
