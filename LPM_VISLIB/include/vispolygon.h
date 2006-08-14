#ifndef vispolygon_h        
#define vispolygon_h
//****************************************************************************/
// Filename :   $RCSfile: vispolygon.h,v $
// Version  :   $Revision: 2.8 $
//
// Author   :   $Author: produit $
//              Yuri Lopez de Meneses // Pierre Roduit
//              Ecole Polytechnique Federale de Lausanne
//              IPR-LPM-VISION
//              CH-1015 Lausanne
//              Switzerland
//
//
// Date     :   $Date: 2006/07/14 16:14:21 $


/** \file vispolygon.h

    \brief Definition of the "Vispolygon" Class wich help to work on polygons
*/


/** \mainpage The Class Vispolygon is used to create polygons and to work on them.
	The polygons can be created from points or CV Contour.
	Some interesting functions like ConvexHull or GetMinimalBoundingBox, wich 
	can be used in the image processing.
	For image processing,the interest to work on polgons is to be faster than
	working on the complete image. In exemple, the minimal bounding box can be 
	calculated very fast.

    
    \author $Author: produit $
    \version $Revision: 2.8 $
    \date $Date: 2006/07/14 16:14:21 $
*/


// See end of this header file for usage notes
/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/
 
#include <iostream>
#include <math.h>
#include <vector>
#include "cv.h"

using namespace std;

/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/

#ifdef linux
  #define VISAPI 
#else
	#ifndef VISAPI
		#ifdef _USRDLL // Compile a DLL
			#define VISAPI __declspec(dllexport) 
		#else
            #ifdef _LIB  // Compile Static library
			  #define VISAPI
            #else  // Link against a DLL
              #define VISAPI
            #endif
		#endif
	#endif
#endif

/*****************************************************************************/
/******************************** types **************************************/
/*****************************************************************************/

typedef CvPoint2D32f CvVector;

/*****************************************************************************/
/******************************** prototypes *********************************/
/*****************************************************************************/








//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//	Vis Extreme
//********************************************************************************
/*! \brief Structure containing the extrama value and indexes
 *
 *	The VisExtreme structure contains the value and the indexes of the polygpon
 *	extrema points for the x and y axis. It contains also the area of the bounding
 *	box mounted on the x and y axis.
 *	All this value are returned from the VisPolygon::GetExtremePoints
 *	
 */
typedef struct VisExtreme
{
  // Values	 
  float xmax;
  float xmin;
  float ymax;
  float ymin;

  // indexes
  int xmax_ind;
  int xmin_ind;
  int ymax_ind;
  int ymin_ind;

  float area;

} VisExtreme;


//********************************************************************************
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//********************************************************************************
//	Vis Polygon
//********************************************************************************
/*! \brief The VisPolygon Class is used to work on polygons.
 *
 *	The VisPolygon Class is used to work on polygons. It gives functions to create
 *	Polygons from CVcontour or points. You can find also basic functions like
 *	scaling, translating or rotating and more complex function like 
 *	GetMinimalBoundingBox or ConvexHull
 *	
 */
class VISAPI VisPolygon
{
 private:

	/********
	Variables
	********/

	/*! \var List of the Polygon's Points*/
	CvPoint2D32f* the_points;  
	/*! \var Number of Points*/
	int num_points;			   
	/*! \var Center of Gravity*/
	CvPoint2D32f cg;		   
	
	/********
	Functions
	********/
	
	//inline function to create cvVector
	inline CvVector cvVector(float v1,float v2) {return (CvVector) cvPoint2D32f(v1,v2);}
	inline CvVector cvVector(CvPoint2D32f p2, CvPoint2D32f p1){return (CvVector) cvPoint2D32f(p2.x-p1.x,p2.y-p1.y);}
	//return mid index of two indexes...
	inline int Midway(int a, int b, int n){if (a<b) return (a+b)/2;else return ((a+b+n)/2)%n;}
	//Scalar Product
	inline float Dot(CvVector v1, CvVector v2){return v1.x*v2.x+v1.y*v2.y;}

	//Function to to get the extremum of the point and indexes
	VisExtreme GetExtremePoints();
	//Get the extreme point in a direction between to indexes of the Polygon
    int Extreme(CvVector dir, int a, int b,bool orientation);
	//Test if a convex polygon is clockwise or counterclockwise.
	int GetOrientation();
	//Simplify Douglas-Peucker
	void simplifyDP(float tol, CvPoint2D32f* v, int j, int k, int* mk);

 public:
	
	//Constructors and desctructors
	VisPolygon();
	VisPolygon(CvPoint2D32f* p, int npoints);
	VisPolygon(CvSeq *contour);
	VisPolygon(VisExtreme v);
	~VisPolygon();

	//Geometric manipulation function
	void Translate(CvPoint2D32f t);
	void Rotate(double degrees);
	void RotateCG(double degrees);
	void Rotate(double degrees, CvPoint2D32f pivot);
	bool Scale(double factor);
	bool ScaleCG(double fator);
	bool Scale(double factor, CvPoint2D32f pivot);


	//Test if a point is inside a polygon
	bool IsInside(CvPoint2D32f p);

	//Easy Geometric Function
	int GetSize();
	CvPoint2D32f GetCenter();
	double GetPerimeter();
	double GetArea();
	CvPoint2D32f GetSecondMoment();

	//Complex Geometric Function
	vector<CvPoint2D32f> IntersectionLine(CvPoint2D32f point,CvVector direction);

	//Approximate the polygon with less points
	VisPolygon ApproxPolygon(double precision);
	//Get the four extrema in a given direction, optimized for big polygons
	CvBox2D OrientedExtrema(CvVector dir,int* top,int* bottom, int* left, int* right);
	//Get the four extrema in a given direction, optimized for small polygons
	CvBox2D OrientedExtremaSlow(CvVector dir,int* top,int* bottom, int* left, int* right);
	//Get the Oriented bounding box
	CvBox2D OrientedBox(CvVector dir,int top,int bottom, int left, int right);
	//Give the Convex Hull of a Polygon
	VisPolygon ConvexHull();
	//Return Vertical Bounding Box
	CvBox2D GetBoundingBox();
	//Return the Minimal Area Bounding Box (every orientation)
	CvBox2D GetMinBoundingBox();

	//Operators redefinition
	VisPolygon& operator=(const VisPolygon& rhs);
	CvPoint2D32f operator[](const int i);
	friend ostream& operator<<(ostream& o, const VisPolygon& v);

	//Output Function
	void Dump(ostream& out); 
	void Draw(IplImage* img,CvScalar color=CV_RGB(255,255,255), int labels=0);
	void DrawBox(IplImage* img,CvBox2D box, CvScalar color=CV_RGB(255,255,255));
};




/*****************************************************************************/
/******************************** usage notes ********************************/
/*****************************************************************************/
#endif
