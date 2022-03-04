#ifndef HEADER_BlobMatching
#define HEADER_BlobMatching

#include <opencv2/core.hpp>
#include "Component.h"
#include "DataStructureParticles.h"

class BlobMatching {
private:
	double mMaxDistance;		//!< (configuration) The maximum distance (in pixels) between the two color blobs that make one particle.
	double mCenter;				//!< (configuration) Selects the center point (0.0 = color 1, 0.5 = middle, 1.0 = color 2).

public:
	DataStructureParticles::tParticleVector mParticles;		//!< The detected particles.

	BlobMatching();
	~BlobMatching() { }

	//! Reads the configuration
	void ReadConfiguration(Component * component);
	//! Find blobs on a binary image.
	void Match(DataStructureParticles::tParticleVector particleList1, DataStructureParticles::tParticleVector particleList2);
};

#endif
