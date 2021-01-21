#include "Track.h"

Track::Track() {
}

/** \brief Constructor - id number id*/
Track::Track(int idnumber) :  mLastUpdateFrame(0), mColorModel(0), trajectory(0) {
	maxlength = 50; // maximum length of trajectory to store
	//printf("%f %f\n",trajectory.at(0).x,trajectory.at(0).y);
	mID = idnumber;
	critpoint.x = -1;
	critpoint.y = -1;
};

Track::~Track() {

}

/** \brief Add a point to the current track (max track) */
void Track::AddPoint(CvPoint2D32f p, int frameNumber) {
	trajectory.push_back(p);
	if (maxlength != -1 && (int)(trajectory.size()) > maxlength)
		trajectory.erase(trajectory.begin());
	mLastUpdateFrame = frameNumber;

}

void Track::SetMaxLength(int length) {
  maxlength = length;
}

void Track:: SetColorModel(CvHistogram * model) {
  cvReleaseHist(&mColorModel);
  mColorModel = model;
}

void Track::SetCritPoint(CvPoint2D32f *p) {
	critpoint.x = p->x;
	critpoint.y = p->y;
}

double Track::GetDist(CvPoint2D32f *p1, CvPoint2D32f *p2) {
	return (
	           (p1->x - p2->x)*
	           (p1->x - p2->x)
	           +
	           (p1->y - p2->y)*
	           (p1->y - p2->y)
	       );
}
