#ifndef HEADER_ComponentColorHistogramParticles
#define HEADER_ComponentColorHistogramParticles

#include <cv.h>
//#include <cvaux.hpp>
extern "C" {
#include "camshift_wrapper.h"
}
#include "Component.h"
#include "DataStructureParticles.h"

//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
class ComponentColorHistogramParticles: public Component {

public:
	//! Constructor.
	ComponentColorHistogramParticles(SwisTrackCore *stc);
	//! Destructor.
	~ComponentColorHistogramParticles();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentColorHistogramParticles(mCore);
	}

private:
	void EraseTrack(int id);
	void UpdateTrackers(IplImage * inputImage);
	void AddNewTracks(IplImage * inputImage);
	void FilterTracks();
	void ClearTracks();
	int mNextTrackId;
	DataStructureParticles::tParticleVector mParticles;
	DataStructureTracks::tTrackMap mTracks;
	std::map<int, camshift> mTrackers;  // track id to tracker

	void  update_mhi( IplImage* img, IplImage* dst, double timestamp,
	                  int diff_threshold );

	int mDiffThreshold; //!< (configuration) The diff_threshold parameter.

	int mInitialWindowSize; //!< (configuration) The size of the initial window when you initialize a new tracker.

	unsigned int mMaximumNumberOfTrackers;

	IplImage * mOutputImage;
	int mFrameKillThreshold;
	int mTrackDistanceKillThresholdSquared;
	int mMinNewTrackDistanceSquared;

	// motion template variables

	// various tracking parameters (in seconds)
	double mMhiDuration;
	double mSegmentThreshold;
	double mMaxTimeDelta;
	double mMinTimeDelta;
	// number of cyclic frame buffer used for motion detection
	// (should, probably, depend on FPS)
	const int N;

	// ring image buffer
	IplImage **buf;
	int last;
	IplImage *mhi; // MHI
	IplImage *orient; // orientation
	IplImage *mask; // valid orientation mask
	IplImage *segmask; // motion segmentation map
	CvMemStorage* storage; // temporary storage
	double firstTimestamp;

	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.
};

#endif

