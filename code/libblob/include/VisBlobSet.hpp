
#ifndef VISBLOBSET_HPP        
#define VISBLOBSET_HPP
//****************************************************************************/
// Filename :   $RCSfile : VisBlobSet.hpp, v $
// Version  :   $Revision : 1.0 $
//
// Author   :   $Author: yuri $
//              Cyrille Lebossé
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
//
//
// Date     :   $Date: 2004/03/24 13:23:39 $
// See end of this header file for usage notes

/*****************************************************************************/
/** \file VisBlobSet.hpp

    \brief Definition of the "VisBlobSet class" which enables to find blobs in binarised image, to sort 
         the created list and to calculate some of their features.
*/


/**
    
     The "VisBlobSet class" enables to detect blobs
	 in a binarised image, to gather them in a list,
	 to sort this list and to calculate their features.
	
		The binarised image can be defined with a Region Of Interest(ROI).
 	
 		It is possible indeed to select some rectangular part of the image or a certain color plane in the
 		image, and process only this part. The selected rectangle is called "Region of
 		Interest". The structure "IplImage" contains the field roi for this purpose. If the
 		pointer is not NULL, it points to the structure "IplROI" that contains parameters of selected
 		ROI, otherwise a whole image is considered selected.

 		If a ROI is defined, blobs are only searched in the ROI.

	 \author $Author: yuri $
     \version $Revision: 1.2 $
     \date $Date: 2004/03/24 13:23:39 $


*/

/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/

#include <iostream>
//#include <conio.h>
#include <cv.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>

#include "VisLine.hpp"
#include "VisBlob.hpp"
#include "VisGeomModel.hpp"


/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/

using namespace std; //So that we can use standard library names


/*****************************************************************************/
/** \file VisBlobSet.hpp

    \brief Definition of the "VisBlobSet class" which enables to find blobs in binarised image, to sort 
         the created list and to calculate some of their features.
*/


/**
    
     The "VisBlobSet class" enables to detect blobs
	 in a binarised image, to gather them in a list,
	 to sort this list and to calculate their features.
	
		The binarised image can be defined with a Region Of Interest(ROI).
 	
 		It is possible indeed to select some rectangular part of the image or a certain color plane in the
 		image, and process only this part. The selected rectangle is called "Region of
 		Interest". The structure "IplImage" contains the field roi for this purpose. If the
 		pointer is not NULL, it points to the structure "IplROI" that contains parameters of selected
 		ROI, otherwise a whole image is considered selected.

 		If a ROI is defined, blobs are only searched in the ROI.

	 \author $Author: yuri $
     \version $Revision: 1.2 $
     \date $Date: 2004/03/24 13:23:39 $


*/
 

/*****************************************************************************/
/******************************** types **************************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** prototypes *********************************/
/*****************************************************************************/

class VisBlobSet
{
	/** private members*/
	private:

			/** \brief Detected Blobs list */
			vector<VisBlob> blob_seq; 
			/** \brief Selected Blobs list (temporary) */
			vector<VisBlob> blob_temp; 
			/** \brief Memory storage for Blob contour */
			CvMemStorage* mem;
			/** \brief Sorting Blobs private method */
			int Sort(void);

			/** \brief Reference models and target points list */
			vector<VisGeomModel> models_seq;
			/** \brief Reference Blobs list (1 list for all reference models)  !!!!!!   ONLY ONE REFERENCE CAN BE MADE  FOR ONE VISBLOBSET  -> another list of reference blobs should be created for another reference  !!!!!!   */
			vector<VisBlob> ref_blob_seq;
			

			/** \brief Sort a vector of Blobs according to the angle of the segment between blob center and gravity center of all blobs */
			int SortByAngle(vector<VisBlob>* p_blob_list);



	/** public members*/
	public:
		/** \brief Constructor */
		VisBlobSet(); 

		/** \brief Destructor */
		~VisBlobSet();	

		/** \brief Number of reference Blobs list (Number of reference Blobs for each reference model) */
		vector<int> num_ref_blob_seq;

		/** \brief Accessor to the Reference Blobs list */
		vector<VisBlob>* GetRefBlobSeq(){ return(&ref_blob_seq); }

		/** \brief Accessor to the Reference models and target points list */
		vector<VisGeomModel>* GetModelsSeq(){ return(&models_seq); }

		/** \brief Clear Reference models list and reference blobs list */
		void ClearModelsList(void){ models_seq.clear(); ref_blob_seq.clear(); num_ref_blob_seq.clear(); }


		
		/** \brief Find all blobs in a binarised image */
		int FindBlobs(IplImage *imgS);

		/** \brief Returns number of selected blobs */
		int GetSelectedNumBlobs();

		/** \brief Returns total number of detected blobs */
		int GetTotalNumBlobs();
		
		/** \brief Empties the detected Blobs list */
		int EmptyDetectedBlobsList(void);
		

		/** \brief Sorts blobs in a descending order thanks to their area or
		 considering the distance between BlobArea and the given area*/
		int SortByArea(double area = 0);
		/** \brief Sorts blobs in a descending order thanks to their perimeter or
		 considering the distance between BlobPerimeter and the given perimeter*/
		int SortByPerimeter(double perimeter = 0);
		/** \brief Sorts blobs in a descending order thanks to their compactness or
		 considering the distance between BlobCompactness and the given compactness*/
		int SortByCompactness(double compactness = 0);
		/** \brief Sorts blobs in a descending order thanks to their eccentricity or
		 considering the distance between BlobEccentricity and the given eccentricity*/
		int SortByEccentricity(double eccentricity = 0);
		/** \brief Sorts blobs in a descending order thanks to their EllipseMajorAxisLength or
		 considering the distance between BlobMajorAxisLength and the given EllipseMajorAxisLength*/
		int SortByEllipseMajorAxisLength(double EllipseMajorAxisLength = 0);
		/** \brief Sorts blobs in a descending order thanks to their EllipseMinorAxisLength or
		 considering the distance between BlobMinorAxisLength and the given EllipseMinorAxisLength*/
		int SortByEllipseMinorAxisLength(double EllipseMinorAxisLength = 0);
		/** \brief Sorts blobs in a descending order thanks to their orientation or
		 considering the distance between BlobOrientation and the given orientation*/
		int SortByOrientation(double oriention = 0);
		/** \brief Sorts blobs in a ascending order considering the distance between blob center and the given CvPoint */
		int SortByPosition(CvPoint2D32f point);

		
		/** \brief Selects blobs whose area is within the given limits */
		int SelectByArea(double max , double min);
		/** \brief Selects blobs whose perimeter is within the given limits */
		int SelectByPerimeter(double max , double min);
		/** \brief Selects blobs whose compactness is within the given limits */
		int SelectByCompactness(double max , double min);
		/** \brief Selects blobs whose eccentricity is within the given limits */
		int SelectByEccentricity(double max , double min);
		/** \brief Selects blobs whose Ellipse Major Axis Length is within the given limits */
		int SelectByEllipseMajorAxisLength(double max , double min);
		/** \brief Selects blobs whose Ellipse Minor Axis Length is within the given limits */
		int SelectByEllipseMinorAxisLength(double max , double min);
		/** \brief Selects blobs whose orientation is within the given limits */
		int SelectByOrientation(double max , double min);
		/** \brief Selects all the detected blobs */
		int SelectAll(void);
		/** \brief Selects only the "blobnum"th blob of the selected blobs list */
		int Select(int blobnum = 0);
		/** \brief Erases only the "blobnum"th blob of the selected blobs list */
		int Erase(int blobnum = 0);

		/** \brief Sorts blobs in a descending order according to their similarity 
 			with the first selected blob of the given BlobSet */
		int BlobFitting(VisBlobSet* pblobset, double area_variance=0);
		
		/** \brief Calculates all the features of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateFeatures(void);
		/** \brief Calculates area of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateAreas(void);
		/** \brief Calculates perimeter of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculatePerimeters(void);
		/** \brief Calculates compactness of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateCompactness(void);
		/** \brief Calculates center of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateCenters(void);
		/** \brief Calculates orientation (in deg) of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateOrientations(void);
		/** \brief Calculates eccentricity of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateEccentricities(void);
		/** \brief Calculates "equivalent" Ellipse Major Axis Length of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateEllipseMajorAxisLengths(void);
		/** \brief Calculates "equivalent" Ellipse Minor Axis Length of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateEllipseMinorAxisLengths(void);
		/** \brief Calculates Major Axis of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateMajorAxis(void);
		/** \brief Calculates Minor Axis of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateMinorAxis(void);
		/** \brief Calculates Major Axis length of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateMajorAxisLength(void);
		/** \brief Calculates Minor Axis length of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateMinorAxisLength(void);
		/** \brief Calculates ellipse similarity factor of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateEllipseSimFacts(void);
		

		/** \brief Calculates the minimum enclosing circle of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateMinimumEnclosingCircle(void);
		/** \brief Calculates the minimum area circumscribed rectangle of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateEquivalentRectangle(void);

		/** \brief Calculates the bounding rectangle of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateBoundingRectangle(void);
		

		/** \brief Returns area of the "blobnum"th blob among selected blobs */
		double GetArea(int blobnum = 0);
		/** \brief Returns perimeter of the "blobnum"th blob among selected blobs */
		double GetPerimeter(int blobnum = 0);
		/** \brief Returns compactness of the "blobnum"th blob among selected blobs */
		double GetCompactness(int blobnum = 0);
		/** \brief Returns ellipse similarity factor of the "blobnum"th blob among selected blobs */
		double GetEllipseSim(int blobnum = 0);
		/** \brief Returns center (with a precision of 1 pixel)  of the "blobnum"th blob among selected blobs */
		CvPoint GetCenter(int blobnum = 0);
		/** \brief Returns real center of the "blobnum"th blob among selected blobs */
		CvPoint2D32f GetCenterReal(int blobnum = 0);
		/** \brief Returns orientation (in deg) of the "blobnum"th blob among selected blobs */
		double GetOrientation(int blobnum = 0);
		/** \brief Returns eccentricity of the "blobnum"th blob among selected blobs */
		double GetEccentricity(int blobnum = 0);
		/** \brief Returns major axis length of the equivalent ellipse of the "blobnum"th blob among selected blobs */
		double GetEllipseMajorAxisLength(int blobnum = 0);
		/** \brief Returns minor axis length of the equivalent ellipse of the "blobnum"th blob among selected blobs */
		double GetEllipseMinorAxisLength(int blobnum = 0);


		/** \brief Returns the radius of the minimum enclosing circle of the "blobnum"th blob among selected blobs */
		float GetMinimumEnclosingCircleRadius(int blobnum = 0);
		/** \brief Returns the real center of the minimum enclosing circle of the "blobnum"th blob among selected blobs */
		CvPoint2D32f GetMinimumEnclosingCircleCenter(int blobnum = 0);
		/** \brief Draws the minimum enclosing circle of the selected blobs in the given destination image */
		int DrawMinimumEnclosingCircle( IplImage *dstimg=0, int color=255, int thickness=1);
		/** \brief Returns the minimum area circumscribed rectangle for the "blobnum"th blob among selected blobs */
		CvBox2D GetEquivalentRectangle(int blobnum = 0);
		/** \brief Draws the "equivalent" rectangle of the selected blobs in the given destination image */
		int DrawEquivalentRectangle( IplImage *dstimg=0, int color=255, int thickness=1);

		/** \brief Draws the bounding rectangle of the selected blobs in the given destination image */
		int DrawBoundingRectangle( IplImage *dstimg=0, int color=255, int thickness=1);


		/** \brief Returns major axis of the "blobnum"th blob among selected blobs */
		VisLine GetMajorAxis(int blobnum = 0);
		/** \brief Returns major axis of the "blobnum"th blob among selected blobs */
		VisLine GetMinorAxis(int blobnum = 0);
		/** \brief Returns major axis length of the "blobnum"th blob among selected blobs */
		double GetMajorAxisLength(int blobnum = 0);
		/** \brief Returns minor axis length of the "blobnum"th blob among selected blobs */
		double GetMinorAxisLength(int blobnum = 0);

		/** Returns Bounding Rectangle of the "blobnum"th blob among selected blobs */
		CvRect GetBoundingRectangle(int blobnum = 0);
		
		/** Returns an int indicating if the blob is near from an elllipse. */
		int GetEllipse_Similarity(int blobnum = 0);

		/** \brief Draws contours of the selected blobs in the given destination image  */
		int DrawContours(IplImage *dstimg, double external_color, double hole_color, int max_level=0, int thickness=1, int connectivity=8);

		/** \brief Draws a cross of the center of the "blobnum"th blob among selected blobs in the given destination image */
		int DisplayCross(IplImage *dstimg, int color, int thickness, int size, int blobnum = 0);


		/** \brief Sets the reference blobs list by taking the "blobnum"th blob among selected blobs as a reference*/
		int LoadReferenceBlob(int blobnum = 0);

		/** \brief Learns the Reference from the given point and the reference blobs list   !!!!!!   ONLY ONE REFERENCE CAN BE MADE  FOR ONE VISBLOBSET  -> another list of reference blobs should be created for another reference  !!!!!! */
		int LearningReference( CvPoint2D32f target , double* p_cond_number=NULL);

		/** \brief Calculates the target point from the learnt reference created with the given BlobSet where blobs have been searched in a new image */
		CvPoint2D32f GetReference( VisBlobSet* pblobset , int num_ref=0 , double area_variance=0 , double* p_cond_number=NULL, double* p_error=NULL);

		/** \brief Returns the angle of the segment constituted by the "blobnum"th blob center and the given point  */
		double GetSegmentAngle( CvPoint2D32f point, int blobnum = 0 );

		/** \brief Returns the list of blobs containing the given point  */
		int IsPointInsideBlob(CvPoint point, vector<int>* p_blob_list);

};

/*****************************************************************************/
/******************************** usage notes ********************************/
/*****************************************************************************/
/*

*/
#endif
