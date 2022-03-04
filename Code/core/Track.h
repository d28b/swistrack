#ifndef HEADER_Track
#define HEADER_Track

#include <vector>
#include <cmath>
#include <opencv2/core.hpp>

class Track {

private:
	int maxlength;     //!< Stores the maximum length of the trajectory to plot
	double GetDist(cv::Point2f * p1, cv::Point2f * p2);
	//cv::Scalar color;
	int mLastUpdateFrame;

public:
	void SetCritPoint(cv::Point2f * p);
	int mID;
	cv::Mat mColorModel;
	std::vector<cv::Point2f> trajectory; /*< Info about objects being tracked */
	/** Critical point */
	cv::Point2f critpoint;
	/** Constructor - id number id*/
	Track(int idnumber);
	Track();
	/** Add a point to the current track (max track) */
	void AddPoint(cv::Point2f p, int lastUpdateFrame);
	void SetMaxLength(int length);
	int LastUpdateFrame() {
		return mLastUpdateFrame;
	}

	/** The track takes ownership of the color model. */
	void SetColorModel(cv::Mat colorModel);

	/** Destructor */
	~Track();
};

#endif
