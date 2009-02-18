#ifndef HEADER_ComponentOpticalFlowPyrLKTracking
#define HEADER_ComponentOpticalFlowPyrLKTracking

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"





//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
class ComponentOpticalFlowPyrLKTracking: public Component {

public:
	//! Constructor.
	ComponentOpticalFlowPyrLKTracking(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOpticalFlowPyrLKTracking();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentOpticalFlowPyrLKTracking(mCore);
	}

private:

	int mMaxCorners;
	int mWinSize;
	IplImage * mLastFrame;
	IplImage * mThisFrame;
	IplImage * mOutputFrame;

	int mCornerCount;

	CvPoint2D32f* mCorners;
	CvPoint2D32f* mCornersB;

	IplImage * eig_image;
	IplImage * tmp_image;

	char * mFeaturesFound;
	float * mFeatureErrors;
	IplImage * pyrA;
	IplImage * pyrB;



	/**
	 * Filter tracks we aren't using any more. 
	 */
	void FilterTracks();
	void DataAssociation();
	void ClearDistanceArray();
	void InitializeTracks();
	double GetCost(const Track & track, CvPoint2D32f p);
	void AddPoint(int i, CvPoint2D32f p);

	DataStructureParticles::tParticleVector *particles;
	std::map<int, double*> distanceArray;
	unsigned int maxParticles;
	int mNextTrackId;

	// Parameters
	double mMaxDistanceSquared; //!< (configuration) The maximum distance between a track and a particle to attach
	double mMinNewTrackDistanceSquared; //!< (configuration) The minimum distance between new tracks
	int mFrameKillThreshold; //!< (configuration) The number of frames a point doesn't get updated before it gets destroyed. 
	int mTrackDistanceKillThresholdSquared; //!< (configuration) The distance between tracks before one should be dropped. 
	DataStructureTracks::tTrackMap mTracks;
	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.

};

#endif

