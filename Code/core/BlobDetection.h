#ifndef HEADER_BlobDetection
#define HEADER_BlobDetection

#include <opencv2/core.hpp>
#include "Component.h"
#include "DataStructureParticles.h"

class BlobDetection {
private:
	int mThreshold;					//!< (configuration) Threshold.
	unsigned int mMaxNumber;		//!< (configuration) The maximum number of blobs that are to detect.

	bool mSelectionByArea; 			//!< (configuration) If the selection is based on the blob area
	int mMinArea;					//!< (configuration) The minimum area of a blob.
	int mMaxArea;					//!< (configuration) The maximum area of a blob.

	bool mSelectionByCompactness;	//!< (configuration) If the selection is based on the blob compactness
	double mMinCompactness;			//!< (configuration) The minimum compactness of a blob.
	double mMaxCompactness;			//!< (configuration) The maximum compactness of a blob.

	bool mSelectionByOrientation;	//!< (configuration) If the selection is based on the blob orientation
	double mMinOrientation;			//!< (configuration) The minimum orientation of a blob.
	double mMaxOrientation;			//!< (configuration) The maximum orientation of a blob.

	static const int NEXT_SIBLING = 0;
	static const int PREV_SIBLING = 1;
	static const int FIRST_CHILD = 2;
	static const int PARENT = 3;

public:
	std::vector<std::vector<cv::Point>> mSelectedContours;	//!< The contours that correspond to the criteria.
	DataStructureParticles::tParticleVector mParticles;		//!< The detected particles.

	BlobDetection();
	~BlobDetection() { }

	//! Reads the configuration
	void ReadConfiguration(Component * component, const wxString prefix);
	//! Find blobs on a binary image.
	void FindBlobs(cv::Mat inputImage, wxDateTime frameTimestamp, int frameNumber);
	//! Draws the contours onto a color image.
	void DrawContours(cv::Mat outputImage);
};

#endif
