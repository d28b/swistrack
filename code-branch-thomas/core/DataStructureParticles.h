#ifndef HEADER_DataStructureParticles
#define HEADER_DataStructureParticles

#include "DataStructure.h"
#include <vector>
#include "cv.h"

class DataStructureParticles: public DataStructure {

public:
	typedef std::vector<Particle> ParticleVector;

	//! Frame number
	ParticleVector mParticles;

	//! Constructor.
	DataStructureParticles(): DataStructure("Particles"), mFrameNumber(0), mFramesPerSecond(-1) {}
	~DataStructureParticles() {}
};

#endif

