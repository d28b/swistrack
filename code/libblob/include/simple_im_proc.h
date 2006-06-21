#ifndef SIMPLE_IM_PROC_H        
#define SIMPLE_IM_PROC_H
//****************************************************************************/
// Filename :   $RCSfile: simple_im_proc.h,v $
// Version  :   $Revision: 1.1.1.1 $
//
// Author   :   $Author: yuri $
//              Cyrille LEBOSSE
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
//
//
// Date     :   $Date: 2003/12/17 02:03:48 $
/** \file 
    \brief Simple Image Processing Library used for tests

    Purpose  : Detailed description
*/
// See end of this header file for usage notes
/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/
#include <ipl.h>
#include <cv.h>

/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** types **************************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** prototypes *********************************/
/*****************************************************************************/


void clean_morpho(IplImage *imgS, IplImage *imgD, IplImage *imgtmp );
void binarisation(IplImage *imgS, IplImage *imgD , int threshold=128);

//void testblobsearch(IplImage *imgS,double *taille_blob, CvPoint* pt_centre );
//void DrawContour(IplImage *imgS, CvSeq *contour_blob,int greyLevel=255);
 

/*****************************************************************************/
/******************************** usage notes ********************************/
/*****************************************************************************/
/*

*/
#endif
