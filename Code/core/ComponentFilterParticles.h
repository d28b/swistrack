#ifndef HEADER_ComponentFilterParticles
#define HEADER_ComponentFilterParticles

#include <opencv2/core.hpp>
#include "Component.h"
#include "DataStructureParticles.h"

//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
class ComponentFilterParticles: public Component {

public:
	//! Constructor.
	ComponentFilterParticles(SwisTrackCore * stc);
	//! Destructor.
	~ComponentFilterParticles();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentFilterParticles(mCore);
	}

private:
	DataStructureParticles::tParticleVector mParticles;

	double mMaxParticleArea;
	double mMinParticleArea;

	Display mDisplayOutput;	//!< The Display showing the last acquired image and the particles.
};

#endif

