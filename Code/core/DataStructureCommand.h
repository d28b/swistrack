#ifndef HEADER_DataStructureCommand
#define HEADER_DataStructureCommand

#include <vector>
#include <opencv2/opencv.hpp>
#include "DataStructure.h"
#include "Command.h"

// A DataStructure holding a list of particles.
class DataStructureCommand: public DataStructure {

public:
	//! Particle vector type.
	typedef std::vector<Command> tCommandVector;

	tCommandVector * mCommands;	//!< Vector of particles.

	//! Constructor.
	DataStructureCommand(): DataStructure(wxT("Commands"), wxT("Commands")), mCommands(0) {}
	//! Destructor.
	~DataStructureCommand() {}
};

#endif

