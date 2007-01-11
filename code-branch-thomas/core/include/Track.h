#ifndef track_h        
#define track_h
//****************************************************************************/
// Filename :   $RCSfile: track.h,v $
// Version  :   $Revision: 1.4 $
//
// Author   :   $Author: correll $
//              Ecole Polytechnique Federale de Lausanne
//              CH-1015 Lausanne
//              Switzerland
//
//
// Date     :   $Date: 2005/08/26 16:54:23 $


/** \file track.h

  Header of Track class
*/

/** 

  \class Track

  Stores trajectory of objects being tracked.
  Implements Kalman filtering on the trajectories.
  
    \author $Author: correll $
    \version $Revision: 1.4 $
    \date $Date: 2005/08/26 16:54:23 $
*/

// See end of this header file for usage notes
/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/


#include <vector>
#include <math.h>
#include <cv.h>
#include "TrackingImage.h"
#include "OldComponent.h"

using namespace std;

/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/


/*****************************************************************************/
/******************************** types **************************************/
/*****************************************************************************/



/*****************************************************************************/
/******************************** prototypes *********************************/
/*****************************************************************************/


class Track : public OldComponent
	{
	
	
	private:
		
		
		int maxlength;     /*< Stores the maximum length of the trajectory to plot */
		TrackingImage* trackingimg;
		double GetDist(CvPoint2D32f *p1, CvPoint2D32f *p2);
		CvScalar color;

	public:
		void SetCritPoint(CvPoint2D32f* p);
//		int touch;
		int id;					/*< Holds ID number of track */
		vector<CvPoint2D32f> trajectory; /*< Info about objects being tracked */
		/** Critical point */
		CvPoint2D32f critpoint;		
		/** Constructor - id number id*/
		Track(int idnumber,TrackingImage* trackingimg,int n_objects);	
		/** Add a point to the current track (max track) */
		void AddPoint(CvPoint2D32f p);
		
		
#define TRAJCROSS 0
#define TRAJNONUMBER 1
#define TRAJFULL 3
#define TRAJNOCROSS 2
#define COVERAGE 4
		
		void DrawTrajectory(int style,int length=0);
		void DrawCross(CvPoint center, int width , CvScalar color);
				
		/** Destructor */
		~Track(void);
		
	};	
/*****************************************************************************/
/******************************** usage notes ********************************/
/*****************************************************************************/
/**

  \todo An Empty() function.
  
	\test See test0() in bidon.cpp
	
*/
#endif
