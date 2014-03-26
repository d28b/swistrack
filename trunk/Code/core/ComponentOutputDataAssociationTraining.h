#ifndef HEADER_ComponentOutputDataAssociationTraining
#define HEADER_ComponentOutputDataAssociationTraining

#include "Component.h"
#include <fstream>

//! A component that writes particles to the communication interface. This information is usually transmitted to clients that connect via TCP to SwisTrack.
class ComponentOutputDataAssociationTraining: public Component {

public:
	//! Constructor.
	ComponentOutputDataAssociationTraining(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOutputDataAssociationTraining();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentOutputDataAssociationTraining(mCore);
	}

private:
	// Configuration
	wxString mFileName;	

	wxTimeSpan mWindowSize;
	double mMaxDistanceSquared; 


	std::fstream mFileStream;
	DataStructureParticles::tParticleVector mParticles;
	IplImage * mOutputImage;

	Display mDisplayOutput;	//!< The Display showing the last acquired image and the particles.

	void BufferParticles(const DataStructureParticles::tParticleVector * inputParticles);
	void FilterParticles();

	void OutputTrainingData(const DataStructureParticles::tParticleVector * inputParticles);
	
};

#endif
