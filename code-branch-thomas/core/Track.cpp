#include "Track.h"

/** \brief Constructor - id number id*/
Track::Track(int idnumber,int n_objects) 
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
	mID=idnumber;
	critpoint.x=-1; critpoint.y=-1;
};

Track::~Track() {
}

/** \brief Add a point to the current track (max track) */
void Track::AddPoint(CvPoint2D32f p){
	trajectory.push_back(p);
	if((int)(trajectory.size()) > maxlength)
		trajectory.erase(trajectory.begin());
}

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
