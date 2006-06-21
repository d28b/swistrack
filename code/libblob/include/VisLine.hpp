#ifndef VISLINE_h        
#define VISLINE_h
//****************************************************************************/
// Filename :   $RCSfile : Line.hpp,v $
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
// Date     :   $Date: 2004/07/12 12:01:48 $

/*! \class VisLine Line.hpp
 *  \brief Definition of the "VisLine class" which contains a line and the methods
 *			which enable to calculate lines intersection, angle between 2 lines
 *			and distance between 2 parallel lines.
 *
 *   Purpose  : Definition of the "VisLine class" which contains a line and the methods
 *			which enable to calculate lines intersection, angle between to lines
 *			and distance between 2 parallel lines.
 *
 *	Line equation : \f[  A*x+B*y+C=0 \f]
 *
 */



// See end of this header file for usage notes
/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/

#include <cv.h>
#include <math.h>


/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/
#define PI 3.14159265358979323846

/*****************************************************************************/
/******************************** prototypes *********************************/
/*****************************************************************************/

/*! \class VisLine Line.hpp
 *  \brief Definition of the "VisLine class" which contains a line and the methods
 *			which enable to calculate lines intersection, angle between 2 lines
 *			and distance between 2 parallel lines.
 *
 *   Purpose  : Definition of the "VisLine class" which contains a line and the methods
 *			which enable to calculate lines intersection, angle between to lines
 *			and distance between 2 parallel lines.
 *
 *	Line equation : \f[  A*x+B*y+C=0 \f]
 *
 */

/*****************************************************************************/
/******************************** types **************************************/
/*****************************************************************************/


class VisLine
{

	/** private members*/
	private:
	
	/**public members*/
	public:

		/** Line equation : A*x+B*y+C=0  */
		double a;
		double b;
		double c;

		/** Default constructor */
		VisLine(); 

		/**  Constructor
		 *
		 *	Line equation : \f[  A*x+B*y+C=0 \f]
		 *
		 *	\param A,B,C : Line equation parameters
		 *		
		 */ 
		VisLine(double A, double B, double C){ a=A; b=B; c=C; }; 
	   
		/* Constructor */
		VisLine(CvPoint p1, CvPoint p2);

		/* Constructor */
		VisLine(CvPoint2D32f p1, CvPoint2D32f p2);

        /* Constructor */
		VisLine(CvPoint p1, double angle);

		/** Destructor */
		~VisLine();        

		VisLine &operator=(const VisLine &line2); // overload of the operator '='

		/** Returns angle between 2 lines (in deg) */
		double GetAngle(VisLine line2);
		/** Returns intersection point of 2 lines */
		CvPoint2D32f GetIntersection(VisLine line2);
		/** Returns distance between 2 lines if they are parallel */
		double GetDistance(VisLine line2);

		/** Draw the line on an image */
		int Draw(IplImage* img, double color=255, int thickness=1);
		

};




/*****************************************************************************/
/******************************** usage notes ********************************/
/*****************************************************************************/
/*

*/
#endif
