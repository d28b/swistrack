#ifndef HEADER_Track
#define HEADER_Track

#include <vector>
#include <cmath>
#include <cv.h>

class Track {

private:
	int maxlength;     //!< Stores the maximum length of the trajectory to plot
	double GetDist(CvPoint2D32f *p1, CvPoint2D32f *p2);
	CvScalar color;

public:
	void SetCritPoint(CvPoint2D32f* p);
	int mID;					/*< Holds ID number of track */
	std::vector<CvPoint2D32f> trajectory; /*< Info about objects being tracked */
	/** Critical point */
	CvPoint2D32f critpoint;
	/** Constructor - id number id*/
	Track(int idnumber, int n_objects);
	/** Add a point to the current track (max track) */
	void AddPoint(CvPoint2D32f p);

	/** Destructor */
	~Track();
};

#endif
