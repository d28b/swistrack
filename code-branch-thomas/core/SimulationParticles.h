#ifndef HEADER_SimulationParticles
#define HEADER_SimulationParticles

#include <string>
#include <list>

//! Reads and holds a simulation of particles.
class SimulationParticles: public CommunicationNMEAInterface {

public:
	//! A single particle.
	typedef struct {
		
	} tParticle;

	std::list<tParticles> mParticles;

	//! Constructor.
	SimulationParticles(): mParticles() {}
	//! Destructor.
	~SimulationParticles() {}

	//! Reads a simulation file.
	void Read(const std::string &filename);
};

#endif