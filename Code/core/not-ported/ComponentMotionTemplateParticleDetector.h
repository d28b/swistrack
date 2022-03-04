#ifndef HEADER_ComponentMotionTemplateParticleDetector
#define HEADER_ComponentMotionTemplateParticleDetector

#include <opencv2/core.hpp>
//#include <cvaux.hpp>
extern "C" {
#include "camshift_wrapper.h"
}
#include "Component.h"
#include "DataStructureParticles.h"

//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
class ComponentMotionTemplateParticleDetector: public Component {

public:
	//! Constructor.
	ComponentMotionTemplateParticleDetector(SwisTrackCore * stc);
	//! Destructor.
	~ComponentMotionTemplateParticleDetector();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentMotionTemplateParticleDetector(mCore);
	}

private:
	DataStructureParticles::tParticleVector mParticles;



	void update_mhi( cv::Mat* img, cv::Mat* dst, cv::Mat * foregroundMask, double timestampIn,
			 int diff_threshold, wxDateTime frameTimestamp );



	cv::Mat * mOutputImage;

	// motion template variables

	// various tracking parameters (in seconds)
	double mMhiDuration;
	double mSegmentThreshold;
	int mDiffThreshold;
	double mMaxTimeDelta;
	double mMinTimeDelta;
	// number of cyclic frame buffer used for motion detection
	// (should, probably, depend on FPS)
	const int N;

	// ring image buffer
	cv::Mat **buf;
	int last;
	cv::Mat *mhi; // MHI
	cv::Mat *orient; // orientation
	cv::Mat *mask; // valid orientation mask
	cv::Mat *segmask; // motion segmentation map

	cv::Mat *mHsv; // hsv image
	CvMemStorage* storage; // temporary storage
	double firstTimestamp;

	cv::Mat *mForegroundMask; // MHI

	cv::Mat * mInputChannels[3];

	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.

	/**
	 * Draws the histogram in the image.
	 * If target is null, creates a new image, otherwise draws in target.
	 * Returns the image that it created, or that was passed in.
	 */
	static cv::Mat * DrawHistogram1D(CvHistogram * histogram, cv::Mat * hist_image=NULL);
};

#endif

