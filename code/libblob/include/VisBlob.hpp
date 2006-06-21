#ifndef VisBlob_h        
#define Visblob_h
//****************************************************************************/
// Filename :   $RCSfile : Visblob.hpp,v $
// Version  :   $Revision : 1.0 $
//
// Author   :   $Author: yuri $
//              Cyrille  Lebossé
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
//
//
// Date     :   $Date: 2003/12/17 02:03:50 $

/*! \class VisBlob VisBlob.hpp
 *  \brief Definition of the "VisBlob class" which contains the blob contour
 *			and its main features. This class must NOT be called by the user.
 *			It is an "internal" class called by the "VisBlobSet" class. You have to call
 *			this one to detect blobs in a binarised image.
 *
 *    Definition fo the "VisBlob class" where some blob features
 *	  are defined with the methods which enable to handle them.
 *	  This class must not be handled directly by the user. It is
 *	  internally used by the "VisBlobSet" class which is the one 
 *	  the user has to call in order to detect blobs in a binarised image
 *	  and calculate their features.
 *
 *	
 *	 \author $Author: yuri $
 *   \version $Revision: 1.1.1.1 $
 *   \date $Date: 2003/12/17 02:03:50 $
 *
 */



// See end of this header file for usage notes
/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/

#include <iostream>
#include <cv.h>
#include <math.h>
#include "VisLine.hpp"


/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/
//#define PI 3.14159265

/*****************************************************************************/
/******************************** prototypes *********************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** types **************************************/
/*****************************************************************************/

/*! \class VisBlob VisBlob.hpp
 *  \brief Definition of the "VisBlob class" which contains the blob contour
 *			and its main features. This class must NOT be called by the user.
 *			It is an "internal" class called by the "VisBlobSet" class. You have to call
 *			this one to detect blobs in a binarised image.
 *
 *    Definition fo the "VisBlob class" where some blob features
 *	  are defined with the methods which enable to handle them.
 *	  This class must not be handled directly by the user. It is
 *	  internally used by the "VisBlobSet" class which is the one 
 *	  the user has to call in order to detect blobs in a binarised image
 *	  and calculate their features.
 *
 *	
 *	 \author $Author: yuri $
 *   \version $Revision: 1.1.1.1 $
 *   \date $Date: 2003/12/17 02:03:50 $
 *
 */

class VisBlob
{

	/** private members*/
	private:

		/** Pointer on the sequence of points which make the contour */
		CvSeq* contour;   

		/** Region of interest in the given image and where blobs are searched */
		IplROI roi;
		
		/** Area of the blob */
		double area;        

		/** Perimeter of the blob */
		double perimeter;   

		/** Center coordinates of the blob (whith a precision of 1 pixel) */
		CvPoint centre ;     
		/** Real center coordinates of the blob */
		CvPoint2D32f centre_real; 

		/** Real center coordinates of the minimum enclosing circle */
		CvPoint2D32f min_enclo_circle_centre; 
		/** Radius of the minimum enclosing circle */
		float min_enclo_circle_radius;
		
		/**  Minimum area circumscribed rectangle for the blob */
		CvBox2D equivalent_rect;

		/** compactness = 4*Pi*area/(perimeter*perimeter)  */
		/** --> compacness=1 for a circle */
		/** --> compactness=0 for highly elongated shape */ 
		double compactness;  

		/** Orientation of the blob (in deg) */
		double orientation;

		/** Eccentricity of the blob */
		double eccentricity;

		/** Major Axis Length of the equivalent ellipse of the blob */
		double ellipsemajoraxislength;

		/** Minor Axis Length of the equivalent ellipse of the blob */
		double ellipseminoraxislength;

		/** Major Axis Length of the blob */
		double majoraxislength;

		/** Minor Axis Length of the blob */
		double minoraxislength;

		/** Minor Axis of the blob */
		VisLine  minoraxis;

		/** Minor Axis of the blob */
		VisLine  majoraxis;

		/** Bounding rectangle of the blob */
		CvRect bounding_rect;

		/** Ellipse similarity factor */
		double ellipse_sim; 

		/** Sortkey use to sort blobs */
		double key; 

		/** Angle of the segment constituted by the blob center and a given point */
		double segment_angle;


	/**public members*/
	public:

		/** Default constructor */
		VisBlob(); 
		/** Constructor from a given contour */
		VisBlob(CvSeq* c,_IplROI* r=NULL); 

		/** Destructor */
		~VisBlob();        


		/** \brief Returns the angle of the segment constituted by the blob center and the given point  */
		double GetSegmentAngle( CvPoint2D32f point );

		/**\brief Sets blob eccentricity */
		void SetEccentricity(double eccent){ eccentricity=eccent;}
		void SetCompactness(double comp){ compactness=comp;}
		
		/** \brief Returns blob contour */
		CvSeq* GetContour(){return contour;}
		/** \brief Returns blob area */
		double GetArea(){return area;}
		/** \brief Returns blob perimeter */
		double GetPerimeter(){return perimeter;}
		/** \brief Returns blob center */
		CvPoint GetCentre(){return centre;}
		/** \brief Returns real blob center */
		CvPoint2D32f GetCentreReal(){return centre_real;}
		/** \brief Returns blob compactness */
		double GetCompactness(){return compactness;}
		/** \brief Returns blob orientation */
		double GetOrientation(){ return orientation; };
		/** \brief Returns real center coordinates of the minimum enclosing circle */
		CvPoint2D32f GetMinEncloCircleCenter(){ return min_enclo_circle_centre; };
		/** \brief Returns radius of the minimum enclosing circle */
		float GetMinEncloCircleRadius(){ return min_enclo_circle_radius; };
		/** \brief Returns minimum area circumscribed rectangle for the blob */
		CvBox2D GetEquivalentRect(){ return equivalent_rect; };
		/** \brief Returns blob eccentricity */
		double GetEccentricity(){return eccentricity;}
		/** \brief Returns major axis length of the equivalent ellipse of the blob */
		double GetEllipseMajorAxisLength(){return ellipsemajoraxislength;}
		/** \brief Returns minor axis length of the equivalent ellipse of the blob */
		double GetEllipseMinorAxisLength(){return ellipseminoraxislength;}
		/** \brief Returns blob minor axis */
		VisLine GetMinorAxis(){return minoraxis;}
		/** \brief Returns blob major axis */
		VisLine GetMajorAxis(){return majoraxis;}
		/** \brief Returns major axis length of the blob */
		double GetMajorAxisLength(){return majoraxislength;}
		/** \brief Returns minor axis length of the blob */
		double GetMinorAxisLength(){return minoraxislength;}

		/** \brief Returns ellipse similarity factor */
		double GetEllipseSim(){return ellipse_sim;}

		/** \brief Returns blob major axis */
		CvRect GetBoundingRectangle(){return bounding_rect;}


		/** \brief Sets the sortkey with the given feature value */
		void SetKey(double nkey){ key = nkey; }
		/** \brief Returns sortkey value */
		double GetKey(void) const { return key; }

		/** Calculates blob area */
		void Calc_Area(void);
		/** Calculates blob perimeter */
		void Calc_Perimeter(void);
		/** Calculates blob center */
		void Calc_Centre(void);
		/** Calculates blob compactness */
		void Calc_Compactness(void);
		/** Calculates blob orientation */
		void Calc_Orientation(void);
		/** Calculates the minimum enclosing circle of the blob */
		void Calc_MinEncloCircle(void);
		/** Calculates the minimum area circumscribed rectangle for the blob */
		void Calc_EquivalentRect(void);
		/** Calculates blob eccentricity */
		void Calc_Eccentricity(void);
		/** Calculates major axis length of the equivalent ellipse of the blob */
		void Calc_EllipseMajorAxisLength(void);
		/** Calculates minor axis length of the equivalent ellipse of the blob */
		void Calc_EllipseMinorAxisLength(void);
		/** Calculates blob minor axis */
		void Calc_MinorAxis(void);
		/** Calculates blob major axis */
		void Calc_MajorAxis(void);
		/** Calculates major axis length of the blob */
		void Calc_MajorAxisLength(void);
		/** Calculates minor axis length of the blob */
		void Calc_MinorAxisLength(void);

		/** Calculates ellipse similarity factor */
		void Calc_EllipseSim(void);

		/** Calculates blob bounding rectangle */
		void Calc_BoundingRectangle(void);


		/** \brief Operator "<" to sort blobs in a descending order 
			The function "std::sort" is used to sort blobs. It sorts thanks to
 			the operator "<". That's why here its action has been reversed. So
 			blobs are sorted in descending order. This sort is based on the sortkey
 			value.*/
		bool operator<(const VisBlob& a) const {return ( this->GetKey() > a.GetKey() );}


};




/*****************************************************************************/
/******************************** usage notes ********************************/
/*****************************************************************************/
/*

*/
#endif
