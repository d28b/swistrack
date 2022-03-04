#ifndef HEADER_DataStructureParticles
#define HEADER_DataStructureParticles

#include <vector>
#include <opencv2/core.hpp>
#include "DataStructure.h"
#include "Particle.h"

// A DataStructure holding a list of particles.
class DataStructureParticles: public DataStructure {

public:
	//! Particle vector type.
	typedef std::vector<Particle> tParticleVector;

	tParticleVector * mParticles;	//!< The current particles.

	//! Constructor.
	DataStructureParticles(): DataStructure(wxT("Particles"), wxT("Particles")), mParticles(0) {}
	//! Destructor.
	~DataStructureParticles() {}
};

#endif

