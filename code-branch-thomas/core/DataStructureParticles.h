#ifndef HEADER_DataStructureParticles
#define HEADER_DataStructureParticles

#include <vector>
#include "cv.h"
#include "DataStructure.h"
#include "Particle.h"

class DataStructureParticles: public DataStructure {

public:
	//! Particle vector type.
	typedef std::vector<Particle> tParticleVector;

	//! Vector of particles.
	tParticleVector *mParticles;

	//! Constructor.
	DataStructureParticles(): DataStructure("Particles"), mParticles(0) {}
	~DataStructureParticles() {}
};

#endif

