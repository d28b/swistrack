#include "ComponentRosTest.h"
#define THISCLASS ComponentRosTest
using namespace std;
#include <iostream>
#include <errno.h>
#include <fstream>
#include "DisplayEditor.h"
#include "Utility.h"

THISCLASS::ComponentRosTest(SwisTrackCore *stc):
		Component(stc, wxT("RosTest")),
		mDisplayOutput(wxT("Output"), wxT("After tracking"))
{
	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDisplay(&mDisplayOutput);
	AddDataStructureRead(&(mCore->mDataStructureTracks));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentRosTest() {
}

void THISCLASS::OnStart() {
	int argc = 0;
	char **argv = NULL;
	ros::init(argc, argv, "ParticleTrack_cu");
	ros::NodeHandle n;
	track_pub = n.advertise<swistrack::ParticleTrack>("/ParticleTrack_cu", 1000);
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {

	// If there is no track, stop
	DataStructureTracks::tTrackMap *mTracks;
	mTracks = mCore->mDataStructureTracks.mTracks;
	if (! mTracks) {
		AddError(wxT("No tracks."));
		return;
	}

	// For each track, write data in the corresponding output file
	DataStructureTracks::tTrackMap::iterator it = mTracks->begin();
	while (it != mTracks->end()) {
		WriteData(it->first); // Is it->first the ID?
		it++;
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	// Kill the publisher.
	track_pub.shutdown();
}

void THISCLASS::WriteParticle(const Particle & p) {
	// Output this particle to ROS.
	//theTrack.header
	//theTrack.timeStamp
	swistrack::ParticleTrack theTrack;
	theTrack.ID = p.mID;
	theTrack.IDCovariance = p.mIDCovariance;
	theTrack.xCenter = p.mCenter.x;
	theTrack.yCenter = p.mCenter.y;
	theTrack.orientation = p.mOrientation;
	theTrack.area = p.mArea;
	theTrack.compactness = p.mCompactness;
	theTrack.xWorldCenter = p.mWorldCenter.x;
	theTrack.yWorldCenter = p.mWorldCenter.y;
	theTrack.frameNumber = p.mFrameNumber;

	track_pub.publish(theTrack);
}

void THISCLASS::WriteData(int ID) {
	// This was originally designed assuming each particles were for only one frame.
	// ComponentMinCostFlow tracking sends particles for many frames at once.
	// This loop still works if particles for each track are sorted in time.
	// So that's part of the spec now.
	//Search for the corresponding particle
	DataStructureParticles::tParticleVector *particles = mCore->mDataStructureParticles.mParticles;
	if (! particles) {
		AddError(wxT("There are no particles"));
		return;
	}

	DataStructureParticles::tParticleVector::iterator it = particles->begin();
	while (it != particles->end()) {
		// Correct ID is found
		if (it->mID == ID) {
			WriteParticle(*it);
		}
		it++;
	}
}

