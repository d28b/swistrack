#ifndef HEADER_Track
#define HEADER_Track

#include <vector>
#include <cmath>
#include <cv.h>

class Track {

private:
	int maxlength;     //!< Stores the maximum length of the trajectory to plot
	double GetDist(CvPoint2D32f *p1, CvPoint2D32f *p2);
	//CvScalar color;
	int mLastUpdateFrame;



public:
	void SetCritPoint(CvPoint2D32f* p);
	int mID;
	CvHistogram * mColorModel;
	std::vector<CvPoint2D32f> trajectory; /*< Info about objects being tracked */
	/** Critical point */
	CvPoint2D32f critpoint;
	/** Constructor - id number id*/
	Track(int idnumber);
	Track();
	/** Add a point to the current track (max track) */
	void AddPoint(CvPoint2D32f p, int lastUpdateFrame);
	void SetMaxLength(int length);
	int LastUpdateFrame() {
	  return mLastUpdateFrame;
	}

	/** The track takes ownership of the color model. */
	void SetColorModel(CvHistogram * model);

	/** Destructor */
	~Track();
};

#endif
