#include "ComponentNearestNeighborTracking.h"
#define THISCLASS ComponentNearestNeighborTracking

#include "DisplayEditor.h"

THISCLASS::ComponentNearestNeighborTracking(SwisTrackCore * stc):
	Component(stc, wxT("NearestNeighborTracking")),
	mMaxDistance2(25), mMaxLostTrackFrames(10), mMaxNumberOfTracks(10), mMaxNumberOfPoints(50),
	mTracks(), mNextTrackID(0),
	mDisplayOutput(wxT("Output"), wxT("Tracking")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentNearestNeighborTracking() {
	if (mTracks.size()) mTracks.clear();
}

void THISCLASS::OnStart() {
	// Start with a fresh set of tracks
	mNextTrackID = 0;
	mTracks.clear();

	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	double maxDistance = GetConfigurationDouble(wxT("MaxDistance"), 25);
	mMaxDistance2 = maxDistance * maxDistance;

	mMaxLostTrackFrames = GetConfigurationInt(wxT("MaxLostTrackFrames"), 10);
	mMaxNumberOfTracks = GetConfigurationInt(wxT("MaxNumberOfTracks"), 10);
	mMaxNumberOfPoints = GetConfigurationInt(wxT("MaxNumberOfPoints"), 50);
}

class Association {
public:
	Track * track;
	Particle * particle;
	double cost;

	Association(Track * track, Particle * particle): track(track), particle(particle) {
		auto & point = track->mTrajectory.back();
		double dx = point.x - particle->mCenter.x;
		double dy = point.y - particle->mCenter.y;
		cost = dx * dx + dy * dy;
	}

	static bool CompareArea(const Association & a, const Association & b) {
		return a.cost < b.cost;
	}
};

void THISCLASS::OnStep() {
	int frameNumber = mCore->mDataStructureInput.mFrameNumber;
	DataStructureParticles::tParticleVector * particles = mCore->mDataStructureParticles.mParticles;
	if (! particles) {
		AddError(wxT("No particles."));
		return;
	}

	// Note that particles already associated (mID != -1) are ignored.

	// Check if we have lost tracks
	for (auto it = mTracks.begin(); it != mTracks.end(); ) {
		auto & lastPoint = it->second.mTrajectory.back();
		if (lastPoint.frameNumber + mMaxLostTrackFrames < frameNumber) {
			it = mTracks.erase(it);
		} else {
			it++;
		}
	}

	// Calculate the cost matrix of all particles with all tracks
	std::vector<Association> associations;
	for (auto & particle : *particles)
		if (particle.mID == -1)
			for (auto & entry : mTracks)
				associations.push_back(Association(&entry.second, &particle));

	// Sort by cost
	std::sort(associations.begin(), associations.end(), Association::CompareArea);

	// Associate these particles
	for (auto & association : associations) {
		// Cost too high
		if (association.cost > mMaxDistance2) break;

		// Already associated
		if (association.particle->mID != -1) continue;
		auto & lastPoint = association.track->mTrajectory.back();
		if (lastPoint.frameNumber == frameNumber) continue;

		// Associate
		association.track->AddPoint(association.particle->mCenter.x, association.particle->mCenter.y, frameNumber);
		association.particle->mID = association.track->mID;
	}

	// Shorten the tracks if necessary
	for (auto & entry : mTracks)
		entry.second.Shorten(mMaxNumberOfPoints);

	// Create new tracks with non-associated particles
	for (auto & particle : *particles) {
		if (mTracks.size() >= mMaxNumberOfTracks) break;
		if (particle.mID != -1) continue;

		Track track;
		track.mID = mNextTrackID;
		track.AddPoint(particle.mCenter.x, particle.mCenter.y, frameNumber);
		mTracks[track.mID] = track;
		particle.mID = mNextTrackID;
		mNextTrackID += 1;
	}

	// Set the tracks
	mCore->mDataStructureTracks.mTracks = &mTracks;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		//de.SetParticles(particles);	// TODO: what was this?
		de.SetMainImage(mCore->mDataStructureInput.mImage);
		de.SetTrajectories(true);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
