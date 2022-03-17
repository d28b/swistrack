#include "ComponentTrackSmoothing.h"
#define THISCLASS ComponentTrackSmoothing

using namespace std;
#include <iostream>

#include "DisplayEditor.h"

THISCLASS::ComponentTrackSmoothing(SwisTrackCore * stc):
	Component(stc, wxT("TrackSmoothing")),
	mDisplayOutput(wxT("Output"), wxT("Smoothing")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureTracks));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentTrackSmoothing() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mWindowSize = GetConfigurationInt(wxT("WindowSize"), 3);
}

void THISCLASS::OnStep() {
	DataStructureTracks::tTrackMap * trackMap = mCore->mDataStructureTracks.mTracks;
	if (! trackMap) {
		AddError(wxT("No input tracks."));
		return;
	}

	// Process new points
	int frameNumber = mCore->mDataStructureInput.mFrameNumber;
	for (auto & entry : *trackMap) {
		auto & track = entry.second;

		// Get or create the state
		auto & state = mState[track.mID];
		if (! state.track) state.track = &mTracks[track.mID];
		state.seen = true;

		auto & newPoint = track.mTrajectory.back();
		if (newPoint.frameNumber != frameNumber) {
			// Add a gap
			state.gaps += 1;
			continue;
		}

		// Fill the gaps
		auto & previousPoint = state.points.back();
		for (int i = 0; i < state.gaps; i++) {
			float x = previousPoint.x + (newPoint.x - previousPoint.x) / (state.gaps + 1) * i;
			float y = previousPoint.y + (newPoint.y - previousPoint.y) / (state.gaps + 1) * i;
			state.AddPoint(x, y, previousPoint.frameNumber + i, mWindowSize);
		}

		state.gaps = 0;

		// Add the new point
		state.AddPoint(newPoint.x, newPoint.y, newPoint.frameNumber, mWindowSize);

		// Shorten to the length of the original track
		state.track->Shorten(track.mTrajectory.size());
	}

	// Remove states of tracks not seen any more
	for (auto it = mState.begin(); it != mState.end(); ) {
		if (! it->second.seen) {
			mTracks.erase(it->first);
			it = mState.erase(it);
		} else {
			it->second.seen = false;
			it++;
		}
	}

	// Set the new tracks
	mCore->mDataStructureTracks.mTracks = &mTracks;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureInput.mImage);
		de.SetTrajectories(true);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

THISCLASS::State::State(): gaps(0), points(), track(NULL) {
}

void THISCLASS::State::AddPoint(float x, float y, int frameNumber, unsigned int windowSize) {
	// Remove points larger than the window size
	while (points.size() >= windowSize)
		points.pop_front();

	// Add the new point to the window
	points.push_back(TrackPoint(x, y, frameNumber));

	// Add a new point to the track
	double xSum = 0;
	double ySum = 0;
	for (auto & point : points) {
		xSum += point.x;
		ySum += point.y;
	}

	float xAverage = xSum / points.size();
	float yAverage = ySum / points.size();
	track->AddPoint(xAverage, yAverage, frameNumber);
}
