#ifndef HEADER_ComponentSimulateParticles
#define HEADER_ComponentSimulateParticles

#include "cv.h"
#include "Component.h"
#include "DataStructureParticles.h"
#include <vector>

//! Simulates particles
class ComponentSimulateParticles: public Component {

public:
	ComponentSimulateParticles(SwisTrackCore *stc);
	~ComponentSimulateParticles();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentSimulateParticles(mCore);}

private:
	CvSize mCameraOrigin;
	double mCameraRotation;
	double mCameraPixelSize;
	double mNoise;
	int mFile;

	std::vector<tRobot> mRobots;
	DataStructureParticles::tParticleVector mParticles;
};

#endif

