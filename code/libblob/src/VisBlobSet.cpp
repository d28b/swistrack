//******************************* DEFINITION MODULE **********************************************/
//
// Author   :   Cyrille Lebossé, cyrille.lebosse@voila.fr
//              $Author: yuri $
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
// Filename :   $RCSfile: VisBlobSet.cpp,v $
// Version:     $Revision 1.0 $
/*************************************************************************************************/

// CVS/SCCS Tag
static char *id="@(#) $Id: VisBlobSet.cpp,v 1.2 2004/03/24 13:23:39 yuri Exp $";


/*! \file VisBlobSet.cpp
 *  \brief This files contains the definition of VisBlobSet class methods.
 *         These methods enable to find blobs in binarised image, to sort 
 *         the created list and to calculate some of their features.
 * 		
 * 		The binarised image can be defined with a Region Of Interest(ROI).
 * 		It is possible indeed to select some rectangular part of the image or a certain color plane in the
 * 		image, and process only this part. The selected rectangle is called "Region of
 * 		Interest". The structure "IplImage" contains the field roi for this purpose. If the
 * 		pointer is not NULL, it points to the structure "IplROI" that contains parameters of selected
 * 		ROI, otherwise a whole image is considered selected.
 *		If a ROI is defined, blobs are only searched in the ROI and, the blob center coordinates 
 *		are reajusted so that they are expressed in the	global image reference frame.
 */

/******************************** inclusions *****************************************************/
/*************************************************************************************************/
#include "VisBlobSet.hpp" 



#include <iostream>
#include <stdio.h>

/*************************************************************************************************/
/************ private types, variables, classes and function prototypes **************************/
/*************************************************************************************************/

/*************************************************************************************************/
/************* public class methods and functions implementations ********************************/
/*************************************************************************************************/


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

//*******************************************************************************
//-------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
/** Constructor */	
VisBlobSet :: VisBlobSet()
{ 
	mem=NULL;
}	

//--------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
/** \brief Destructor 
 *
 *         Used to clear the lists and to free memory.
 */
VisBlobSet :: ~VisBlobSet()
{ 
	blob_seq.clear();
	blob_temp.clear();
	if(mem != NULL)
	{
		cvReleaseMemStorage (&mem);
	}
}	

//--------------------------------------------------------------------------------
//********************************************************************************
//  FindBlobs
//********************************************************************************
/*! \brief Blobs searching in a binarised image.The image can be defined with 
 *	a Region Of Interest (ROI) and so the function search blobs only in the ROI.
 *		
 *		It is possible indeed to select some rectangular part of the image or a certain color plane in the
 *		image, and process only this part. The selected rectangle is called "Region of
 *		Interest". The structure "IplImage" contains the field roi for this purpose. If the
 *		pointer is not NULL, it points to the structure "IplROI" that contains parameters of selected
 *		ROI, otherwise a whole image is considered selected.
 *
 *		If a ROI is defined, blobs are only searched in the ROI.
 *
 *	The blob center coordinates are reajusted so that they are expressed in the
 *	global image reference frame.
 * 
 *	If this function is called repeatedly with different images but with the same BlobSet, the newly detected
 *	Blobs are added to the detected blobs list.
 *
 *	\param imgS: source image (binarised image)
 *	\return Returns the number of detected blobs.
 */

int VisBlobSet :: FindBlobs(IplImage* imgS)
{

	int retval = -1;

	IplImage *imagetmp;

	CV_FUNCNAME( "FindBlobs" );
	__BEGIN__;

	bool last_blob = false;
	int j=0;
 
    CvSeq* contour_blob;
	CvContourScanner blob_list;
	
	if( blob_temp.size() > 0 )
	{
		// noting, don't have to create a memory bloc 
    }
	else
	{
		mem = cvCreateMemStorage();//firt time!
	}

	if (!mem) 
    {
      CV_ERROR(CV_StsError, "\n Error creating memory storage mem \n");
	
    }

	contour_blob = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvContour),mem);

	if (!contour_blob) 
    {
      CV_ERROR(CV_StsError, "\n Error creating CvSeq* contour_blob \n");
    }



	if (!imgS)
    {
      CV_ERROR(CV_StsBadArg, "\n IplImage* imgS is empty \n");
    }

	if ( imgS->nChannels != 1 )
    {
      CV_ERROR(CV_StsBadArg, "\n IplImage* is not a grayscale image \n" );
    }


	if( imgS->roi == 0 )
	{
		//Temporary images
		imagetmp = cvCreateImage(cvSize(imgS->width,imgS->height),imgS->depth,imgS->nChannels); //iplCloneImage(imgS);
		cvCopyImage(imgS,imagetmp); // Because cvStartFindContours destroys the image given as a parameter
	}
	else
	{
		//Temporary images
		imagetmp = cvCreateImage(cvSize( (imgS->roi)->width,(imgS->roi)->height),imgS->depth,imgS->nChannels); //iplCloneImage(imgS);
		cvCopyImage(imgS,imagetmp); // Because cvStartFindContours destroys the image given as a parameter
	}


	//Contoursearch
	blob_list = cvStartFindContours(imagetmp,mem,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
	

	for(j=0 ; last_blob!=true ; j++)
	{
		contour_blob = cvFindNextContour(blob_list);


		if(contour_blob==NULL)
		{
			last_blob = true;//if last contour -> leave scan
			
			if(j==0)
			{ // no contour 
				retval = 0;
				
			}//end_if_no_contour
			
			break;

		}//end_if_last_contour
		else
		{
			VisBlob nblob(contour_blob, imgS->roi);

			nblob.Calc_Area();

			if( nblob.GetArea() > 0 )
			{
				blob_seq.push_back(nblob);
			}
		}

	}//end_for

	SelectAll();
	
    retval = blob_temp.size();

	cvReleaseImage (&imagetmp);


	__END__;

    return retval;
}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  GetSelectedNumBlobs
//********************************************************************************
/** \brief Returns the number of selected blobs.This function returns the
 *		number of selected blobs which is the size of the vector containing
 *		selected blobs.
 *
 *  \return Returns the number of blobs in the selected blobs list.
 */

int VisBlobSet :: GetSelectedNumBlobs(void)
{
	CV_FUNCNAME( "GetSelectedNumBlobs" );
	__BEGIN__;


	return( blob_temp.size() );


	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  GetTotalNumBlobs
//********************************************************************************
/** \brief Returns the total number of blobs detected.
 *
 *	 This function returns
 *	 the number of detected blobs which is the size of the vector containing
 *	 detected blobs.
 *
 *  \return Returns the total number of blobs detected.
 */

int VisBlobSet :: GetTotalNumBlobs(void)
{
	CV_FUNCNAME( "GetTotalNumBlobs" );
	__BEGIN__;


	return( blob_seq.size() );

	
	__END__;
}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateFeatures
//********************************************************************************
/*! \brief Calculates features of the selected blobs. 
 *			By defect, all the detected blobs are selected.
 *
 *	 Area, Perimeter, Compactness, Center, Orientation, MinimumEnclosingCircle, MinimumAreaRectangle.  
 *	
 *		This function calculates all the defined
 *		blob features by calling the adapted methods.
 *
 *  \return Returns 0 if calculation is possible or -1 if there is no blob.
 *  \sa Calc_Area(), Calc_Perimeter(), Calc_Centre(), Calc_Compactness(), Calc_Orientation();
 *		Calc_MinEncloCircle(), Calc_EquivalentRect().
 */

int VisBlobSet :: CalculateFeatures(void)
{

	CV_FUNCNAME( "CalculateFeatures" );
	__BEGIN__;

   if( blob_temp.size() > 0 )
   {
		for(unsigned int i=0; i<blob_temp.size(); i++)
		{
			blob_temp[i].Calc_Area();
			blob_temp[i].Calc_Perimeter();
			blob_temp[i].Calc_Centre();
			blob_temp[i].Calc_Compactness();
			blob_temp[i].Calc_Orientation();
			blob_temp[i].Calc_MinEncloCircle();
			blob_temp[i].Calc_EquivalentRect();
			blob_temp[i].Calc_Eccentricity();
			blob_temp[i].Calc_EllipseMajorAxisLength();
			blob_temp[i].Calc_EllipseMinorAxisLength();
			blob_temp[i].Calc_MajorAxis();
			blob_temp[i].Calc_MinorAxis();
			blob_temp[i].Calc_BoundingRectangle();
			blob_temp[i].Calc_EllipseSim();
			blob_temp[i].Calc_MinorAxisLength();
			blob_temp[i].Calc_MajorAxisLength();
		}//end_for
	
		return 0;
   }
   else
   {
		return -1;
   }

	
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateAreas
//********************************************************************************
/*! \brief Calculates blobs area (total number of pixels).
 *
 *	Calculates area of the selected blobs (By defect, all the detected blobs are selected)
 *
 *	\return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateAreas(void)
{

	CV_FUNCNAME( "CalculateAreas" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {

		for(unsigned int i=0; i<blob_temp.size(); i++)
		{
			blob_temp[i].Calc_Area();
		}//end_for

		return 0;
	}
	else
	{
		return -1;
	}
	
	
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculatePerimeters
//********************************************************************************
/** \brief Calculates blobs perimeter (number of pixels).
 *
 *	Calculates perimeter of the selected blobs (By defect, all the detected blobs are selected)
 *
 *	\return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculatePerimeters(void)
{

	CV_FUNCNAME( "CalculatePerimeters" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {
		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_Perimeter();
		}//end_for
		
		return 0;
	}
	else
	{
		return -1;
	}
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateCompactness
//********************************************************************************
/** \brief Calculates blobs compactness.
 *
 *	Calculates compactness of the selected blobs (By defect, all the detected blobs are selected)
 *
 *  This method calculates the blob compactness by using OpenCV functions. 
 *  Compactness is a contour feature defined as 
 *	\f[ \frac{4*pi*Area}{Perimeter^2} \f]
 *
 *  It is maximum (1) for a circle, less but superior to 0 for other shapes.
 *  
 *	If the blob perimeter is less than 50, then the error on the compactness is
 *	bigger than 10%.
 *
 *	\return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateCompactness(void)
{

	CV_FUNCNAME( "CalculateCompactness" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {
		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_Compactness();
		}//end_for

		return 0;
	}
	else
	{
		return -1;
	}

	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateEllipseSimFacts
//********************************************************************************
/** \brief Calculates the ellipse similarity factor of blobs.
 *
 *	Calculates the ellipse similarity factor of the selected blobs (By defect, all the detected blobs are selected)
 *
 *
 *	The factor is defined as : \f[  \frac{Area}{MinEnclosingCircleRadius^2} \f]
 *  
 *	\return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateEllipseSimFacts(void)
{

	CV_FUNCNAME( "CalculateEllipseSimFacts" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {
		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_EllipseSim();
		}//end_for

		return 0;
	}
	else
	{
		return -1;
	}

	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateEccentricities
//********************************************************************************
/** \brief Calculates blobs eccentricity.
 *
 *	Calculates eccentricity of the selected blobs (By defect, all the detected blobs are selected)
 *
 *
 *  This method calculates the blob eccentricity by using moments. 
 *  Eccentricity is a contour feature which shows elongation. 
 *  It is maximum (1) for a line, minimum (0) for a disc.
 *  
 *	\return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateEccentricities(void)
{

	CV_FUNCNAME( "CalculateEccentricities" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {
		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_Eccentricity();
		}//end_for

		return 0;
	}
	else
	{
		return -1;
	}

	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateEllipseMajorAxisLengths
//********************************************************************************
/** \brief Calculates the major axis length of the "equivalent" ellipse of the blob.
 *
 *  This function finds the "equivalent" ellipse of the blob and 
 *	calculates the length of its major axis. "Equivalent" means
 *  that the ellipse has the same area, orientation and inertia as the blob.
 *	Thus, it has the same MajorAxis as the blob but, warning, its length is different.
 *	The advantage is that it is less sensitive to noise than the real blob major axis length. 
 *  
 *
 *	\return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateEllipseMajorAxisLengths(void)
{

	CV_FUNCNAME( "CalculateEllipseMajorAxisLengths" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {
		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_EllipseMajorAxisLength();
		}//end_for

		return 0;
	}
	else
	{
		return -1;
	}

	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateEllipseMinorAxisLengths
//********************************************************************************
/** \brief Calculates the minor axis length of the "equivalent" ellipse of the blob.
 *
 *  This function finds the "equivalent" ellipse of the blob and 
 *	caculates the length of its minor axis. "Equivalent" means
 *  that the ellipse has the same area, orientation and inertia as the blob.
 *	Thus, it has the same MinorAxis as the blob but, warning, its length is different.
 *	The advantage is that it is less sensitive to noise than the real blob minor axis length.   
 *  
 *
 *	\return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateEllipseMinorAxisLengths(void)
{

	CV_FUNCNAME( "CalculateEllipseMinorAxisLengths" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {
		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_EllipseMinorAxisLength();
		}//end_for

		return 0;
	}
	else
	{
		return -1;
	}

	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateMajorAxis
//********************************************************************************
/** \brief Calculates blobs major axis.
 *
 *
 *	\return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateMajorAxis(void)
{

	CV_FUNCNAME( "CalculateMajorAxis" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {

		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_MajorAxis();
		}//end_for
	
		return 0;
	}
	else
	{
		return -1;
	}
	
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateMajorAxisLength
//********************************************************************************
/** \brief Calculates blobs major axis length.
 *
 *
 *	\return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateMajorAxisLength(void)
{

	CV_FUNCNAME( "CalculateMajorAxisLength" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {

		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_MajorAxisLength();
		}//end_for
	
		return 0;
	}
	else
	{
		return -1;
	}
	
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateMinorAxis
//********************************************************************************
/** \brief Calculates blobs minor axis.
 *
 *
 *	\return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateMinorAxis(void)
{

	CV_FUNCNAME( "CalculateMinorAxis" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {

		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_MinorAxis();
		}//end_for
	
		return 0;
	}
	else
	{
		return -1;
	}
	
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateMinorAxisLength
//********************************************************************************
/** \brief Calculates blobs minor axis length.
 *
 *
 *	\return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateMinorAxisLength(void)
{

	CV_FUNCNAME( "CalculateMinorAxisLength" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {

		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_MinorAxisLength();
		}//end_for
	
		return 0;
	}
	else
	{
		return -1;
	}
	
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateCenters
//********************************************************************************
/** \brief Calculates blobs center
 *
 *  Precision of 1 pixel but subpixel precision for "CenterReal".
 *  
 *  If a ROI is defined, blob center coordinates are reajusted so that they are expressed in the
 *	global image reference frame.
 *
 *	\return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateCenters(void)
{

	CV_FUNCNAME( "CalculateCenters" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {

		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_Centre();
		}//end_for
	
		return 0;
	}
	else
	{
		return -1;
	}
	
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateOrientations
//********************************************************************************
/** \brief Calculates blobs orientation (angle in degree compared to the horizontal axis).
 *
 *  \return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateOrientations(void)
{

	CV_FUNCNAME( "CalculateOrientations" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {

		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_Orientation();
		}//end_for
	
		return 0;
	}
	else
	{
		return -1;
	}
	
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateMinimumEnclosingCircle
//********************************************************************************
/** \brief Calculates the minimum enclosing circle of the detected blobs.
 *
 *  \return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateMinimumEnclosingCircle(void)
{

	CV_FUNCNAME( "CalculateMinimumEnclosingCircle" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {

		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_MinEncloCircle();
		}//end_for
	
		return 0;
	}
	else
	{
		return -1;
	}
	
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateBoundingRectangle
//********************************************************************************
/** \brief Calculates the bounding rectangle of the detected blobs.
 *
 *  This function finds the bounding rectangle for the blob. "Bounding" means
 *  that all the points from the blob are either inside or on the boundary of the
 *  rectangle. The rectangle is horizontal that is why it is not the minimum
 *	enclosing rectangle wchich would have the same orientation as the blob.
 *
 *  \return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateBoundingRectangle(void)
{

	CV_FUNCNAME( "CalculateBoundingRectangle" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {

		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_BoundingRectangle();
		}//end_for
	
		return 0;
	}
	else
	{
		return -1;
	}
	
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  CalculateEquivalentRectangle
//********************************************************************************
/** \brief Calculates the "equivalent" rectangle for the blob.
 *
 *  This function finds the "equivalent" rectangle of the blob. "Equivalent" means
 *  that the rectangle has the same area, orientation and inertia as the blob.
 *	Thus, it has the same MajorAxis as the blob but, warning, its length is different.
 *	The advantage is that it is less sensitive to noise than the real blob major axis length.
 *
 *  \return Returns 0 if calculation is possible or -1 if there is no blob.
 */

int VisBlobSet :: CalculateEquivalentRectangle(void)
{

	CV_FUNCNAME( "CalculateEquivalentRectangle" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
    {

		for(unsigned int i=0; i<blob_temp.size() ; i++)
		{
			blob_temp[i].Calc_EquivalentRect();
		}//end_for
	
		return 0;
	}
	else
	{
		return -1;
	}
	
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  Sort
//********************************************************************************
/** \brief Blobs sorting (private method)
 *
 *	 The function "std::sort" is used to sort blobs. It sorts thanks to
 *	 the operator "<". That's why here its action has been reversed. So
 *	 blobs are sorted in descending order. This sort is based on the sortkey
 *	 value. 
 *
 *	\return Returns 0 if sorting is possible or -1 if there is no blob.
 */

int VisBlobSet :: Sort(void)
{

	CV_FUNCNAME( "Sort" );
	__BEGIN__;


	if( blob_temp.size() > 0 )
	{
 
		std::sort(blob_temp.begin(),blob_temp.end());
		/** sorting in a descending order
		 * That's why the operator "<" is reversed.
		 */
		return 0;

	}
	else
	{
		// No Blob found
		return(-1);	
	}

		
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  SortByArea
//********************************************************************************
/** \brief  Sorts blobs in a descending order thanks to their area or
 *			 considering the distance between BlobArea and the given area.
 *
 *	Distance : \f[ |BlobArea-area| \f]
 *
 *	\return Returns 0 if sorting is possible or -1 if there is no blob.
 */
//	For each blob,this function calculates area if it has not been done, stores
//	the BlobArea value in the sortkey if  \f[ area=0 \f]  or stores the distance
//	\f[ |BlobArea-area| \f] 
//	in the sortkey if \f[ area \neq 0 \f] and then calls the private "Sort" method.

int VisBlobSet :: SortByArea(double area)
{
	
    unsigned int i; 

	CV_FUNCNAME( "SortByArea" );
	__BEGIN__;


	
	if( blob_temp[0].GetArea() == 0 )
	{
		CalculateAreas();
	}
 
	if( area == 0 )
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{
			blob_temp[i].SetKey(blob_temp[i].GetArea());
		}
	}
	else
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{		
			blob_temp[i].SetKey(-fabs( (blob_temp[i].GetArea()-area ) ) );	
		}
	}

	return( Sort() );	

				
	__END__;
	
}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SortByPerimeter
//********************************************************************************
/*! \brief Sorts blobs in a descending order thanks to their perimeter or
 *			considering the distance between BlobPerimeter and the given perimeter.
 *
 *	Distance : \f[ |BlobPerimeter-perimeter| \f] 
 *
 *	\return Returns 0 if sorting is possible or -1 if there is no blob.
 */
//	For each blob,this function calculates perimeter if it has not been done, stores
//	the BlobPerimeter value in the sortkey if  \f[ perimeter=0 \f]  or stores the distance
//	\f[ |BlobPerimeter-perimeter| \f] 
//	in the sortkey if \f[ perimeter \neq 0 \f] and then calls the private "Sort" method.

int VisBlobSet :: SortByPerimeter(double perimeter)
{

    unsigned int i;
 	
	CV_FUNCNAME( "SortByPerimeter" );
	__BEGIN__;



	if( blob_temp[0].GetPerimeter() == 0 )
	{
		CalculatePerimeters();
	}

	
	if( perimeter == 0 )
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{
			blob_temp[i].SetKey(blob_temp[i].GetPerimeter());
		}
	}
	else
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{		
			blob_temp[i].SetKey(-fabs( (blob_temp[i].GetPerimeter()-perimeter ) ) );	
		}
	}

	return( Sort() );	

				
	__END__;
}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SortByCompactness
//********************************************************************************
/*! \brief Sorts blobs in a descending order thanks to their compactness or
 *			considering the distance between BlobCompactness and the given compactness.
 *
 *	Distance : \f[ |BlobCompactness-compactness| \f] 
 *
 *	\return Returns 0 if sorting is possible or -1 if there is no blob.
 */
//	For each blob,this function calculates compactness if it has not been done, stores
//	the BlobCompactness value in the sortkey if  \f[ compactness=0 \f]  or stores the distance
//	\f[ |BlobCompactness-compactness| \f] 
//	in the sortkey if \f[ compactness \neq 0 \f] and then calls the private "Sort" method.
int VisBlobSet :: SortByCompactness(double compactness)
{
	
    unsigned int i; 

	CV_FUNCNAME( "SortByCompactness" );
	__BEGIN__;



	if( blob_temp[0].GetCompactness() == 0 )
	{
		CalculateCompactness();
	}
 
	if( compactness == 0 )
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{
			blob_temp[i].SetKey(blob_temp[i].GetCompactness());
		}
	}
	else
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{		
			blob_temp[i].SetKey(-fabs( (blob_temp[i].GetCompactness()-compactness )) );	
		}
	}

	return( Sort() );

					
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SortByEccentricity
//********************************************************************************
/*! \brief Sorts blobs in a descending order thanks to their eccentricity or
 *			considering the distance between BlobEccentricity and the given eccentricity.
 *
 *	Distance : \f[ |BlobEccentricity-eccentricity| \f] 
 *
 *	\return Returns 0 if sorting is possible or -1 if there is no blob.
 */
//	For each blob,this function calculates eccentricity if it has not been done, stores
//	the BlobEccentricity value in the sortkey if  \f[ eccentricity=0 \f]  or stores the distance
//	\f[ |BlobEccentricity-eccentricity| \f] 
//	in the sortkey if \f[ eccentricity \neq 0 \f] and then calls the private "Sort" method.
int VisBlobSet :: SortByEccentricity(double eccentricity)
{

    unsigned int i; 
	
	CV_FUNCNAME( "SortByEccentricity" );
	__BEGIN__;



	if( blob_temp[0].GetEccentricity() == 0 )
	{
		CalculateEccentricities();
	}
 
	if( eccentricity == 0 )
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{
			blob_temp[i].SetKey(blob_temp[i].GetEccentricity());
		}
	}
	else
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{		
			blob_temp[i].SetKey(-fabs( (blob_temp[i].GetEccentricity()-eccentricity )) );	
		}
	}

	return( Sort() );

					
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SortByEllipseMajorAxisLength
//********************************************************************************
/*! \brief Sorts blobs in a descending order thanks to their "equivalent" ellipse
 *			major axis length or considering the distance between
 *			BlobEllipseMajorAxisLength and the given EllipseMajorAxisLength.
 *
 *	Distance : \f[ |BlobEllipseMajorAxisLength-EllipseMajorAxisLength| \f] 
 *
 *	\return Returns 0 if sorting is possible or -1 if there is no blob.
 */
//	For each blob,this function calculates Major Axis Length if it has not been done, stores
//	the BlobEllipseMajorAxisLength value in the sortkey if  \f[ eccentricity=0 \f]  or stores the distance
//	\f[ |BlobEllipseMajorAxisLength-EllipseMajorAxisLength| \f] 
//	in the sortkey if \f[ EllipseMajorAxisLength \neq 0 \f] and then calls the private "Sort" method.
int VisBlobSet :: SortByEllipseMajorAxisLength(double EllipseMajorAxisLength)
{
	
    unsigned int i; 

	CV_FUNCNAME( "SortByEllipseMajorAxisLength" );
	__BEGIN__;



	if( blob_temp[0].GetEllipseMajorAxisLength() == 0 )
	{
		CalculateEllipseMajorAxisLengths();
	}
 
	if( EllipseMajorAxisLength == 0 )
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{
			blob_temp[i].SetKey(blob_temp[i].GetEllipseMajorAxisLength());
		}
	}
	else
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{		
			blob_temp[i].SetKey(-fabs( (blob_temp[i].GetEllipseMajorAxisLength()-EllipseMajorAxisLength )) );	
		}
	}

	return( Sort() );

					
	__END__;


}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SortByEllipseMinorAxisLength
//********************************************************************************
/*! \brief Sorts blobs in a descending order thanks to their "equivalent" ellipse
 *			 minor axis length or considering the distance between
 *			BlobEllipseMinorAxisLength and the given EllipseMainrAxisLength.
 *
 *	Distance : \f[ |BlobEllipseMinorAxisLength-EllipseMinorAxisLength| \f] 
 *
 *	\return Returns 0 if sorting is possible or -1 if there is no blob.
 */
//	For each blob,this function calculates Major Axis Length if it has not been done, stores
//	the BlobEllipseMinorAxisLength value in the sortkey if  \f[ EllipseMinorAxisLength=0 \f]  or stores the distance
//	\f[ |BlobEllipseMinorAxisLength-EllipseMinorAxisLength| \f] 
//	in the sortkey if \f[ EllipseMinorAxisLength \neq 0 \f] and then calls the private "Sort" method.
int VisBlobSet :: SortByEllipseMinorAxisLength(double EllipseMinorAxisLength)
{
	
    unsigned int i;

	CV_FUNCNAME( "SortByEllipseMinorAxisLength" );
	__BEGIN__;



	if( blob_temp[0].GetEllipseMinorAxisLength() == 0 )
	{
		CalculateEllipseMinorAxisLengths();
	}
 
	if( EllipseMinorAxisLength == 0 )
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{
			blob_temp[i].SetKey(blob_temp[i].GetEllipseMinorAxisLength());
		}
	}
	else
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{		
			blob_temp[i].SetKey(-fabs( (blob_temp[i].GetEllipseMinorAxisLength()-EllipseMinorAxisLength )) );	
		}
	}

	return( Sort() );

					
	__END__;


}

//********************************************************************************
//--------------------------------------------------------------------------------




//--------------------------------------------------------------------------------
//********************************************************************************
//  SortByOrientation
//********************************************************************************
/*! \brief Sorts blobs in a descending order thanks to their orientation or
 *			considering the distance between BlobOrientation and the given orientation.
 *
 *	Distance : \f[ |BlobOrientation-orientation| \f]
 *
 *	\return Returns 0 if sorting is possible or -1 if there is no blob.
 */
//	For each blob,this function calculates orientation if it has not been done, stores
//	the BlobOrientation value in the sortkey if  \f[ orientation=0 \f]  or stores the distance
//	\f[ |BlobOrientation-orientation| \f] 
//	in the sortkey if \f[ orientation \neq 0 \f] and then calls the private "Sort" method.

int VisBlobSet :: SortByOrientation(double orientation)
{
 	
    unsigned int i; 

	CV_FUNCNAME( "SortByOrientation" );
	__BEGIN__;



	if( blob_temp[0].GetOrientation() == 0 )
	{
		CalculateOrientations();
	}

	
	if( orientation == 0 )
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{
			blob_temp[i].SetKey(blob_temp[i].GetOrientation());
		}
	}
	else
	{
		for( i=0 ; i<blob_temp.size() ; i++ )
		{		
			blob_temp[i].SetKey(-fabs( (blob_temp[i].GetOrientation()-orientation )) );	
		}
	}

	return( Sort() );	

				
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SortByPosition
//********************************************************************************
/*! \brief  Sorts blobs in a descending order considering the distance 
 *			between blob center and the given CvPoint2D32f.
 *		
 *	Distance: \f[ \sqrt{ (RealBlobCenter.x-point.x)^2 + (RealBlobCenter.y-point.y)^2 } \f]
 *
 *	\return Returns 0 if sorting is possible or -1 if there is no blob.
 */
//	For each blob,this function calculates blob center if it has not been done, stores
//	the distance \f[ \sqrt{ (RealBlobCenter.x-point.x)^2 + (RealBlobCenter.y-point.y)^2 } \f] 
//	in the sortkey and then calls the private "Sort" method.

int VisBlobSet :: SortByPosition(CvPoint2D32f point)
{
 	
	int retval=-1;

	CV_FUNCNAME( "SortByPosition" );
	__BEGIN__;

	unsigned int i=0;

	if (!(&point))
    {
      CV_ERROR(CV_StsBadArg, "\n CvPoint2D32f point is empty \n");
    }


	if( (blob_temp[0].GetCentreReal()).x == 0.0 )
	{
		CalculateCenters();
	}

	
	for( i=0 ; i<blob_temp.size() ; i++ )
	{
		blob_temp[i].SetKey(-sqrt( ( (blob_temp[i].GetCentreReal()).x-point.x )*( (blob_temp[i].GetCentreReal()).x-point.x ) + ( (blob_temp[i].GetCentreReal()).y-point.y )*((blob_temp[i].GetCentreReal()).y-point.y )) );	
	}

	retval = Sort();	

				
	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  BlobFitting
//********************************************************************************
/*! \brief Sorts blobs in a descending order according to their similarity 
 *			with the first selected blob of the given BlobSet.
 *
 *	The similarity is based on the blob compactness, eccentricity and ellipse
 *	similarity factor. It is invariant to scale and orientation.
 *
 *	The similarity can also be based on the blob area (variant to scale), but 
 *	the area variance must be given as a parameter. If you don't know which
 *	variance choose, you can give the following variance:
 *		\f[  \frac{ImageWidth*ImageHeight}{12}  \f]
 *
 *	\param blobset : Given BlobSet whose first selected blob is taken as reference.
 *	\param area_variance : Area variance of blobs.
 *	\return Returns 0 if sorting is possible or -1 if there is no blob.
 */
int VisBlobSet :: BlobFitting(VisBlobSet* pblobset, double area_variance)
{

	int retval = -1;

	CV_FUNCNAME( "BlobFitting" );
	__BEGIN__;

	int k=0;
	int i=0;

	if( area_variance > 0 )
	{ 
		k=1;
	} // area is taken into account

	if ( pblobset == NULL || area_variance < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad area variance or bad blobset \n");
    }

	if( (blob_temp[0].GetCompactness() == 0) || (blob_temp[0].GetEccentricity() == 0) || (blob_temp[0].GetEllipseSim() == 0)  )
	{
		CalculateCompactness();
		CalculateEccentricities();
		CalculateEllipseSimFacts();
	}

	if( (pblobset->GetCompactness(0) == 0) || (pblobset->GetEccentricity(0) == 0) || (pblobset->GetEllipseSim(0) == 0)  )
	{
		pblobset->CalculateCompactness();
		pblobset->CalculateEccentricities();
		pblobset->CalculateEllipseSimFacts();	
	}

	for( i=0 ; i<(int)blob_temp.size() ; i++ )
	{		
		blob_temp[i].SetKey(-sqrt( (blob_temp[i].GetCompactness()-pblobset->GetCompactness(0))*(blob_temp[i].GetCompactness()-pblobset->GetCompactness(0)) \
			+ (blob_temp[i].GetEccentricity()-pblobset->GetEccentricity(0))*(blob_temp[i].GetEccentricity()-pblobset->GetEccentricity(0)) \
			+ (blob_temp[i].GetEllipseSim()-pblobset->GetEllipseSim(0))*(blob_temp[i].GetEllipseSim()-pblobset->GetEllipseSim(0)) \
			+ k*(blob_temp[i].GetArea()/area_variance-pblobset->GetArea(0)/area_variance)*(blob_temp[i].GetArea()/area_variance-pblobset->GetArea(0)/area_variance) ) );	
	}

	retval =  Sort();	
				
	__END__;

	return retval;
}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  Select
//*********************************************************************************
/** \brief Selects the "blobnum"th blob of the setected blobs list.
 *
 *	After having called this function, there is only one blob in the selected blobs list.
 *	This blob is the "blobnum"th blob of the previously setected blobs list.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).	
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns 0 if selection is possible or -1 if there is no blob before selection.
 */

int VisBlobSet :: Select(int blobnum )
{
	
	CV_FUNCNAME( "Select" );
	__BEGIN__;


	vector<VisBlob>::iterator temp;
	VisBlob tmpblob;
	int i=0, num;
	num = blob_temp.size();

	if( blob_temp.size() > 0 )
	{

		if( blob_temp[0].GetArea() == 0 )
		{
			CalculateFeatures();
		}


		for( i=0 ; i<blobnum ; i++  )
		{
			temp = blob_temp.begin();
			blob_temp.erase(temp);
			num--;
		}

		num--;
		
		for( i=0 ; i<num ; i++  )
		{
			temp = blob_temp.begin();
			temp++;
			blob_temp.erase(temp);
		}

		return 0;

	}
	else
	{	
		// no blob
		return -1;
	}

						
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  Erase
//*********************************************************************************
/** \brief Erases the "blobnum"th blob of the setected blobs list.
 *
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).	
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns 0 if selection is possible or -1 if there is no blob before selection.
 */

int VisBlobSet :: Erase(int blobnum )
{
	
	CV_FUNCNAME( "Erase" );
	__BEGIN__;


	vector<VisBlob>::iterator temp;
	VisBlob tmpblob;
	int i=0;

	if( blob_temp.size() > 0 )
	{

		temp = blob_temp.begin();
		for( i=0 ; i<blobnum ; i++  )
		{	
			temp++;
		}
	
		blob_temp.erase(temp);

		return 0;

	}
	else
	{	
		// no blob
		return -1;
	}

						
	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SelectByArea
//*********************************************************************************
/** \brief Selects blobs whose area is within the given limits.
 *
 *	This function delete blobs from the selected blobs (temporary) list whose area is not
 *	within the given limits.
 *
 *  \param  max: maximum area value
 *  \param  min: minimum area value
 *	\return Returns 0 if selection is possible or -1 if there is no blob before selection.
 */

int VisBlobSet :: SelectByArea( double max, double min )
{
	
	int retval = -1;

	CV_FUNCNAME( "SelectByArea" );
	__BEGIN__;


	vector<VisBlob>::iterator temp;

	if ( max < 0  || min < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad limits : max < 0 or min < 0 \n");
    }


	if( blob_temp.size() > 0 )
	{

		if( blob_temp[0].GetArea() == 0 )
		{
			CalculateAreas();
		}

		for( temp = blob_temp.begin() ; temp != blob_temp.end() ;  )
		{
			if( temp->GetArea() < min  || temp->GetArea()  >  max )
			{
				blob_temp.erase(temp);
				temp = blob_temp.begin();
			}
			else
			{
				temp++;
			}

		}//end_for

		retval = 0;

	}
	else
	{	
		// no blob
		retval = -1;
	}

						
	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SelectByPerimeter
//********************************************************************************
/** \brief Selects blobs whose perimeter is within the given limits.
 *
 *	This function delete blobs from the selected blobs (temporary) list whose perimeter is not
 *	within the given limits.
 *
 *  \param  max: maximum perimeter value
 *  \param  min: minimum perimeter value
 *	\return Returns 0 if selection is possible or -1 if there is no blob before selection.
 */

int VisBlobSet :: SelectByPerimeter( double max, double min )
{

	int retval = -1;

	CV_FUNCNAME( "SelectByPerimeter" );
	__BEGIN__;


	vector<VisBlob>::iterator temp;

	if ( max < 0  || min < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad limits : max < 0 or min < 0 \n");
    }


	if( blob_temp.size() > 0 )
	{

		if( blob_temp[0].GetPerimeter() == 0 )
		{
			CalculatePerimeters();
		}

		for( temp = blob_temp.begin() ; temp != blob_temp.end() ; )
		{
				if( (*temp).GetPerimeter() < min  || (*temp).GetPerimeter()  >  max )
				{
					blob_temp.erase(temp);
					temp = blob_temp.begin();
				}
				else
				{
					temp++;
				}
		}

		retval = 0;

	}
	else
	{
		// no blob
		retval = -1;
	}

	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  SelectByCompactness
//********************************************************************************
/** \brief Selects blobs whose compactness is within the given limits.
 *
 *	This function delete blobs from the selected blobs (temporary) list whose compactness is not
 *	within the given limits.
 *
 *  \param  max: maximum compactness value
 *  \param  min: minimum compactness value
 *	\return Returns 0 if selection is possible or -1 if there is no blob before selection.
 */

int VisBlobSet :: SelectByCompactness( double max, double min )
{

	int retval = -1;

	CV_FUNCNAME( "SelectByCompactness" );
	__BEGIN__;


	vector<VisBlob>::iterator temp;

	if ( max < 0  || min < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad limits : max < 0 or min < 0 \n");
    }


	if( blob_temp.size() > 0 )
	{

		if( blob_temp[0].GetCompactness() == 0 )
		{
			CalculateCompactness();
		}

		for( temp = blob_temp.begin() ; temp != blob_temp.end() ; )
		{

				if( (*temp).GetCompactness() < min  || (*temp).GetCompactness()  >  max )
				{
					blob_temp.erase(temp);
					temp = blob_temp.begin();
				}
				else
				{
					temp++;
				}
		}
	
		retval = 0;
	}
	else
	{
		//no blob
		retval = -1;
	}
	

	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SelectByEccentricity
//********************************************************************************
/** \brief Selects blobs whose eccentricity is within the given limits.
 *
 *	This function delete blobs from the selected blobs (temporary) list whose eccentricity is not
 *	within the given limits.
 *
 *  \param  max: maximum eccentricity value
 *  \param  min: minimum eccentricity value
 *	\return Returns 0 if selection is possible or -1 if there is no blob before selection.
 */

int VisBlobSet :: SelectByEccentricity( double max, double min )
{

	int retval = -1;

	CV_FUNCNAME( "SelectByEccentricity" );
	__BEGIN__;


	vector<VisBlob>::iterator temp;

	if ( max < 0  || min < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad limits : max < 0 or min < 0 \n");
    }


	if( blob_temp.size() > 0 )
	{

		if( blob_temp[0].GetEccentricity() == 0 )
		{
			CalculateEccentricities();
		}

		for( temp = blob_temp.begin() ; temp != blob_temp.end() ; )
		{
				if( (*temp).GetEccentricity() < min  || (*temp).GetEccentricity()  >  max )
				{
					blob_temp.erase(temp);
					temp = blob_temp.begin();
				}
				else
				{
					temp++;
				}
		}
	
		retval = 0;
	}
	else
	{
		//no blob
		retval = -1;
	}

	
	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SelectByEllipseMajorAxisLength
//********************************************************************************
/** \brief Selects blobs whose Ellipse Major Axis Length is within the given limits.
 *
 *	This function delete blobs from the selected blobs (temporary) list whose Ellipse Major Axis Length is not
 *	within the given limits.
 *
 *  \param  max: maximum Ellipse Major Axis Length value
 *  \param  min: minimum Ellipse Major Axis Length value
 *	\return Returns 0 if selection is possible or -1 if there is no blob before selection.
 */

int VisBlobSet :: SelectByEllipseMajorAxisLength( double max, double min )
{

	int retval = -1;

	CV_FUNCNAME( "SelectByEllipseMajorAxisLength" );
	__BEGIN__;


	vector<VisBlob>::iterator temp;

	if ( max < 0  || min < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad limits : max < 0 or min < 0 \n");
    }

	if( blob_temp.size() > 0 )
	{

		if( blob_temp[0].GetEllipseMajorAxisLength() == 0 )
		{
			CalculateEllipseMajorAxisLengths();
		}

		for( temp = blob_temp.begin() ; temp != blob_temp.end() ; )
		{
				if( (*temp).GetEllipseMajorAxisLength() < min  || (*temp).GetEllipseMajorAxisLength()  >  max )
				{
					blob_temp.erase(temp);
					temp = blob_temp.begin();
				}
				else
				{
					temp++;
				}
		}
	
		retval = 0;
	}
	else
	{
		//no blob
		retval = -1;
	}
	

	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SelectByEllipseMinorAxisLength
//********************************************************************************
/** \brief Selects blobs whose Ellipse Minor Axis Length is within the given limits.
 *
 *	This function delete blobs from the selected blobs (temporary) list whose Ellipse Minor Axis Length is not
 *	within the given limits.
 *
 *  \param  max: maximum Ellipse Minor Axis Length value
 *  \param  min: minimum Ellipse Minor Axis Length value
 *	\return Returns 0 if selection is possible or -1 if there is no blob before selection.
 */

int VisBlobSet :: SelectByEllipseMinorAxisLength( double max, double min )
{

	int retval = -1;

	CV_FUNCNAME( "SelectByEllipseMinorAxisLength" );
	__BEGIN__;


	vector<VisBlob>::iterator temp;

	if ( max < 0  || min < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad limits : max < 0 or min < 0 \n");
    }

	if( blob_temp.size() > 0 )
	{

		if( blob_temp[0].GetEllipseMinorAxisLength() == 0 )
		{
			CalculateEllipseMinorAxisLengths();
		}

		for( temp = blob_temp.begin() ; temp != blob_temp.end() ; )
		{
				if( (*temp).GetEllipseMinorAxisLength() < min  || (*temp).GetEllipseMinorAxisLength()  >  max )
				{
					blob_temp.erase(temp);
					temp = blob_temp.begin();
				}
				else
				{
					temp++;
				}
		}
	
		retval = 0;
	}
	else
	{
		//no blob
		retval = -1;
	}

	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SelectByOrientation
//*********************************************************************************
/** \brief Selects blobs whose orientation is within the given limits.
 *
 *	This function delete blobs from the selected blobs (temporary) list whose orientation is not
 *	within the given limits.
 *
 *  \param  max: maximum orientation value
 *  \param  min: minimum orientation value
 *	\return Returns 0 if selection is possible or -1 if there is no blob before selection.
 */

int VisBlobSet :: SelectByOrientation( double max, double min )
{
	
	int retval = -1;

	CV_FUNCNAME( "SelectByOrientation" );
	__BEGIN__;


	vector<VisBlob>::iterator temp;


	if( blob_temp.size() > 0 )
	{

		if( blob_temp[0].GetOrientation() == 0 )
		{
			CalculateOrientations();
		}

		for( temp = blob_temp.begin() ; temp != blob_temp.end() ;  )
		{
			if( temp->GetOrientation() < min  || temp->GetOrientation()  >  max )
			{
				blob_temp.erase(temp);
				temp = blob_temp.begin();				
			}
			else
			{
				temp++;
			}

		}//end_for

		retval = 0;

	}
	else
	{	
		// no blob
		retval = -1;
	}
						
	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SelectAll
//********************************************************************************
/** \brief Select all the blobs detected.
 *
 *	This function copies the detected blobs list in the selected blobs (temporary) list.
 *	It a sort of "reset", that's why you have to recalculate blobs features after a "SelectAll". 
 *	This function must be used if you want to begin a new blobs selection from the detected blobs list.
 *
 *	\return Returns 0 if selection is possible or -1 if there is no blob before selection.
 */

int VisBlobSet :: SelectAll( void )
{

	CV_FUNCNAME( "SelectAll" );
	__BEGIN__;


	blob_temp.clear();

	if( blob_seq.size() > 0 )
	{
		
		for( unsigned int i = 0 ; i<blob_seq.size() ; i++ )
		{
			blob_temp.push_back(blob_seq[i]);
		}

		return 0;
	}
	else
	{
		return -1;
	}


	__END__;


}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  GetSegmentAngle
//********************************************************************************
/*! \brief Returns the angle of the segment constituted by the "blobnum"th blob center and the given point. 
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\param  point :  Second point of the segment
 *	\return Returns the nth blob area of the temporary list or -1 if there is 
 * 		   no blob corresponding to the given number. 
 */

double VisBlobSet :: GetSegmentAngle( CvPoint2D32f point , int blobnum )
{
	
	double retval = -1;

	CV_FUNCNAME( "GetSegmentAngle" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		retval = -1;
	}
	else
	{

		retval =  blob_temp[blobnum].GetSegmentAngle(point);
	}


	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
//********************************************************************************
//  GetArea
//********************************************************************************
/*! \brief Returns area of the "blobnum"th blob among selected blobs. 
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob area of the temporary list or -1 if there is 
 * 		   no blob corresponding to the given number. 
 */

double VisBlobSet :: GetArea( int blobnum )
{
	
	double retval = -1;

	CV_FUNCNAME( "GetArea" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		retval = -1;
	}
	else
	{

		if( blob_temp[blobnum].GetArea() == 0 )
		{
			CalculateAreas();
		}

		retval =  blob_temp[blobnum].GetArea();
	}


	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  GetPerimeter
//********************************************************************************
/*! \brief Returns perimeter of the "blobnum"th blob among selected blobs.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob perimeter of the temporary list or -1 if there is
 *		   no blob corresponding to the given number. 
 */

double VisBlobSet :: GetPerimeter( int blobnum )
{
		
	double retval = -1;

	CV_FUNCNAME( "GetPerimeter" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		retval = -1;
	}
	else
	{

		if( blob_temp[blobnum].GetPerimeter() == 0 )
		{
			CalculatePerimeters();
		}

		retval = blob_temp[blobnum].GetPerimeter();
	}


	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  GetCompactness
//********************************************************************************
/*! \brief Returns compactness of the "blobnum"th blob among selected blobs.
 *
 *	If the blob perimeter is less than 50, then the error on the compactness is
 *	bigger than 10%.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob compactness of the temporary list or -1 if there is
 *		   no blob corresponding to the given number.   
 */

double VisBlobSet :: GetCompactness( int blobnum )
{
	double retval = -1;
			
	CV_FUNCNAME( "GetCompactness" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		retval = -1;
	}
	else
	{
		
		if( blob_temp[blobnum].GetCompactness() == 0 )
		{
			CalculateCompactness();
		}

		retval = blob_temp[blobnum].GetCompactness();
	}


	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  GetEccentricity
//********************************************************************************
/*! \brief Returns eccentricity of the "blobnum"th blob among selected blobs.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob compactness of the temporary list or -1 if there is
 *		   no blob corresponding to the given number.   
 */

double VisBlobSet :: GetEccentricity( int blobnum )
{
	double retval = -1;		

	CV_FUNCNAME( "GetEccentricity" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		retval = -1;
	}
	else
	{

		if( blob_temp[blobnum].GetEccentricity() == 0 )
		{
			CalculateEccentricities();
		}

		retval = blob_temp[blobnum].GetEccentricity();
	}

	
	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  GetBoundingRectangle
//********************************************************************************
/*! \brief Returns Bounding Rectangle of the "blobnum"th blob among selected blobs.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  This function finds the bounding rectangle for the blob. "Bounding" means
 *  that all the points from the blob are either inside or on the boundary of the
 *  rectangle. The rectangle is horizontal that is why it is not the minimum
 *	enclosing rectangle wchich would have the same orientation as the blob.
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob compactness of the temporary list or -1 if there is
 *		   no blob corresponding to the given number.   
 */

CvRect VisBlobSet :: GetBoundingRectangle( int blobnum )
{
			
	CvRect temp;
	temp.x=-1;
	temp.y=-1;
	temp.width=-1;
	temp.height=-1;

	CV_FUNCNAME( "GetBoundingRectangle" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		return temp;
	}
	else
	{

		if( (blob_temp[blobnum].GetBoundingRectangle()).width == 0 )
		{
			CalculateBoundingRectangle();
		}
		
		temp = blob_temp[blobnum].GetBoundingRectangle();
	}

	
	__END__;

	return temp;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  GetEllipseMajorAxisLength
//********************************************************************************
/*! \brief Returns "equivalent" ellipse major axis length of the "blobnum"th 
 *			blob among selected blobs.
 *
 *  This function finds the "equivalent" ellipse of the blob and 
 *	returns the length of its major axis. "Equivalent" means
 *  that the ellipse has the same area, orientation and inertia as the blob.
 *	Thus, it has the same MajorAxis as the blob but, warning, its length is different.
 *	The advantage is that it is less sensitive to noise than the real blob major axis length.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob compactness of the temporary list or -1 if there is
 *		   no blob corresponding to the given number.   
 */

double VisBlobSet :: GetEllipseMajorAxisLength( int blobnum )
{
	double retval = -1;		

	CV_FUNCNAME( "GetEllipseMajorAxisLength" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		retval = -1;
	}
	else
	{

		if( blob_temp[blobnum].GetEllipseMajorAxisLength() == 0 )
		{
			CalculateEllipseMajorAxisLengths();
		}

		retval = blob_temp[blobnum].GetEllipseMajorAxisLength();
	}

	
	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  GetEllipseMinorAxisLength
//********************************************************************************
/*! \brief Returns "equivalent" ellipse minor axis length of the "blobnum"th blob among selected blobs.
 *
 *  This function finds the "equivalent" ellipse of the blob and 
 *	returns the length of its minor axis. "Equivalent" means
 *  that the ellipse has the same area, orientation and inertia as the blob.
 *	Thus, it has the same MinorAxis as the blob but, warning, its length is different.
 *	The advantage is that it is less sensitive to noise than the real blob minor axis length.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob compactness of the temporary list or -1 if there is
 *		   no blob corresponding to the given number.   
 */

double VisBlobSet :: GetEllipseMinorAxisLength( int blobnum )
{
	double retval = -1;		

	CV_FUNCNAME( "GetEllipseMinorAxisLength" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		retval = -1;
	}
	else
	{

		if( blob_temp[blobnum].GetEllipseMinorAxisLength() == 0 )
		{
			CalculateEllipseMinorAxisLengths();
		}

		retval = blob_temp[blobnum].GetEllipseMinorAxisLength();
	}

	
	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  GetMajorAxis
//********************************************************************************
/*! \brief Returns major axis of the "blobnum"th blob among selected blobs.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob orientation of the temporary list or a Line 
 *			whose coefficients are set to -10000 if there is
 *		   no blob corresponding to the given number.   
 */

VisLine VisBlobSet :: GetMajorAxis( int blobnum )
{

	VisLine temp(-10000,-10000,-10000);
	

	CV_FUNCNAME( "GetMajorAxis" );
	__BEGIN__;

	
	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum > number of blobs \n");
	}
	else
	{
		return( blob_temp[blobnum].GetMajorAxis() );
	}

	
	__END__;

	return temp;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  GetMinorAxis
//********************************************************************************
/*! \brief Returns minor axis of the "blobnum"th blob among selected blobs.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob orientation of the temporary list or a Line 
 *			whose coefficients are set to -10000 if there is
 *		   no blob corresponding to the given number.     
 */

VisLine VisBlobSet :: GetMinorAxis( int blobnum )
{

	VisLine temp(-10000,-10000,-10000);

	CV_FUNCNAME( "GetMinorAxis" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum > number of blobs \n");
	}
	else
	{
		return( blob_temp[blobnum].GetMinorAxis() );
	}

	
	__END__;

	return temp;

}

//********************************************************************************
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
//********************************************************************************
//  GetOrientation
//********************************************************************************
/*! \brief Returns orientation (in deg) of the "blobnum"th blob among selected blobs.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob orientation of the temporary list or -1 if there is
 *		   no blob corresponding to the given number.   
 */

double VisBlobSet :: GetOrientation( int blobnum )
{
	double retval = -1;	
	
	CV_FUNCNAME( "GetOrientation" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		retval = -1;
	}
	else
	{

		if( blob_temp[blobnum].GetOrientation() == 0 )
		{
			CalculateOrientations();
		}

		retval = blob_temp[blobnum].GetOrientation();
	}

	
	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  GetMajorAxisLength
//********************************************************************************
/*! \brief Returns major axis length of the "blobnum"th blob among selected blobs.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob major axis length of the temporary list or -1 if there is
 *		   no blob corresponding to the given number.   
 */

double VisBlobSet :: GetMajorAxisLength( int blobnum )
{
	double retval = -1;	
	
	CV_FUNCNAME( "GetMajorAxisLength" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		retval = -1;
	}
	else
	{

		if( blob_temp[blobnum].GetMajorAxisLength() == 0 )
		{
			CalculateMajorAxisLength();
		}

		retval = blob_temp[blobnum].GetMajorAxisLength();
	}

	
	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  GetMinorAxisLength
//********************************************************************************
/*! \brief Returns minor axis length of the "blobnum"th blob among selected blobs.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob minor axis length of the temporary list or -1 if there is
 *		   no blob corresponding to the given number.   
 */

double VisBlobSet :: GetMinorAxisLength( int blobnum )
{
	double retval = -1;	
	
	CV_FUNCNAME( "GetMinorAxisLength" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		retval = -1;
	}
	else
	{

		if( blob_temp[blobnum].GetMinorAxisLength() == 0 )
		{
			CalculateMinorAxisLength();
		}

		retval = blob_temp[blobnum].GetMinorAxisLength();
	}

	
	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
//********************************************************************************
//  GetEllipseSim
//********************************************************************************
/*! \brief Returns the ellipse similarity factor of the "blobnum"th blob among selected blobs.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob orientation of the temporary list or -1 if there is
 *		   no blob corresponding to the given number.   
 */

double VisBlobSet :: GetEllipseSim( int blobnum )
{
	double retval = -1;		

	CV_FUNCNAME( "GetEllipseSim" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		retval = -1;
	}
	else
	{

		if( blob_temp[blobnum].GetEllipseSim() == 0 )
		{
			CalculateEllipseSimFacts();
		}

		retval = blob_temp[blobnum].GetEllipseSim();
	}

	
	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  GetCenter
//********************************************************************************
/*! \brief  Returns center (with a precision of 1 pixel)  of the "blobnum"th blob
 *          among selected blobs.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob center of the temporary list or a CvPoint whose
 *         coordinates are -1 if there is no blob corresponding to the given number.  
 */

CvPoint VisBlobSet :: GetCenter( int blobnum )
{

	CvPoint centre ;
	centre.x = -1;
	centre.y = -1;
				
	CV_FUNCNAME( "GetCenter" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		
	}
	else
	{
		 CalculateCenters();

		 centre = blob_temp[blobnum].GetCentre();
	}


	__END__;

	return centre;

}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  GetCenterReal
//********************************************************************************
/*! \brief Returns real center of the "blobnum"th blob among selected blobs.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob center of the temporary list or a CvPoint whose
 *         coordinates are -1 if there is no blob corresponding to the given number. 
 */

CvPoint2D32f VisBlobSet :: GetCenterReal( int blobnum )
{

	CvPoint2D32f centre ;
	centre.x = -1;
	centre.y = -1;

	CV_FUNCNAME( "GetCenterReal" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		
	}
	else
	{
		CalculateCenters();

		centre = blob_temp[blobnum].GetCentreReal();
	}

	
	__END__;

	return centre;
	
}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  GetMinimumEnclosingCircleRadius
//********************************************************************************
/*! \brief Returns the radius of the minimum enclosing circle of the 
 *			"blobnum"th blob among selected blobs.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob center of the temporary list or -1 if there is a problem 
 */

float VisBlobSet :: GetMinimumEnclosingCircleRadius( int blobnum )
{

	float radius = -1.0 ;

	CV_FUNCNAME( "GetMinimumEnclosingCircleRadius" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		
		radius = -1.0;
	}
	else
	{

		if( blob_temp[blobnum].GetMinEncloCircleRadius() == 0 )
		{
			CalculateMinimumEnclosingCircle();
		}

		radius = blob_temp[blobnum].GetMinEncloCircleRadius();
	}


	__END__;

	return radius;

	
}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  GetMinimumEnclosingCircleCenter
//********************************************************************************
/*! \brief Returns the center of the minimum enclosing circle of the
 *		 "blobnum"th blob among selected blobs.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob center of the temporary list or -1 if there is a problem 
 */

CvPoint2D32f VisBlobSet :: GetMinimumEnclosingCircleCenter( int blobnum )
{

	CvPoint2D32f centre ;
	centre.x = -1;
	centre.y = -1;

	CV_FUNCNAME( "GetMinimumEnclosingCircleCenter" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		
	}
	else
	{

		if( (blob_temp[blobnum].GetMinEncloCircleCenter()).x == 0.0 )
		{
			CalculateMinimumEnclosingCircle();
		}

		centre = blob_temp[blobnum].GetMinEncloCircleCenter();
	}

	
	__END__;

	return centre;

	
}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  DrawMinimumEnclosingCircle
//********************************************************************************
/*! \brief Draws minimum enclosing circle of the
 *		 "blobnum"th blob among selected blobs.
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list.
 *  \param  dstimg : Destination image where the circle is to be drawn.
 *  \param  color :  Circle color
 *  \param  thickness : Circle thickness ( to fulfill the circle, thickness=-1)
 *	\return Returns the nth blob center of the temporary list or -1 if there is a problem. 
 */

int VisBlobSet :: DrawMinimumEnclosingCircle( IplImage *dstimg, int color, int thickness )
{
	int retval = -1;

	int nblob = blob_temp.size();
	int i=0;

	CV_FUNCNAME( "DrawMinimumEnclosingCircle" );
	__BEGIN__;


	if (!dstimg)
    {
      CV_ERROR(CV_StsBadArg, "\n IplImage* dstimg is empty \n");
    }

	if( nblob > 0 )
	{

		for( i=0 ; i<nblob ; i++ )
		{
		  cvCircle(
            dstimg,
            cvPoint( 
              (int)(blob_temp[i].GetMinEncloCircleCenter()).x,
              (int)(blob_temp[i].GetMinEncloCircleCenter()).y
            ),
            (int)(blob_temp[i].GetMinEncloCircleRadius()),
            CV_RGB(color,color,color),
            thickness
          );
		}
	
		retval = 0;
	}
	else
	{
		retval = -1;
	}

	

	__END__;

	return retval;


}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  GetEquivalentRectangle
//********************************************************************************
/*! \brief Returns the "equivalent" rectangle of the
 *		 "blobnum"th blob among selected blobs 
 *
 *	The "equivalent" rectangle of the blob is a rectangle which has the 
 *	same area, center, orientation and so inertia as the blob.
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns the nth blob center of the temporary list  
 */

CvBox2D VisBlobSet :: GetEquivalentRectangle( int blobnum )
{
	return( blob_temp[blobnum].GetEquivalentRect() );
}

//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  DrawEquivalentRectangle
//********************************************************************************
/*! \brief Draws the "equivalent" rectangle of the
 *		 "blobnum"th blob among selected blobs.
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list.
 *  \param  dstimg : Destination image where the circle is to be drawn.
 *  \param  color :  Rectangle color
 *  \param  thickness : Rectangle thickness ( to fulfill the circle, thickness=-1)
 *	\return Returns the nth blob center of the temporary list or -1 if there is a problem. 
 */

int VisBlobSet :: DrawEquivalentRectangle( IplImage *dstimg, int color, int thickness )
{
	int retval = -1;

	int nblob = blob_temp.size();
	int i=0;

	
	CvPoint2D32f rect[4];

	CV_FUNCNAME( "DrawEquivalentRectangle" );
	__BEGIN__;


	if (!dstimg)
    {
      CV_ERROR(CV_StsBadArg, "\n IplImage* dstimg is empty \n");
    }

	if( nblob > 0 )
	{

		for( i=0 ; i<nblob ; i++ )
		{
		cvBoxPoints( blob_temp[i].GetEquivalentRect(), rect );
		cvLine( dstimg, cvPoint((int)rect[0].x , (int)rect[0].y ), cvPoint((int)rect[1].x , (int)rect[1].y ), CV_RGB(color,color,color), thickness=1 );
		cvLine( dstimg, cvPoint((int)rect[1].x , (int)rect[1].y ), cvPoint((int)rect[2].x , (int)rect[2].y ), CV_RGB(color,color,color), thickness=1 );
		cvLine( dstimg, cvPoint((int)rect[2].x , (int)rect[2].y ), cvPoint((int)rect[3].x , (int)rect[3].y ), CV_RGB(color,color,color), thickness=1 );
		cvLine( dstimg, cvPoint((int)rect[3].x , (int)rect[3].y ), cvPoint((int)rect[0].x , (int)rect[0].y ), CV_RGB(color,color,color), thickness=1 );
		}
	
		retval = 0;
	}
	else
	{
		retval = -1;
	}
	
	__END__;

	return retval; 

}

//********************************************************************************
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
//********************************************************************************
//  DrawBoundingRectangle
//********************************************************************************
/*! \brief Draws the bounding rectangle of the
 *		 "blobnum"th blob among selected blobs.
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list.
 *  \param  dstimg : Destination image where the circle is to be drawn.
 *  \param  color :  Rectangle color
 *  \param  thickness : Rectangle thickness ( to fulfill the circle, thickness=-1)
 *	\return Returns the nth blob center of the temporary list or -1 if there is a problem. 
 */

int VisBlobSet :: DrawBoundingRectangle( IplImage *dstimg, int color, int thickness )
{
	int retval = -1;

	int nblob = blob_temp.size();
	int i=0;

	CvRect rect;
	CvPoint prect[2];


	CV_FUNCNAME( "DrawBoundingRectangle" );
	__BEGIN__;


	if (!dstimg)
    {
      CV_ERROR(CV_StsBadArg, "\n IplImage* dstimg is empty \n");
    }

	if( nblob > 0 )
	{

		for( i=0 ; i<nblob ; i++ )
		{
			rect = GetBoundingRectangle(i);
			prect[0].x = rect.x;
			prect[0].y = rect.y;
			prect[1].x = rect.x + rect.width;
			prect[1].y = rect.y + rect.height;
			cvRectangle( dstimg, prect[0], prect[1], CV_RGB(color,color,color), thickness );
		}
	
		retval = 0;
	}
	else
	{
		retval = -1;
	}

  
	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
//********************************************************************************
//  DrawContours
//********************************************************************************
/*! \brief Draw contours of the selected blobs in the given destination image.
 *  \param dstimg:  Destination image where the contours are to be drawn.
 *  \param externalColor:  Color to draw external contours with.
 *  \param holeColor:  Color to draw holes with.
 *  \param maxLevel:  Maximal level for drawn contours. If 0, only the contour is 
 *			drawn. If 1, the contour and all contours after it on the same level
 *			are drawn. If 2, all contours after and all contours one level below
 *			the contours are drawn, etc.
 *  \param thickness:  Thickness of lines the contours are drawn with (if thickness = -1 then the contour is fulfilled).
 *  \return Returns 0 if drawing is possible or -1 if there is no selected blob.
 */
int VisBlobSet :: DrawContours(IplImage* dstimg, double external_color, double hole_color, int max_level, int thickness, int connectivity)
{
	int retval = -1;

	CV_FUNCNAME( "DrawContours" );
	__BEGIN__;


	int nblob = blob_temp.size();
	int i=0;

	if (!dstimg)
    {
      CV_ERROR(CV_StsBadArg, "\n IplImage* dstimg is empty \n");
    }


	if( nblob > 0 )
	{

		for( i=0 ; i<nblob ; i++ )
		{
			cvDrawContours(dstimg, blob_temp[i].GetContour(), CV_RGB(external_color,external_color,external_color), CV_RGB(hole_color,hole_color,hole_color), max_level ,thickness, connectivity);
		}
	
		retval = 0;
	}
	else
	{
		retval = -1;
	}
	

	__END__;

	return retval;

}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  DisplayCross
//********************************************************************************
/*! \brief Draw a cross of the center of the "blobnum"th blob among selected blobs.
 *  \param dstimg:  Destination image where the contours are to be drawn.
 *
 *  \return Returns 0 if drawing is possible or -1 if there is no selected blob.
 */
int VisBlobSet :: DisplayCross(IplImage *dstimg, int color, int thickness, int size, int blobnum )
{
	int retval = -1;

	CvPoint pt = cvPoint(0,0);
	CvPoint pt1 = cvPoint(0,0);
	CvPoint pt2 = cvPoint(0,0);
	int nblob = 0;

	CV_FUNCNAME( "DisplayCross" );
	__BEGIN__;


	if (!dstimg)
    {
      CV_ERROR(CV_StsBadArg, "\n IplImage* dstimg is empty \n");
    }

	nblob = blob_temp.size();

	if( nblob > 0 )
	{

		pt = GetCenter(blobnum);
	
		pt1.x = pt.x - size;
		pt1.y = pt.y;

		pt2.x = pt.x + size;
		pt2.y = pt.y;

		cvLine (dstimg, pt1, pt2, CV_RGB(color,color,color), thickness, 8);

		pt1.x = pt.x;
		pt1.y = pt.y - size;

		pt2.x = pt.x;
		pt2.y = pt.y + size;

		cvLine (dstimg, pt1, pt2, CV_RGB(color,color,color), thickness, 8);
	
		retval = 0;

	}
	else
	{
		retval = -1;
	}
	

	__END__;

	return retval;

}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//  EmptyDetectedBlobsList
//********************************************************************************
/** \brief Empties the detected Blobs list.
 *
 *	This function empties the detected Blobs list and releases memory.
 *
 *	\return Returns 0 if drain is possible or -1 if there is no blob before selection.
 */

int VisBlobSet ::  EmptyDetectedBlobsList( void )
{

	CV_FUNCNAME( "EmptyDetectedBlobsList" );
	__BEGIN__;


	if( blob_seq.size() > 0 )
	{
		blob_temp.clear();
		blob_seq.clear();
		cvReleaseMemStorage (&mem);
	}

	return 0;

	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  GetEllipse_Similarity
//********************************************************************************
/*! \brief Returns an int indicating if the blob is near from an elllipse.
 *
 *   If result=1, the blob is near from an ellipse.
 *	 If result=2, the blob is near from a line.
 *	 If result=0, no distinction is possible.
 *
 *	This function is based on the ratio : 
 *	\f[  \frac{Area}{MinEnclosingCircleRadius^2} \f]
 *
 *	Blobs are numbered from 0 to (Selected Blobs Number - 1).
 *
 *  \param  blobnum: Number of the indicated blob in the selected blobs list
 *	\return Returns  -1 if there is no blob corresponding to the given number
 *					  1 if the blob is near from an ellipse
 *					  2 if the blob is near from a line
 */

int VisBlobSet :: GetEllipse_Similarity( int blobnum )
{
	int retval = -1;

	double sim =0.0;

	CV_FUNCNAME( "GetEllipse_Similarity" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		retval = -1;
	}
	else
	{
		if( GetEllipseSim(blobnum) == 0 )
		{
			blob_temp[blobnum].Calc_EllipseSim();
		}

		sim = GetEllipseSim(blobnum);

		cout << " \n sim =  " << sim << endl;

		if( sim > 2 )
		{
			retval = 1; //ellipse
		}
		else
		{
			if( sim < 0.2 )
			{
				retval = 2; //line
			}
			else
			{
				retval = 0;//no distinction
			}
		}

	}


	__END__;

	return retval;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  LoadReferenceBlob
//********************************************************************************
/** \brief Learns the Reference from the given point and the reference blobs list 
 *
 *	To create the reference blobs list, you have to call this method
 *	for each blob of the selected blobs list you want to take as a reference.
 *
 *	\param  blobnum: Number of the indicated blob in the selected blobs list. 
 *	\return Returns  -1 if there is no blob corresponding to the given number
 *	\sa LearnReference(), GetReference()
 */
int VisBlobSet ::  LoadReferenceBlob( int blobnum )
{
	int retval = -1;


	CV_FUNCNAME( "LoadReferenceBlob" );
	__BEGIN__;


	if ( blobnum < 0 )
    {
      CV_ERROR(CV_StsBadArg, "\n Bad blob number : blobnum < 0 \n");
    }

	if( blobnum > (int)(blob_temp.size()-1) )
	{
		retval = -1;
	}
	else
	{
		if( (blob_temp[0].GetCompactness() == 0) || (blob_temp[0].GetEccentricity() == 0) || (blob_temp[0].GetEllipseSim() == 0)  )
		{
			CalculateCompactness();
			CalculateEccentricities();
			CalculateEllipseSimFacts();
		}

		ref_blob_seq.push_back(blob_temp[blobnum]);

		retval=0;
	}


	__END__;

	return retval;


}
//********************************************************************************
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
//********************************************************************************
//  LearningReference
//********************************************************************************
/** \brief Learns the Reference from the given point and the reference blobs list 
 *
 *	This function, with the functions "GetReference()" and "LoadReferenceBlob()", enable
 *	to calculate the coordinates of a point by learning its position according to
 *	other points which are here the blob centers in a reference image.
 *
 *	To create the reference blobs list, you have to call the method "LoadReferenceBlob()"
 *	for each blob of the selected list you want to take as a reference.
 *
 *	!!!!!!   ONLY ONE REFERENCE CAN BE MADE  FOR ONE VISBLOBSET  -> another list of reference blobs should be created for another reference  !!!!!!
 *	!!!!!!       AN IMPROVEMENT SHOULD BE DONE, MAY BE  A VECTOR<VECTOR<VisBlob>>          !!!!!!!
 *
 *	\param  target :  Target point which will be calculated in the new image. 
 *	\return Returns 0 if learning is possible or -1 if there is no blob before selection.
 *	\sa GetReference(), LoadReferenceBlob()
 */

int VisBlobSet ::  LearningReference( CvPoint2D32f target , double* p_cond_number)
{
	int i=0;
	/*
	CvPoint2D32f gravity_center;
	gravity_center.x=0;
	gravity_center.y=0;
	*/
//------------------------------------------------------------------
//	ofstream ostr("Model_3.dat", ios::out | ios::app) ;
//	clock_t start=0, finish=0;
//------------------------------------------------------------------


	CV_FUNCNAME( "LearningReference" );
	__BEGIN__;


	// Creates the new model
	VisGeomModel new_model;

	// Set the new learnt target point
	new_model.SetLearntTargetPoint(target);


/*
	for( i = 0 ; i<ref_blob_seq.size() ; i++ )
		{
			gravity_center.x += (ref_blob_seq[i].GetCentreReal()).x/ref_blob_seq.size();
			gravity_center.y += (ref_blob_seq[i].GetCentreReal()).y/ref_blob_seq.size();
		}
	
	// Sorts blobs considering their angular position according to the gravity center of all the blob centers
	for( i = 0 ; i<ref_blob_seq.size() ; i++ )
		{
			 ref_blob_seq[i].SetKey( ref_blob_seq[i].GetSegmentAngle(gravity_center) );
		}
	std::sort(ref_blob_seq.begin(),ref_blob_seq.end());
		// sorting in a descending order
		// That's why the operator "<" is reversed.
*/

//--------------------------------------------------------
//for(int k=0  ; k<1000 ; k++ )
//{
//----------------------------------
//	start = clock(); // start time
//----------------------------------

	SortByAngle(&ref_blob_seq);	

//----------------------------------
//	finish = clock(); // finish  time
//----------------------------------

//	time += ((double)(finish - start) );

//}	
//--------------------------------------------------------------------------
//	ostr << "\t  " << ((double)(finish - start) ) << "   \t ";
//--------------------------------------------------------------------------



	num_ref_blob_seq.push_back(ref_blob_seq.size());

	
	
	// Set the learnt reference points list
	for( i = 0 ; i<(int)(ref_blob_seq.size()) ; i++ )
	{
			new_model.SetLearntPointsList( ref_blob_seq[i].GetCentreReal() );
	}

//--------------------------------------------------------
//----------------------------------
//	start = clock(); // start time
//----------------------------------
//for(int k=0  ; k<1000 ; k++ )
//{


	new_model.CreateModel(p_cond_number);


//}	
//----------------------------------
//	finish = clock(); // finish  time
//----------------------------------
//--------------------------------------------------------------------------
//	ostr << "\t  " << ((double)(finish - start) ) << "   \n";
//--------------------------------------------------------------------------



	models_seq.push_back(new_model);

	return 0;

	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  GetReference
//********************************************************************************
/** \brief Calculates the target point from the learnt reference created with the given BlobSet  
 *
 *	This function, with the functions "LearnReference()" and "LoadReferenceBlob()", enable
 *	to calculate the coordinates of a point by learning its position according to
 *	other points which are here the blob centers in a reference image.
 *
 *	\param  p_refblobset : address of the BlobSet used to learn the references
 *	\param  num_ref : Index of the reference you want to calculate 
 *					(if you have made N references then the index is comprised between 0 and N-1)
 *	\param  area_variance :  Area variance of blobs
 *					indicates if there is a change of scale. If area_variance=0, the scale is considered
 *					different in the new image and reference blobs are not searched considering their area.
 *					If area_variance \neq 0, the scale is considered the same in the new image as in the learning 
 *					one, and reference blobs are searched considering their area.In this case, the area variance must be given 
 *					as a parameter. If you don't know which variance choose, you can give the following variance:
 *					\f[  \frac{ImageWidth*ImageHeight}{12}  \f]  
 *
 *	\param  p_cond_number : Pointer on the double where the condition number of the marix used in
 *							calcultaion is stored. The condition number must be lower than 10.
 *	\param  p_error : Pointer on the double where the least square error of the calculation is stored.
 *						
 *	\return Returns 0 if drain is possible or -1 if there is no blob before selection.
 *	\sa LearnReference(), LoadReferenceBlob()
 */

CvPoint2D32f VisBlobSet ::  GetReference( VisBlobSet* pblobset, int num_ref , double area_variance , double* p_cond_number, double* p_error)
{

	CvPoint2D32f temp_point;
	temp_point.x = -1;
	temp_point.y = -1;

	unsigned int i=0;
    int j=0;
	int num_points = 0;
	int index_ref_points = 0;
	vector<VisBlob>::iterator temp;

//-----------------------------------------------------
//	ofstream ostr("CalcTarget_2.dat", ios::out | ios::app) ;
//	clock_t start, finish;
//-----------------------------------------------------


	CV_FUNCNAME( "GetReference" );
	__BEGIN__;



	num_points = (pblobset->num_ref_blob_seq)[num_ref];

	
	if( num_ref > 0 )
	{
		for( i=0 ; i<(unsigned)num_ref ; i++ )
		{
			index_ref_points += (pblobset->num_ref_blob_seq)[i];  
		}
	}


	// Find the blobs that match the given ones
	if( (blob_temp[0].GetCompactness() == 0) || (blob_temp[0].GetEccentricity() == 0) || (blob_temp[0].GetEllipseSim() == 0)  )
	{
		CalculateCompactness();
		CalculateEccentricities();
		CalculateEllipseSimFacts();
	}


	for(j=0 ; j<num_points ; j++ )
	{

		if(area_variance==0)
		{
			for( i=0 ; i<blob_temp.size() ; i++ )
			{		
				blob_temp[i].SetKey(-sqrt( (blob_temp[i].GetCompactness()-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetCompactness())*(blob_temp[i].GetCompactness()-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetCompactness()) \
					+ (blob_temp[i].GetEccentricity()-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetEccentricity())*(blob_temp[i].GetEccentricity()-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetEccentricity()) \
					+ (blob_temp[i].GetEllipseSim()-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetEllipseSim())*(blob_temp[i].GetEllipseSim()-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetEllipseSim())  ) );	
			}
		}
		else
		{
			for( i=0 ; i<blob_temp.size() ; i++ )
			{
				blob_temp[i].SetKey(-sqrt( (blob_temp[i].GetCompactness()-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetCompactness())*(blob_temp[i].GetCompactness()-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetCompactness()) \
					+ (blob_temp[i].GetEccentricity()-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetEccentricity())*(blob_temp[i].GetEccentricity()-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetEccentricity()) \
					+ (blob_temp[i].GetEllipseSim()-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetEllipseSim())*(blob_temp[i].GetEllipseSim()-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetEllipseSim()) \
					+ (blob_temp[i].GetArea()/area_variance-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetArea()/area_variance)*(blob_temp[i].GetArea()/area_variance-(*(pblobset->GetRefBlobSeq()))[index_ref_points+j].GetArea()/area_variance) ) );	
			}
		}

		VisBlobSet :: Sort();

		
		(*(pblobset->GetModelsSeq()))[num_ref].SetFoundPointsList( blob_temp[0].GetCentreReal() );


		// Delete blob in order to avoid selecting several times the same one.
		temp = blob_temp.begin();
		blob_temp.erase(temp);

	}


//----------------------------------
//	start = clock(); // start time
//----------------------------------
//for(int k=0  ; k<1000 ; k++ )
//{


	temp_point = (*(pblobset->GetModelsSeq()))[num_ref].CalculateTarget(p_cond_number, p_error);


//}	
//----------------------------------
//	finish = clock(); // finish  time
//----------------------------------
//--------------------------------------------------------------------------
//	ostr << "\t  " << ((double)(finish - start) ) << "   \n";
//--------------------------------------------------------------------------


	return temp_point;

	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  IsPointInsideBlob
//********************************************************************************
/** \brief Returns the list of the numbers of blobs containing the given point 
 *
 *
 *	\param  point: Given point. 
 *	\param  blob_list :  Pointer to the vector where the numbers of the blobs are stored
 *	\return Returns  -1 if there is no blob containing the given point or 0 if at least 1 blob contains it.
 */
int VisBlobSet ::  IsPointInsideBlob( CvPoint point, vector<int>* p_blob_list )
{
	int retval = -1;
	int flag=0;
	int i=0;


	CV_FUNCNAME( "IsPointInsideBlob" );
	__BEGIN__;

	for( i=0 ; i<GetSelectedNumBlobs() ; i++) 
	{
			flag = IsPointInsidePoly(blob_temp[i].GetContour(), point);

			if(flag == 1)
			{
				retval = flag;
				p_blob_list->push_back(i);
			}
	}


	__END__;

	return retval;


}
//********************************************************************************
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  SortByAngle
//********************************************************************************
/** \brief Sort a vector of Blobs according to the angle of the segment between
 *			 blob center and gravity center of all blobs. 
 *
 *
 *	\param  p_blob_list :  Pointer on the vector of blobs. 
 *	\return Returns 0 if sort is possible or -1 if there is no blob in the vector.
 */

int VisBlobSet ::  SortByAngle( vector<VisBlob>* p_blob_list )
{
	unsigned int i=0;
	CvPoint2D32f gravity_center;
	gravity_center.x=0;
	gravity_center.y=0;

	CV_FUNCNAME( "SortByAngle" );
	__BEGIN__;


	if( p_blob_list->size() > 0 )
	{

		for( i = 0 ; i<p_blob_list->size() ; i++ )
		{
			gravity_center.x += ((*p_blob_list)[i].GetCentreReal()).x/p_blob_list->size();
			gravity_center.y += ((*p_blob_list)[i].GetCentreReal()).y/p_blob_list->size();
		}
	
		// Sorts blobs considering their angular position according to the gravity center of all the blob centers
		for( i = 0 ; i<p_blob_list->size() ; i++ )
		{
			 (*p_blob_list)[i].SetKey( (*p_blob_list)[i].GetSegmentAngle(gravity_center) );
		}

		std::sort(p_blob_list->begin(),p_blob_list->end());
		// sorting in a descending order
		// That's why the operator "<" is reversed.

		return 0;
	}

	else
	{
		return -1;	
	}

	__END__;

}

//********************************************************************************
//--------------------------------------------------------------------------------








/*************************************************************************************************/
/************* private class methods and functions implementations *******************************/
/*************************************************************************************************/

/*************************************************************************************************/
/********************** test routines/program ****************************************************/
/*************************************************************************************************/

#ifdef BLOBSET_TEST

#include <iostream>
#include <fstream>
#include "ipllpm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include "VisLine.hpp"
#include "simple_im_proc.h"
#include <conio.h>
#include <cv.h>
#include <time.h>
#include "VisBinarisation.hpp"

using namespace std; //So that we can use standard library names



void tst0(void) 
{

	int i = 0;


	IplImage* imagetmp;
	//IplImage* image;
		
	imagetmp = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	//image = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);

//----------------------------------------------------------------
//	imagetmp = loadtiff("U:/images/diamants_3.tif");  //--> image fichier
//----------------------------------------------------------------

//----------------------------------------------------------------
//	imagetmp = loadtiff("U:/Projet2/testblobset/testBlob_riz.tif");  //--> image fichier
//----------------------------------------------------------------

	//binarisation(imagetmp, image );
	//clean_morpho(IplImage *imgS, IplImage *imgD, IplImage *imgtmp );
	//savetiff(image,"testBlob_diamant.tif");



	cvCircle(imagetmp,cvPoint(250,250),100,255,-1);
	cvCircle(imagetmp,cvPoint(250,250),70,0,-1);
	cvCircle(imagetmp,cvPoint(100,100),50,255,-1);
	cvRectangle (imagetmp, cvPoint(300,50), cvPoint( (300+70) , (50+30)),255,-1);
	cvRectangle (imagetmp, cvPoint(450,250), cvPoint( (450+ 111) , (250+ 111)),255,-1);
	cvCircle(imagetmp,cvPoint(505,305),50,0,-1);
	cvEllipse (imagetmp, cvPoint(500,50), cvSize( 70, 30),20,-180,180,255,-1);
	cvEllipse (imagetmp, cvPoint(500,500), cvSize( 50, 38),65,-180,180,255,-1);
	cvEllipse (imagetmp, cvPoint(150,500), cvSize( 120, 28),15,-180,180,255,-1);


	//savetiff(imagetmp,"testBlobs_1.tif");

	VisBlobSet group;
	VisBlobSet group2;

	cout << "\n group2 \n";

	group2.FindBlobs(imagetmp);

	group2.CalculateFeatures();
	group2.SelectByArea(100000 , 100);
	group2.Select(3);
	cout << " The 1rst Blob Compactness :  " << group2.GetCompactness()  << endl ;
	cout << " The 1rst Blob Eccentricity :  " << group2.GetEccentricity()  << endl ;

	cout << "\n group \n";

	group.FindBlobs(imagetmp);
	
	cout << " Total Number of Blobs found :  " << group.GetTotalNumBlobs()  << endl ;
	
	if( group.CalculateFeatures() == 0 )
	{

		group.SelectByArea(200000 , 100);
		cout << " Number of selected Blobs  :  " << group.GetSelectedNumBlobs()  << endl ;

		
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Area :  " << group.GetArea(i)  << endl ;
		}

		cout << endl;

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Perimeter :  " << group.GetPerimeter(i)  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Compactness :  " << group.GetCompactness(i)  << endl ;
		}

		cout << endl;

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Eccentricity :  " << group.GetEccentricity(i)  << endl ;
		}

		cout << endl;

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) EllipseMajorAxisLength :  " << group.GetEllipseMajorAxisLength(i)  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) EllipseMinorAxisLength :  " << group.GetEllipseMinorAxisLength(i)  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Center x :  " << (group.GetCenter(i)).x  << endl ;
			cout << " Blob( " <<  i  << " ) Center y :  " << (group.GetCenter(i)).y  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Center Real x :  " << (group.GetCenterReal(i)).x  << endl ;
			cout << " Blob( " <<  i  << " ) Center Real y :  " << (group.GetCenterReal(i)).y  << endl ;
		}

		cout << endl;

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Orientation :  " << (group.GetOrientation(i)) << " deg" << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Coeff A pour axe majeur :  " << (group.GetMajorAxis(i)).a  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Coeff B pour axe majeur :  " << (group.GetMajorAxis(i)).b  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) radius of minimum enclosing circle :  " << (group.GetMinimumEnclosingCircleRadius(i)) << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Center Real x of minimum enclosing circle :  " << (group.GetMinimumEnclosingCircleCenter(i)).x  << endl ;
			cout << " Blob( " <<  i  << " ) Center Real y of minimum enclosing circle :  " << (group.GetMinimumEnclosingCircleCenter(i)).y  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Center Real x of minimum area rectangle :  " << ((group.GetEquivalentRectangle(i)).center).x  << endl ;
			cout << " Blob( " <<  i  << " ) Center Real y of minimum area rectangle :  " << ((group.GetEquivalentRectangle(i)).center).y  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Orientation of minimum area rectangle :  " << ((group.GetEquivalentRectangle(i)).angle)*180/3.14 << " deg " << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Width of minimum area rectangle :  " << ((group.GetEquivalentRectangle(i)).size).width << endl ;
		}

		cout << endl;

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Height of minimum area rectangle :  " << ((group.GetEquivalentRectangle(i)).size).height << endl ;
		}

		cout << endl;

		cout << "\n\n Is point (250,250) inside a blob ? \n";
		vector<int> blob_num_list;
		group.IsPointInsideBlob(cvPoint(250,250) , &blob_num_list);
		for(i=0 ; i<blob_num_list.size() ; i++ )
		{
			cout << "\n  The point is inside the blob number : " <<blob_num_list[i] <<" \n";
		}


			//group.DrawMinimumEnclosingCircle( imagetmp, 135, 1);
	
			//group.DrawEquivalentRectangle( imagetmp, 135, 1);
		
		//group.DrawBoundingRectangle( imagetmp, 135, 1);

		//savetiff(imagetmp,"Enclo_circle_rect.tif");
	
		//group.DrawContours(imagetmp, 135, 135, 0, 1);
		//savetiff(imagetmp,"Contours_riz.tif");

		cout << endl;
		getch();
/*
		cout << "\n Ellipse Similarity of Blob(7) : " << group.GetEllipse_Similarity(7) << endl;

		cout << endl;
		getch();

		group.SelectAll();
		cout << "\n\n SelectAll \n";
		cout << " Total Number of Blobs found :  " << group.GetTotalNumBlobs()  << endl ;
		
		group.CalculateFeatures();


		cout << "\n\n Blob Fitting  : \n";
		group.BlobFitting(&group2);
		cout << " The 1rst Blob Compactness :  " << group.GetCompactness()  << endl ;
		cout << " The 1rst Blob Eccentricity :  " << group.GetEccentricity()  << endl ;

*/

		cout << endl;
		getch();

		group.SortByArea();
		cout << "\n SortByArea \n";
		cout << " The largest Blob area :  " << group.GetArea()  << endl ;

		group.SortByPerimeter();
		cout << "\n SortByPerimeter \n";
		cout << " The largest Blob perimeter :  " << group.GetPerimeter()  << endl ;

		group.SortByCompactness();
		cout << "\n SortByCompactness \n";
		cout << " The compactest Blob Compactness :  " << group.GetCompactness()  << endl ;

		group.SelectByArea(50000 , 20000);
		cout << "\n SelectByArea( max = 50000 , min = 20000) \n";
		cout << " Number of selected Blobs  :  " << group.GetSelectedNumBlobs()  << endl ;
		group.SortByArea();
		cout << " SortByArea \n";
		cout << " SelectByArea : Number of Blobs found =  " << group.GetSelectedNumBlobs()  << endl ;
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Area :  " << group.GetArea(i)  << endl ;
		}

		group.SelectAll();
		cout << "\n SelectAll \n";

		getch();

		group.SelectByCompactness(1 , 0.90);
		cout << "\n SelectByComp( max = 1 , min = 0.9) \n";
		cout << " Number of selected Blobs  :  " << group.GetSelectedNumBlobs()  << endl ;
		group.SortByCompactness();
		cout << " SortByCompactness \n";
		cout << " SelectByCompactness : Number of Blobs found =  " << group.GetSelectedNumBlobs()  << endl ;
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Compactness :  " << group.GetCompactness(i)  << endl ;
		}

		group.SelectAll();
		cout << "\n SelectAll \n";

		getch();

		group.SelectByPerimeter(700 , 300);
		cout << "\n SelectByPerimeter( max = 700 , min = 300) \n";
		cout << " Number of selected Blobs  :  " << group.GetSelectedNumBlobs()  << endl ;
		group.SortByPerimeter();
		cout << " SortByPerimeter \n";
		cout << " SelectByPerimeter : Number of Blobs found =  " << group.GetSelectedNumBlobs()  << endl ;
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Perimeter :  " << group.GetPerimeter(i)  << endl ;
		}


		group.Select(1);
		cout << "\n Select Blob number 1 " << endl;
		cout << " Blob( " <<  1  << " ) Perimeter :  " << group.GetPerimeter()  << endl ;

		group.SelectAll();
		cout << "\n\n SelectAll \n";

		getch();

		group.SelectByArea(50000 , 20000);
		cout << "\n SelectByArea( max = 50000 , min = 20000) \n";
		cout << " Number of selected Blobs  :  " << group.GetSelectedNumBlobs()  << endl ;
		group.SortByArea();
		cout << " SortByArea \n";
		cout << " SelectByArea : Number of Blobs found =  " << group.GetSelectedNumBlobs()  << endl ;
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Area :  " << group.GetArea(i)  << endl ;
		}

		getch();

		group.SelectAll();
		cout << "\n\n SelectAll \n";
		cout << " Total Number of Blobs found :  " << group.GetTotalNumBlobs()  << endl ;
		
		group.CalculateFeatures();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Orientation :  " << (group.GetOrientation(i)) << " deg" << endl ;
		}

		group.SelectAll();
		cout << "\n\n SelectAll \n";
		cout << " Total Number of Blobs found :  " << group.GetTotalNumBlobs()  << endl ;

		cout << "\n SelectOrientation( max = 90 , min = -1) \n";
		group.SelectByOrientation(90 , -1);
		cout << " Number of selected Blobs  :  " << group.GetSelectedNumBlobs()  << endl ;
		group.SortByOrientation();
		cout << " SortByOrientation \n";
		cout << " SelectByOrientation : Number of Blobs found =  " << group.GetSelectedNumBlobs()  << endl ;
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Orientation :  " << group.GetOrientation(i)  << endl ;
		}

		group.SelectAll();
		cout << "\n\n SelectAll \n";
		cout << " Total Number of Blobs found :  " << group.GetTotalNumBlobs()  << endl ;

		getch();

		group.CalculateFeatures();
		cout << "\n  SortByPosition : cvPoint(105,105) \n" << endl;
		group.SortByPosition(cvPoint2D32f(105.0,105.0));
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Center x :  " << (group.GetCenter(i)).x  << endl ;
			cout << " Blob( " <<  i  << " ) Center y :  " << (group.GetCenter(i)).y  << endl ;
		}


		group.SelectAll();
		cout << "\n\n SelectAll \n";
		cout << " Total Number of Blobs found :  " << group.GetTotalNumBlobs()  << endl ;

		getch();

		group.SortByArea(10000);
		cout << " SortByArea(10000) : \n";
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Area :  " << group.GetArea(i)  << endl ;
		}

		group.SortByCompactness(0.5);
		cout << " SortByCompactness (0.5) :  \n";
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Compactness :  " << group.GetCompactness(i)  << endl ;
		}
		
		group.SortByPerimeter(200);
		cout << " SortByPerimeter(200) \n";
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Perimeter :  " << group.GetPerimeter(i)  << endl ;
		}

		cout << "\n\n Perimeter of blob number 4 :  " << group.GetPerimeter(4)  << endl ;
		cout << "\n Erase Blob number 4 \n";
		group.Erase(4);
		cout << " Number of selected Blobs  :  " << group.GetSelectedNumBlobs()  << endl ;

		getch();
		
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Perimeter :  " << group.GetPerimeter(i)  << endl ;
		}

		getch();

	}//endif


/*

		cout << " \n\n   2eme Image  \n\n ";
		getch();

		//group.EmptyDetectedBlobsList();

		IplImage* imagetmp2;
		
	imagetmp2 = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);

//----------------------------------------------------------------
//	imagetmp = loadtiff("U:/images/blobs.tif");  //--> image fichier
//----------------------------------------------------------------
	
	cvCircle(imagetmp2,cvPoint(250,250),100,255,-1);
	cvCircle(imagetmp2,cvPoint(250,250),70,0,-1);
	cvCircle(imagetmp2,cvPoint(100,100),50,255,-1);
	cvRectangle (imagetmp2, cvPoint(300,50), cvPoint( (300+70) , (50+30)),255,-1);
	cvRectangle (imagetmp2, cvPoint(450,250), cvPoint( (450+ 111) , (250+ 111)),255,-1);
	cvCircle(imagetmp2,cvPoint(505,305),50,0,-1);
	cvEllipse (imagetmp2, cvPoint(500,50), cvSize( 70, 30),20,-180,180,255,-1);
	cvEllipse (imagetmp2, cvPoint(500,500), cvSize( 50, 38),65,-180,180,255,-1);
	cvEllipse (imagetmp2, cvPoint(150,500), cvSize( 120, 28),15,-180,180,255,-1);

	group.FindBlobs(imagetmp2);
	
	cout << " Total Number of Blobs found :  " << group.GetTotalNumBlobs()  << endl ;
	group.CalculateFeatures();

	for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Area :  " << group.GetArea(i)  << endl ;
		}

		cout << endl;

		group.SortByArea(10000);
		cout << " SortByArea(10000) : \n";
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Area :  " << group.GetArea(i)  << endl ;
		}

		cout << endl;

		group.SortByCompactness(0.5);
		cout << " SortByCompactness (0.5) :  \n";
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Compactness :  " << group.GetCompactness(i)  << endl ;
		}

		cout << endl;
		
		group.SortByPerimeter(200);
		cout << " SortByPerimeter(200) \n";
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Perimeter :  " << group.GetPerimeter(i)  << endl ;
		}

	cout << endl;
	getch();
	cout << endl;

		cout << " \n\n  IMAGE 3  \n\n ";
	
		IplImage* imagetmp3;
		
	imagetmp3 = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);

//----------------------------------------------------------------
//	imagetmp = loadtiff("U:/images/blobs.tif");  //--> image fichier
//----------------------------------------------------------------
	
	cvCircle(imagetmp3,cvPoint(250,250),100,255,-1);
	cvCircle(imagetmp3,cvPoint(250,250),70,0,-1);
	cvCircle(imagetmp3,cvPoint(100,100),50,255,-1);
	
	group.FindBlobs(imagetmp3);
	
	cout << " Total Number of Blobs found :  " << group.GetTotalNumBlobs()  << endl ;
	group.CalculateFeatures();

	for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Area :  " << group.GetArea(i)  << endl ;
		}

		cout << endl;

		group.SortByArea(10000);
		cout << " SortByArea(10000) : \n";
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Area :  " << group.GetArea(i)  << endl ;
		}

		cout << endl;
		getch();

*/

	cvReleaseImage (&imagetmp);
	cout << endl;
	//cvReleaseImage (&imagetmp2);
	//cvReleaseImage (&imagetmp3);
	//cvReleaseImage (&image);
	cout << "\n Images desallouees" << endl;

};

void tst1(void) 
{
	VisLine L1(1,-1,0), L2(1,-1,3), L3(0,0,0);

	L3 = L1;

	cout << "\n L3.a = " << L3.a << endl;
	cout << " L3.b = " << L3.b << endl;
	cout << " L3.c = " << L3.c << endl;
	
	cout << "\n  Test sur les droites \n\n ";
	cout << " Droites :  L1(1,-1,0) et L2(1,-1,3) " << endl; 
	cout << "\n Angle entre les doites : " << L1.GetAngle(L2) << " deg " << endl;
	cout << " Point d'intersection : P.x = " << (L1.GetIntersection(L2)).x << endl;
	cout << " Point d'intersection : P.y = " << (L1.GetIntersection(L2)).y << endl;

	if((L1.GetIntersection(L2)).x==-55555)
	{
		cout << "\n\n Les 2 droites sont paralleles. \n";
		cout << "\n Distance entre les droite = " <<  L1.GetDistance(L2)  << endl ;

	}
	if(L1.GetAngle(L2)==90)
	{
		cout << "\n\n Les 2 droites sont orthogonales. \n";
	}


};

void tst2(void) 
{
	ofstream ostr("Find_Perim_Clock_600.dat") ;
	clock_t start, finish;

	int i = 0;
	int j = 0;
	IplImage* imagetmp;
	imagetmp = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	VisBlobSet group;

	for( j=0 ; j<600 ; j++)
	{
		for( i=0 ; i<140 ; i++ )
		{
		
			cvCircle(imagetmp,cvPoint(384,288),(280-i*2),255,-1);
			cvCircle(imagetmp,cvPoint(384,288),(280-(i+1)*2),0,-1);
			i++;

			start = clock(); // start time

			group.FindBlobs(imagetmp);

			group.CalculatePerimeters();

			finish = clock(); // finish  time


			ostr << "\t " << group.GetTotalNumBlobs() << "  \t " << ((double)(finish - start) / CLOCKS_PER_SEC)  << "  \t  " << "\n";

			//group.EmptyDetectedBlobsList();
		}

		group.EmptyDetectedBlobsList();
		cout << "\n " << j ;

	}

	//savetiff(imagetmp,"Circles.tif");

	cout << "\n group \n";

	start = clock(); // start time

	group.FindBlobs(imagetmp);

	group.CalculatePerimeters();

	finish = clock(); // finish  time

	cout << "\n Number of clock ticks to find "<< group.GetTotalNumBlobs() << " blobs :" << (finish - start) << "  \n";
	cout << "\n Number of seconds to find "<< group.GetTotalNumBlobs() << " blobs :" << ((double)(finish - start) / CLOCKS_PER_SEC) << " seconds  \n";

	ostr.close();

	cvReleaseImage (&imagetmp);

};


void tst3(void) 
{

	ofstream ostr("Find_Area_Clock_Rand.dat") ;
	clock_t start, finish;

	int i = 0;
	int j = 0;
	int r = 6000;
	IplImage* imagetmp;
	imagetmp = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	VisBlobSet group;


	cvCircle(imagetmp,cvPoint(384,288),(280- 4),255,-1);
	cvCircle(imagetmp,cvPoint(384,288),(280- 8),0,-1);
	cvCircle(imagetmp,cvPoint(384,288),(280- 12),255,-1);
	cvCircle(imagetmp,cvPoint(384,288),(280- 16),0,-1);
	cvCircle(imagetmp,cvPoint(384,288),(280- 20),255,-1);
	cvCircle(imagetmp,cvPoint(384,288),(280- 24),0,-1);
	cvCircle(imagetmp,cvPoint(384,288),(280- 28),255,-1);

	group.FindBlobs(imagetmp);


//	for( j=0 ; j<100 ; j++)
//	{
		
		for( i=0 ; i< 200 ; i++)
		{
			
			r = (int)rand()/30;

			while( r > 5000 )
			{
				r = rand()/30;
			}

			cout << "\n i = " << i;
			cout << "\n r = " << r << endl;

			while( group.GetTotalNumBlobs() < r )
			{

				group.FindBlobs(imagetmp);
			}
		
			cout << "\n Blobs = " << group.GetTotalNumBlobs() << endl;
		
			start = clock(); // start time

			group.FindBlobs(imagetmp);

			group.CalculateAreas();

			finish = clock(); // finish  time


			ostr << "\t " << group.GetTotalNumBlobs() << "  \t " << ((double)(finish - start) / CLOCKS_PER_SEC)  << "  \t  " << "\n";

			group.EmptyDetectedBlobsList();
		
		}

//		cout << "\n " << j ;

//	}

	
	ostr.close();

	cvReleaseImage (&imagetmp);

};


void tst4(void) 
{

	int i = 0;
	double area_variance=0;

	int num_points=0;
	CvPoint2D32f target;
	CvPoint2D32f point;

	IplImage* image_source;
	IplImage* image;
		
	image_source = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	image = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);

//----------------------------------------------------------------
//	image_source = loadtiff("U:/Projet2/testblobset/testBlob_roue_2.tif");  //--> image fichier
//----------------------------------------------------------------

//----------------------------------------------------------------
//	image = loadtiff("U:/Projet2/testblobset/Roue_seuil.tif");  //--> image fichier
//----------------------------------------------------------------

//----------------------------------------------------------------
	image_source = loadtiff("U:/Projet2/testblobset/Ref_blob.tif");  //--> image fichier
//----------------------------------------------------------------

//----------------------------------------------------------------
	image = loadtiff("U:/Projet2/testblobset/Ref_blob_2.tif");  //--> image fichier
//----------------------------------------------------------------

//----------------------------------------------------------------
//	image_source = loadtiff("D:/images/images_seuillees/3_roues_seuil.tif");  //--> image fichier
//----------------------------------------------------------------

//----------------------------------------------------------------
//	image = loadtiff("D:/images/images_seuillees/3_roues_seuil_modif.tif");  //--> image fichier
//----------------------------------------------------------------

	
	//binarisation(image_source, image );
	//clean_morpho(IplImage *imgS, IplImage *imgD, IplImage *imgtmp );
	//savetiff(image,"testBlob_diamant.tif");

	double condition_number=0;
	VisBlobSet group;
	VisBlobSet group2;

	cout << "\n group \n";

	group.FindBlobs(image_source);
	
	cout << " Total Number of Blobs found :  " << group.GetTotalNumBlobs()  << endl ;
	
	if( group.CalculateFeatures() == 0 )
	{

		group.SelectByArea(250000 , 10);
		cout << " Number of selected Blobs  :  " << group.GetSelectedNumBlobs()  << endl ;

		group.SortByArea();

		cout << endl;
		getch();
	}

	group.SortByCompactness();
	target = group.GetCenterReal(0);


	//target.x += 5;
	//target.y += 5;

	cout << "\n Target Center Real x :  " << target.x  << endl ;
	cout << " Target Center Real y :  " << target.y  << endl ;
	


	for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Area :  " << group.GetArea(i)  << endl ;
		}

	for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Compactness :  " << group.GetCompactness(i)  << endl ;
		}


	//group.SelectByArea(6580, 6430);
	cout << "\n\n Number of selected Blobs  :  " << group.GetSelectedNumBlobs()  << endl ;
	
	group.SortByCompactness();
	
	for( i=1 ; i<group.GetSelectedNumBlobs()  ; i++ )
	{
		group.LoadReferenceBlob(i);
	}
	cout << "\n  Reference blobs have been loaded... \n";


	
	group.LearningReference( target , &condition_number);
	cout << " \n  Learning Reference done... \n\n ";
	cout << "\n\n Condition Number of A : " << condition_number << " \n";

	getch();

	num_points = (group.num_ref_blob_seq)[0];
	cout << "\n Nombre de points pour la référence = " << num_points << "  \n\n";

	for( i=0; i<num_points ;i++)
	{
		point = (*((*group.GetModelsSeq())[0].GetLearntPointsSeq()))[i];
		cout << "\nLearntPoint( " << i <<" ).x = " <<point.x <<endl;
		cout << "LearntPoint( " << i <<" ).y = " <<point.y <<endl;
	}
	  
	group2.FindBlobs(image);
	//group2.FindBlobs(image_source);
	group2.CalculateFeatures();
	group2.SortByCompactness();
	//area_variance = image->width*image->height/12;
	cout << "\n\n New BlobSet and New Image \n ";



	double error=0;

	target = group2.GetReference( &group, 0, area_variance , &condition_number , &error);
	cout << " \n  Calculating Reference done...   \n ";

	cout << "\n\n Condition Number of A : " << condition_number << " \n";
	cout << "\n\n Error on the calculation : " << error << " \n";

	num_points = (group.num_ref_blob_seq)[0];
	cout << "\n Nombre de points pour la référence = " << num_points << "  \n\n";

	for(i=0; i<num_points ;i++)
	{
		point = (*((*group.GetModelsSeq())[0].GetFoundPointsSeq()))[i];
		cout << "\nFoundPoint( " << i <<" ).x = " <<point.x <<endl;
		cout << "FoundPoint( " << i <<" ).y = " <<point.y <<endl;
	}

	cout << "\n Target Center Real x :  " << target.x  << endl ;
	cout << " Target Center Real y :  " << target.y  << endl ;

	getch();

	
	cvReleaseImage (&image_source);
	cvReleaseImage (&image);
	cout << endl;

	cout << "\n Images desallouees" << endl;

};
void tst5(void) 
{
	int i = 0;

	IplImage* imagetmp;
		
	imagetmp = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);

	//cvCircle(imagetmp,cvPoint(250,250),100,255,-1);
	
	cvRectangle (imagetmp, cvPoint(300,50), cvPoint( (300+70) , (50+30)),255,-1);


	IplROI ROI;
	ROI.coi=0;
	ROI.xOffset= 280;
	ROI.yOffset= 40;
	ROI.width=100;
	ROI.height=105;
	imagetmp->roi = &ROI;


	cout << "\n Pointer to ROI :  " << imagetmp->roi << endl ;
	cout << "\n Pointer to ROI :  " << &ROI << endl ;
	cout << " ROI.xOffset = "  << (*(imagetmp->roi)).xOffset << endl;
	cout << " ROI.yOffset = "  << (*(imagetmp->roi)).yOffset << endl;
	cout << " ROI.width = "  << (*(imagetmp->roi)).width << endl;
	cout << " ROI.height = "  << (*(imagetmp->roi)).height << endl;
	cout << endl << endl ;
	
	//savetiff(imagetmp,"testBlob_ROI.tif");

	VisBlobSet group;

	group.EmptyDetectedBlobsList();
	cout << "\n Empty detected blobs list \n" <<" Total Number of Blobs :  " << group.GetTotalNumBlobs()  << endl << endl ;

	cout << "\n New image with a ROI and 1 blob \n ";

	group.FindBlobs(imagetmp);
	
	cout << " Total Number of Blobs found :  " << group.GetTotalNumBlobs()  << endl ;
	

		group.CalculateFeatures();
		group.SelectByArea(200000 , 100);
		cout << " Number of selected Blobs  :  " << group.GetSelectedNumBlobs()  << endl ;

		
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Area :  " << group.GetArea(i)  << endl ;
		}

		cout << endl;

		
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Perimeter :  " << group.GetPerimeter(i)  << endl ;
		}

		cout << endl;
		getch();

		
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Compactness :  " << group.GetCompactness(i)  << endl ;
		}

		cout << endl;

		
		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Eccentricity :  " << group.GetEccentricity(i)  << endl ;
		}

		cout << endl;

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) EllipseMajorAxisLength :  " << group.GetEllipseMajorAxisLength(i)  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) EllipseMinorAxisLength :  " << group.GetEllipseMinorAxisLength(i)  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Center x :  " << (group.GetCenter(i)).x  << endl ;
			cout << " Blob( " <<  i  << " ) Center y :  " << (group.GetCenter(i)).y  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Center Real x :  " << (group.GetCenterReal(i)).x  << endl ;
			cout << " Blob( " <<  i  << " ) Center Real y :  " << (group.GetCenterReal(i)).y  << endl ;
		}

		cout << endl;
			cout << endl;

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Orientation :  " << (group.GetOrientation(i)) << " deg" << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Coeff A pour axe majeur :  " << (group.GetMajorAxis(i)).a  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Coeff B pour axe majeur :  " << (group.GetMajorAxis(i)).b  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) radius of minimum enclosing circle :  " << (group.GetMinimumEnclosingCircleRadius(i)) << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Center Real x of minimum enclosing circle :  " << (group.GetMinimumEnclosingCircleCenter(i)).x  << endl ;
			cout << " Blob( " <<  i  << " ) Center Real y of minimum enclosing circle :  " << (group.GetMinimumEnclosingCircleCenter(i)).y  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Center Real x of minimum area rectangle :  " << ((group.GetEquivalentRectangle(i)).center).x  << endl ;
			cout << " Blob( " <<  i  << " ) Center Real y of minimum area rectangle :  " << ((group.GetEquivalentRectangle(i)).center).y  << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Orientation of minimum area rectangle :  " << ((group.GetEquivalentRectangle(i)).angle)*180/3.14 << " deg " << endl ;
		}

		cout << endl;
		getch();

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Width of minimum area rectangle :  " << ((group.GetEquivalentRectangle(i)).size).width << endl ;
		}

		cout << endl;

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Height of minimum area rectangle :  " << ((group.GetEquivalentRectangle(i)).size).height << endl ;
		}


		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Minor Axis Length :  " << group.GetEllipseMinorAxisLength(i) << endl ;
		}

		for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Major Axis Length :  " << group.GetEllipseMajorAxisLength(i) << endl ;
		}

		cout << endl;

	
	group.EmptyDetectedBlobsList();
	
	cout << "\n Empty detected blobs list \n" <<" Total Number of Blobs :  " << group.GetTotalNumBlobs()  << endl ;

	//cvResetImageROI( imagetmp );
	imagetmp->roi=NULL;
	cvReleaseImage (&imagetmp);
	cout << endl;
	cout << "\n Images desallouees" << endl;
};


void tst6(void) 
{

	int i = 0;

	IplImage* image1;
	IplImage* image2;
	IplImage* image3;
	IplImage* imagetmp;
		
	image1 = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	image2 = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	image3 = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	imagetmp = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);

	
	image1 = loadtiff("C:/Documents and Settings/clebosse/My Documents/images/support_montre_2.tif"); 


	binarisation(image1, image2 );
	clean_morpho(image2, image3, imagetmp );
	savetiff(image3,"support_montre_2_seuil.tif");

	cvReleaseImage (&imagetmp);
	cvReleaseImage (&image1);
	cvReleaseImage (&image2);
	cvReleaseImage (&image3);
	cout << endl;
	cout << "\n Images desallouees" << endl;

};

void tst7(void) 
{

	ofstream ostr("area_comp_th_5_bis.dat");

	int i = 0;
	int j = 0;
	int threshold=0;

	IplImage* image1;
	IplImage* image2;
	IplImage* image3;
	IplImage* imagetmp;
		
	image1 = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	image2 = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	image3 = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	imagetmp = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);

	VisBlobSet group;
	double compactness=0;
	double area =0;
	double orientation=0;
	
	image1 = loadtiff("C:/Documents and Settings/clebosse/My Documents/images/rondelle_intensity_5.tif"); 
	//image1 = loadtiff("U:/Prive/Optimisation_seuillage/rect_intensity_5.tif"); 

	for(i=0 ; i<256 ; i++)
	{

		binarisation(image1, image2 , i);
		clean_morpho(image2, image3, imagetmp );
		group.FindBlobs(image3);
		cout << " Total Number of Blobs :  " << group.GetTotalNumBlobs()  << endl ;

		group.CalculateFeatures();
		group.SelectByArea(40000, 20000);

		cout << " Number of selected Blobs  :  " << group.GetSelectedNumBlobs()  << endl << endl ;
	
		group.SortByCompactness();

		for( j=0 ; j<group.GetSelectedNumBlobs()  ; j++ )
		{
			cout << " Blob( " <<  j  << " ) Area :  " << group.GetArea(j)  << endl ;
		}
		//getch();

		if( group.GetSelectedNumBlobs() > 0)
		{
			compactness=group.GetCompactness(0);
			area=group.GetArea(0);

			orientation=group.GetOrientation(0);
			//savetiff(image3,"rect_intensity_5_seuil.tif");

			cout << "\n area = " << area << endl;
			cout << "\n compactness = " << compactness << endl;
			//cout << "\n orientation = " << orientation << endl << endl;
		}

		ostr << " \t  " << area << " \t " << " \t " << " \t " <<  compactness << " \t " << " \t  "<< " \t " <<  orientation << " \t " << " \t  "<< " \t " << i << "\n";
	
	
		group.EmptyDetectedBlobsList();
	
		cout << "\n Empty detected blobs list \n" <<" Total Number of Blobs :  " << group.GetTotalNumBlobs()  << endl ;

		cout << "\n iteration = " << i << " \n\n" ;
		cout << "\n\n----------------------------------------------------\n\n";
		
		compactness=0;
		area=0;
		orientation=0;

	}

	getch();

	cvReleaseImage (&imagetmp);
	cvReleaseImage (&image1);
	cvReleaseImage (&image2);
	cvReleaseImage (&image3);
	cout << endl;
	cout << "\n Images desallouees" << endl;

	ostr.close();

};


void tst8(void) 
{

	int i = 0;
	double area_variance=0;

	int num_points=0;
	CvPoint2D32f target;
	CvPoint2D32f found_target;
	CvPoint2D32f point;

	IplImage* image_source;
	IplImage* image;
		
	image_source = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	image = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);

//----------------------------------------------------------------
	image_source = loadtiff("U:/Projet2/testblobset/Ref_blob.tif");  //--> image fichier
//----------------------------------------------------------------

//----------------------------------------------------------------
	image = loadtiff("U:/Projet2/testblobset/Ref_blob_2.tif");  //--> image fichier
//----------------------------------------------------------------


	VisBlobSet group;
	VisBlobSet group2;

	//cout << "\n group \n";

	group.FindBlobs(image_source);
	
	cout << " Total Number of Blobs found :  " << group.GetTotalNumBlobs()  << endl ;
	
	if( group.CalculateFeatures() == 0 )
	{

		group.SelectByArea(250000 , 10);
		//cout << " Number of selected Blobs  :  " << group.GetSelectedNumBlobs()  << endl ;

		group.SortByArea();

		cout << endl;
		//getch();
	}

	group.SortByCompactness();
	target = group.GetCenterReal(0);


	//target.x += 5;
	//target.y += 5;
/*
	cout << "\n Target Center Real x :  " << target.x  << endl ;
	cout << " Target Center Real y :  " << target.y  << endl ;
	


	for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Area :  " << group.GetArea(i)  << endl ;
		}

	for( i=0 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			cout << " Blob( " <<  i  << " ) Compactness :  " << group.GetCompactness(i)  << endl ;
		}
*/


	double condition_number=0;
	double error=0;


	for(int k=0 ; k < 100 ; k++ )
	{

		group.SortByCompactness();
	
		for( i=1 ; i<group.GetSelectedNumBlobs()  ; i++ )
		{
			group.LoadReferenceBlob(i);
		}

		cout << "\n  Reference blobs have been loaded... \n";

		group.LearningReference( target , &condition_number);
		cout << " \n  Learning Reference done... \n\n ";
		cout << "\n\n Condition Number of A : " << condition_number << " \n";

		getch();

		num_points = (group.num_ref_blob_seq)[0];
		cout << "\n Nombre de points pour la référence = " << num_points << "  \n\n";

		for( i=0; i<num_points ;i++)
		{
			point = (*((*group.GetModelsSeq())[0].GetLearntPointsSeq()))[i];
			//cout << "\nLearntPoint( " << i <<" ).x = " <<point.x <<endl;
			//cout << "LearntPoint( " << i <<" ).y = " <<point.y <<endl;
		}
	  
		group2.FindBlobs(image);
		//group2.FindBlobs(image_source);
		group2.CalculateFeatures();
		group2.SortByCompactness();
		//area_variance = image->width*image->height/12;
		//cout << "\n\n New BlobSet and New Image \n ";


		//getch();


		found_target = group2.GetReference( &group, 0, area_variance , &condition_number , &error);
		//cout << " \n  Calculating Reference done...   \n ";

		cout << "\n\n Condition Number of A : " << condition_number << " \n";
		cout << "\n\n Error on the calculation : " << error << " \n";

		num_points = (group.num_ref_blob_seq)[0];
		cout << "\n Nombre de points pour la référence = " << num_points << "  \n\n";

		/*
		for(i=0; i<num_points ;i++)
		{
			point = (*((*group.GetModelsSeq())[0].GetFoundPointsSeq()))[i];
			cout << "\nFoundPoint( " << i <<" ).x = " <<point.x <<endl;
			cout << "FoundPoint( " << i <<" ).y = " <<point.y <<endl;
		}
		*/
		cout << "\n Target Center Real x :  " << found_target.x  << endl ;
		cout << " Target Center Real y :  " << found_target.y  << endl ;
		
		//getch();


		(*group.GetModelsSeq())[0].ClearFoundPointsList();
		(*group.GetModelsSeq())[0].ClearLearntPointsList();
		group.ClearModelsList();

		cout << "\n\n iterations number =  " << k << " \n\n";
		cout << "\n------------------------------------------\n\n";

		//getch();
		found_target.x=0;
		found_target.y=0;
	}

	
	cvReleaseImage (&image_source);
	cvReleaseImage (&image);
	cout << endl;




};

void tst9(void) 
{

	int i = 0;
	int j = 0;
	int threshold=0;

	//clock_t start, finish;

	IplImage* image1;
	IplImage* image2;
	IplImage* image3;
	IplImage* imagetmp;
		
	image1 = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	image2 = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	image3 = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);
	imagetmp = cvCreateImage(cvSize(768,576),IPL_DEPTH_8U,1);

	VisBinarisation bin;
    int opt_threshold=0;
	char caract = 'C';
	
	image1 = loadtiff("C:/Documents and Settings/clebosse/My Documents/images/rondelle_intensity_5.tif"); 
	//image1 = loadtiff("U:/Prive/Optimisation_seuillage/rect_intensity_5.tif"); 

/*
//rect
	IplROI ROI;
	ROI.coi=0;
	ROI.xOffset= 280;
	ROI.yOffset= 100;
	ROI.width=325;
	ROI.height=325;
	image1->roi = &ROI;
	image2->roi = &ROI;
*/

//rondelle
	IplROI ROI;
	ROI.coi=0;
	ROI.xOffset= 260;
	ROI.yOffset= 114;
	ROI.width=260;
	ROI.height=260;
	image1->roi = &ROI;
	image2->roi = &ROI;


	bin.SetSourceImage(image1);

	cout << "\n SetSourceImage done... \n\n";

	bin.SetDestinationImage(image2);

	cout << "\n SetDestinationImage done... \n\n";

	bin.SetOptFeature(34000, 34000, caract);
	//bin.SetOptFeature(1, 34000);

	cout << "\n SetOptFeature done... \n\n";

	bin.InitializeOptBinarisation(10, 50, 150, 0.01);

	getch();

	cout << "\n InitializeOptBinarisation done...\n\n";

	cout << "\n Begin OptBinarisation \n\n";

		//start = clock(); // start time

	bin.OptBinarisation( &opt_threshold);

		//finish = clock(); // finish  time

	cout << "\n OptBinarisation done... \n\n";

	//cout  <<"\n\n " << (double)(finish - start)  << "  ms  " << "\n\n";


	getch();

	cout << " \n  Optmized Threshold =   "<< opt_threshold << " \n\n";

	getch();

	image1->roi=NULL;
	image2->roi=NULL;

	cvReleaseImage (&imagetmp);
	cvReleaseImage (&image1);
	cvReleaseImage (&image2);
	cvReleaseImage (&image3);
	cout << endl;
	cout << "\n Images desallouees" << endl;

};


int main(void)
  {
  char ans;
  do
    {
	cout << "\n\n 0: BlobSet functions test \n" << endl ;
	cout << " 1: Line functions test \n" << endl ;
	cout << " 2: Time execution test \n" << endl ;
	cout << " 3: Time execution test (random) \n" << endl ;
	cout << " 4: Reference calculation test \n" << endl ;
	cout << " 5: Roi test \n" << endl ;
	cout << " 6: Seuillage images \n" << endl ;
	cout << " 7: Tests Seuillage images \n" << endl ;
	cout << " 8: Time test for reference \n" << endl ;
	cout << " 9: Optimized Binarisation \n" << endl ;
    cout << "0-9 tst0->tst9" << endl;
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







