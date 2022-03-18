#ifndef HEADER_ComponentOutputParticlesToFile
#define HEADER_ComponentOutputParticlesToFile

#include <vector>
#include <map>
#include <opencv2/core.hpp>
#include <fstream>
#include "Component.h"

class ComponentOutputParticlesToFile: public Component {

public:
	//! Constructor.
	ComponentOutputParticlesToFile(SwisTrackCore * stc);
	//! Destructor.
	~ComponentOutputParticlesToFile();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentOutputParticlesToFile(mCore);
	}

private:
	std::fstream mFile;			//!< File to be written.

};

#endif

