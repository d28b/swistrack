//****************************************************************************/
// Filename :   $RCSfile: track.cpp,v $
// Version  :   $Revision: 1.3 $
//
// Author   :   $Author: correll $
//              Yuri L. de Meneses
//              Ecole Polytechnique Federale de Lausanne
//              IPR-LPM-VISION
//              CH-1015 Lausanne
//              Switzerland
//
//
// Date     :   $Date: 2005/08/26 16:54:23 $


/** \file track.h
*	\brief Header of Track class
*/

/** \class Track
*
* \brief Stores information about objects being tracked.
* 
*/

// See end of this header file for usage notes
/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/


#include "Track.h"

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







	/** \brief Constructor - id number id*/
	Track::Track(int idnumber,TrackingImage* trackingimg,int n_objects) 
		{

		int r=128,g=128,b=128;

		if(idnumber>2*(n_objects)/3){
			r=255/(idnumber+1-2*n_objects/3)*3;
		}
		else if(idnumber>(n_objects)/3){
			g=255/(idnumber+1-n_objects/3)*3;
		}
		else{
			b=255/(idnumber+1)*3;
		}
		color=CV_RGB(r,g,b);

		maxlength=50; // maximum length of trajectory to store
		//printf("%f %f\n",trajectory.at(0).x,trajectory.at(0).y);
		id=idnumber;
		this->trackingimg=trackingimg;
		critpoint.x=-1; critpoint.y=-1;
		};
	
	/** \brief Add a point to the current track (max track) */
	void Track::AddPoint(CvPoint2D32f p){
		trajectory.push_back(p);
		if((int)(trajectory.size()) > maxlength)
			trajectory.erase(trajectory.begin());
		}
	
	

	/** \brief Draw current trajectory */
	void Track::DrawTrajectory(int style, int length){
		vector<CvPoint2D32f>::iterator it;
		vector<CvPoint2D32f>* traj;


		traj = &trajectory;
		
		
		if(style >= TRAJCROSS && style != TRAJNOCROSS){
			DrawCross(                    // draw the cross on the video feed
				cvPointFrom32f(             //
				traj->back()),      //
				20,color        //
				);
			}
		
		if(style >= TRAJNONUMBER){
			for( it = traj->begin()+1; it != traj->end()-length; it++ ){
				if(GetDist(&(*(it)),&(*(it-1)))<1600)
				//CV_RGB(id*255/num_objects,(255-id)*255/num_objects,((120-id)*255+120)/num_objects)
					trackingimg->Line(*it,*(it-1),color);
				}
			}
		if(style >= TRAJNOCROSS){
			char smove[3];
			sprintf(smove, "%d", id);
			CvPoint pt = cvPointFrom32f(             //
				traj->front());
			
			trackingimg->Text(smove,pt,color);
			}
		}
	
		/** \brief Draws a cross on an image
		*
		* \param img: image
		* \param center: cross position
		* \param width: cross width
		* \param color: grayscale intensity or CV_RGB( r, g, b ).
	*/
	void Track::DrawCross(CvPoint center, int width , CvScalar color)
		{
		
		trackingimg->Line(cvPoint2D32f( center.x-(int)(width/2),center.y),
			cvPoint2D32f( center.x+(int)(width/2),center.y), color);
		trackingimg->Line(cvPoint2D32f( center.x,center.y-(int)(width/2)),
			cvPoint2D32f( center.x,center.y+(int)(width/2)), color);
		}
	
	
    /* Destructor */
    Track::~Track(void) {
        if (trajectory.size()) trajectory.clear();
		};
	


void Track::SetCritPoint(CvPoint2D32f *p)
{
	critpoint.x=p->x;
	critpoint.y=p->y;
}

double Track::GetDist(CvPoint2D32f *p1, CvPoint2D32f *p2)
{
	return(
		(p1->x-p2->x)*
		(p1->x-p2->x)
		+
		(p1->y-p2->y)*
		(p1->y-p2->y)
		);
}
