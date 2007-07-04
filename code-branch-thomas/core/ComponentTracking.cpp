#include "ComponentTracking.h"
#define THISCLASS ComponentTracking

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentTracking(SwisTrackCore *stc):
		Component(stc, "Tracking"),
		 mMaxNumber(10), mDistanceGate(0.1),
		mDisplayOutput("Output", "Tracking") {

	// Data structure relations
	mCategory=&(mCore->mCategoryBlobDetection);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();

	for (int i=0;i<mMaxNumber;i++)          // initiate nr_objects Track classes
	{
	 sharedage.push_back(0);
	 oldshared.push_back(0);
	 restingtraj.push_back(0);
	 mTracks.push_back(Track(i,					// id number
	 mMaxNumber));
	}
}

THISCLASS::~ComponentTracking() {
		if(mTracks.size()) mTracks.clear();
}

void THISCLASS::OnStart() {
	mMaxNumber=GetConfigurationInt("MaxNumber", 10);
	mDistanceGate=GetConfigurationDouble("DistanceGate",0.1);

	// Check for stupid configurations
	if (mMaxNumber<1) {
		AddError("Max number of tracks must be greater or equal to 1");
	}

	if (mDistanceGate<0) {
		AddError("The minimal distance (DistanceGate) cannot be smaller than zero");
	}
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	// Particles we want to associate
	DataStructureParticles::tParticleVector *particles = mCore->mDataStructureParticles.mParticles;






	// Set these particles
	mCore->mDataStructureTracks.mTracks=&mTracks;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		//de.SetTracks(&mTracks);
		de.SetMainImage(mCore->mDataStructureImageBinary.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles=0;
}

void THISCLASS::OnStop() {
}
