#include "ComponentCamShiftTracking.h"
#define THISCLASS ComponentCamShiftTracking

#include "DisplayEditor.h"
#include "Utility.h"
#include <limits>
#include <iostream>
#include <set>

using namespace std;

THISCLASS::ComponentCamShiftTracking(SwisTrackCore *stc):
		Component(stc, wxT("CamShiftTracking")),
		mNextTrackId(0), mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("Tracking"))
{
	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	Initialize();						// Read the XML configuration file
}

THISCLASS::~ComponentCamShiftTracking()
{

}

void THISCLASS::OnStart()
{
	mVmin = GetConfigurationInt(wxT("VMin"), 60);
	mSmin = GetConfigurationInt(wxT("SMin"), 50);
	mHistogramDimensions = GetConfigurationInt(wxT("HistogramDimensions"), 1);
	for (std::map<int, camshift>::iterator i = mTrackers.begin();
	        i != mTrackers.end(); i++) {
		setVmin(&i->second, mVmin);
		setSmin(&i->second, mSmin);
	}

	// load other parameters:
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration()
{
	mInitialWindowSize = GetConfigurationInt(wxT("InitialWindowSize"), 60);
	mFrameKillThreshold = GetConfigurationInt(wxT("FrameKillThreshold"), 10);


	mMinNewTrackDistanceSquared =
	    pow(GetConfigurationDouble(wxT("MinNewTrackDistance"), 100), 2);
	mTrackDistanceKillThresholdSquared =
	    pow(GetConfigurationDouble(wxT("TrackDistanceKillThreshold"), 10), 2);

	mMaximumNumberOfTrackers = GetConfigurationInt(wxT("MaximumNumberOfTrackers"), 4);
}
void THISCLASS::UpdateTrackers(IplImage * inputImage)
{
	for (std::map<int, camshift>::iterator i = mTrackers.begin();
	        i != mTrackers.end(); i++) {
		CvBox2D box = track(&i->second, inputImage);
		// if the new point is too close to the old point, don't add it.
		// if the tracker stops moving significantly, it will get deleted.
		if (Utility::SquareDistance(box.center, mTracks[i->first].trajectory.back()) >
		        pow(5, (float) 2)) {
			float dist = Utility::SquareDistance(mTracks[i->first].trajectory.back(),
			                                     box.center);
			if (dist > 0.01) {
				mTracks[i->first].AddPoint(box.center,
				                           mCore->mDataStructureInput.mFrameNumber);
				Particle p;
				p.mCenter = box.center;

				p.mID = i->first;
				p.mArea = -1;
				p.mCompactness = -1;
				p.mOrientation = -1;
				p.mTimestamp = mCore->mDataStructureInput.FrameTimestamp();
				p.mFrameNumber = mCore->mDataStructureInput.mFrameNumber;
				mParticles.push_back(p);
			}

		}
	}
}
void THISCLASS::AddNewTracks(IplImage * inputImage) {

	if (mTracks.size() >= mMaximumNumberOfTrackers) {
		// already have enough trackers, so punt.
		return;
	}

	if (mCore->mDataStructureParticles.mParticles == NULL) {
		AddError(wxT("No input particles!"));
		return;
	}
	const DataStructureParticles::tParticleVector & particles = *mCore->mDataStructureParticles.mParticles;
	for (DataStructureParticles::tParticleVector::const_iterator pIt =
	            particles.begin();
	        pIt != particles.end(); pIt++) {
		assert(pIt->mID == -1); // (particle should not be associated)
		float minSquareDist = std::numeric_limits<float>::max();
		Track * closestTrack = NULL;
		for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin(); i != mTracks.end(); i++) {
			float distance = Utility::SquareDistance(i->second.trajectory.back(),
			                 pIt->mCenter);
			if (distance < minSquareDist) {
				minSquareDist = distance;
				closestTrack = &i->second;
			}
		}
		if ((closestTrack == NULL) || (minSquareDist > mMinNewTrackDistanceSquared)) {
			int id = mNextTrackId++;
			mTracks[id] = Track(id);
			mTracks[id].AddPoint(pIt->mCenter,
			                     mCore->mDataStructureInput.mFrameNumber);
			Particle p;
			p.mCenter = pIt->mCenter;
			p.mID = id;
			p.mArea = pIt->mArea;
			p.mCompactness = pIt->mCompactness;
			p.mOrientation = pIt->mOrientation;
			p.mTimestamp = pIt->mTimestamp;
			p.mFrameNumber = pIt->mFrameNumber;
			mParticles.push_back(p);

			mTrackers[id] = camshift();
			createTracker(&mTrackers[id], inputImage, mHistogramDimensions);
			setVmin(&mTrackers[id], mVmin);
			setSmin(&mTrackers[id], mSmin);
			CvRect start = Utility::RectByCenter(pIt->mCenter.x,
			                                     pIt->mCenter.y,
			                                     mInitialWindowSize, mInitialWindowSize);
			startTracking(&mTrackers[id], inputImage, &start);
		}
	}
}

void THISCLASS::FilterTracks()
{
	set<int> trackIdsToErase;
	for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin();
	        i != mTracks.end(); i++) {
		Track & track = i->second;
		int frameDifference = mCore->mDataStructureInput.mFrameNumber - track.LastUpdateFrame();
		if ( frameDifference >= mFrameKillThreshold) {
			trackIdsToErase.insert(i->first);
		}
	}


	for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin();
	        i != mTracks.end(); i++) {
		Track & track1 = i->second;
		for (DataStructureTracks::tTrackMap::iterator j = mTracks.begin();
		        j != mTracks.end(); j++) {
			Track & track2 = j->second;
			double cost = Utility::SquareDistance(track1.trajectory.back(), track2.trajectory.back());
			if (track1.mID != track2.mID && cost < mTrackDistanceKillThresholdSquared) {
				// kill a track - keep the older one
				if (track1.mID < track2.mID) {
					trackIdsToErase.insert(track2.mID);
				} else {
					trackIdsToErase.insert(track1.mID);
					break;
				}
			}
		}
	}
	for (set<int>::iterator i = trackIdsToErase.begin();
	        i != trackIdsToErase.end(); i++) {
		EraseTrack(*i);
	}

}


void THISCLASS::OnStep()
{
	wxTimeSpan timeSinceLastFrame = mCore->mDataStructureInput.TimeSinceLastFrame();
	if (timeSinceLastFrame.IsLongerThan(wxTimeSpan::Seconds(5))) {
		cout << "Clearing tracks because there was a gap: " << timeSinceLastFrame.Format().ToAscii() << endl;
		ClearTracks();
	}
	IplImage *inputImage = mCore->mDataStructureImageColor.mImage;
	if (! inputImage) {
		AddError(wxT("No input image."));
		return;
	}
	mParticles.clear();
	UpdateTrackers(inputImage);

	AddNewTracks(inputImage);
	FilterTracks();

	// update the tracks store locally to this component
	mCore->mDataStructureTracks.mTracks = &mTracks;
	mCore->mDataStructureParticles.mParticles = &mParticles;
	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		if (mOutputImage == NULL) {
			mOutputImage = cvCloneImage(inputImage);
		}

		if (mTrackers.size() == 0) {
			cvCopy(mCore->mDataStructureInput.mImage, mOutputImage);
		} else {
		  //mOutputImage = cvCloneImage(mTrackers.begin()->second.pHueImg);
			//cvCopy(inputImage, mOutputImage);
		  cout << "Tracker: " << mTrackers.begin()->first << endl;
		  cvCvtColor(mTrackers.begin()->second.pProbImg, mOutputImage, CV_GRAY2BGR);
		  //cvCopy(mCore->mDataStructureInput.mImage, mOutputImage);
		}

		for (std::map<int, camshift>::iterator i = mTrackers.begin();
		        i != mTrackers.end(); i++) {
			if (i->second.faceBox.size.height >= 0 &&
			        i->second.faceBox.size.width >= 0) {
				cvEllipseBox(mOutputImage, i->second.faceBox,
				             CV_RGB(255, 0, 0), 3, CV_AA, 0 );
			}
		}

		de.SetMainImage(mOutputImage);
		de.SetTrajectories(true);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
	cvReleaseImage(&mOutputImage);
	mOutputImage = 0;
	ClearTracks();
}

void THISCLASS::ClearTracks() {
	mTracks.clear();
	for (std::map<int, camshift>::iterator i = mTrackers.begin();
	        i != mTrackers.end(); i++) {
		releaseTracker(&i->second);
	}
	mTrackers.clear();

}
void THISCLASS::EraseTrack(int id) {
	mTracks.erase(id);
	releaseTracker(&mTrackers[id]);
	mTrackers.erase(id);
}
